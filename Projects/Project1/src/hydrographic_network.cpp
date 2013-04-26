#include "hydrographic_network.h"
#include "graphviewer.h"
#include "loader.h"
#include "menu.h"

#include <numeric>
#include <set>
#include <cassert>
#include <cmath>

#define WHITESPACE_GRAPHVIEWER " \b" ///< hack to remove vertex and edge labels (empty string or only whitespace does not work)

Menu* HydrographicNetwork::_menu = Loader<Menu>("hydrographicBasinMenu.txt").Load();

DeliveryRoute HydrographicNetwork::GetDeliveryItinerary(Delivery& delivery)
{
    uint src = delivery.GetSourceVillage();
    Delivery::OrderMap& orders = delivery.GetOrders();
    double boatCapacity = delivery.GetBoatCapacity();
    double supportVesselCapacity = delivery.GetSupportVesselCapacity();
    uint numberOfSupportVessels = delivery.GetNumberOfSupportVessels();

    std::vector<DeliveryRoute::PathInfo> path;

    _igarapeMaxCapacity = boatCapacity;

    Delivery::OrderMap unreacheable = FilterUnreachable(src, orders);

    Delivery::OrderMap reachableWithIgarapes;

    if (numberOfSupportVessels > 0 && !unreacheable.empty())
    {
        _igarapeMaxCapacity = supportVesselCapacity;
        std::swap(unreacheable, reachableWithIgarapes);
        unreacheable = FilterUnreachable(src, reachableWithIgarapes);
    }

    Delivery::OrderMap::iterator it = orders.find(src);
    if (it != orders.end())
        orders.erase(it);

    BoatMap numberOfBoats;
    for (Delivery::OrderMap::reference dest : orders)
    {
        uint ordersVolume = 0;
        uint ordersWeight = 0;

        for (std::vector<Order>::iterator it = dest.second.begin(); it != dest.second.end(); it++)
        {
            uint orderVolume = it->GetVolume();
            if (orderVolume > boatCapacity)
            {
                it = dest.second.erase(it);
                it--;
                continue;
            }

            ordersVolume += it->GetVolume();
            ordersWeight += it->GetWeight();
        }

        numberOfBoats.emplace(dest.first, std::make_pair(static_cast<uint>(ceil(ordersVolume / static_cast<double>(boatCapacity))), ordersWeight));
    }

    for (Delivery::OrderMap::reference dest : reachableWithIgarapes)
    {
        double numberOfBoatsForOrder = 0.;
        uint ordersWeight = 0;

        for (std::vector<Order>::iterator it = dest.second.begin(); it != dest.second.end(); it++)
        {
            uint orderVolume = it->GetVolume();
            if (orderVolume > supportVesselCapacity)
            {
                it = dest.second.erase(it);
                if (!dest.second.empty() && it != dest.second.begin())
                    it--;
                else if(dest.second.empty())
                    break;
                continue;
            }

            double inc = ceil(orderVolume / static_cast<double>(boatCapacity));

            numberOfBoatsForOrder += inc;
            ordersWeight += it->GetWeight();
        }

        if (!dest.second.empty())
        {
            numberOfSupportVessels -= static_cast<uint>(ceil(numberOfBoatsForOrder));
            numberOfBoats.emplace(dest.first, std::make_pair(static_cast<uint>(ceil(numberOfBoatsForOrder)), ordersWeight));
        }

        if (numberOfSupportVessels == 0)
        {
            auto it = reachableWithIgarapes.find(dest.first);
            reachableWithIgarapes.erase(it, reachableWithIgarapes.end());
            break;
        }
    }

    for (Delivery::OrderMap::iterator it = reachableWithIgarapes.begin(); it != reachableWithIgarapes.end();)
    {
        Delivery::OrderMap::iterator eraseit = it++;
        if (eraseit->second.empty())
            reachableWithIgarapes.erase(eraseit);
    }

    double totalWeight = std::accumulate(orders.begin(), orders.end(), 0.0, [] (double val, Delivery::OrderMap::value_type o1)
    {
        for (const Order& ord : o1.second)
            val += ord.GetWeight();
        return val;
    });

    totalWeight += std::accumulate(reachableWithIgarapes.begin(), reachableWithIgarapes.end(), 0.0, [] (double val, Delivery::OrderMap::value_type o1)
    {
        for (const Order& ord : o1.second)
            val += ord.GetWeight();
        return val;
    });

    const bool needsSupportVessel = !reachableWithIgarapes.empty();

    _igarapeMaxCapacity = numberOfSupportVessels > 0 ? supportVesselCapacity : boatCapacity;
    std::vector<uint> topoOrder = topologicalOrder();
    size_t number_of_times = orders.size() + reachableWithIgarapes.size();
    uint dijkstraSrc = src;
    for (size_t i = 0; i < number_of_times; ++i)
    {
        _igarapeMaxCapacity = boatCapacity;
        DijkstraShortestPath shrtPath = dijkstraShortestPath(dijkstraSrc);
        DijkstraShortestPath shrtPathWithIgarapes;
        if (needsSupportVessel)
        {
            _igarapeMaxCapacity = supportVesselCapacity;
            shrtPathWithIgarapes = dijkstraShortestPath(dijkstraSrc);
        }

        double minDist = std::numeric_limits<double>::infinity();

        std::vector<uint> minimumDists;

        for (DijkstraShortestPath::const_reference elem : shrtPath)
        {
            if (elem.first != dijkstraSrc && (orders.find(elem.first) != orders.end()))
            {
                if (elem.second.Dist < minDist)
                {
                    minDist = elem.second.Dist;
                    minimumDists.erase(minimumDists.begin(), minimumDists.end());
                    minimumDists.push_back(elem.first);
                }
                else if (elem.second.Dist == minDist)
                    minimumDists.push_back(elem.first);
            }
        }

        for (DijkstraShortestPath::const_reference elem : shrtPathWithIgarapes)
        {
            if (elem.first != dijkstraSrc && (reachableWithIgarapes.find(elem.first) != reachableWithIgarapes.end()))
            {
                if (elem.second.Dist < minDist)
                {
                    minDist = elem.second.Dist;
                    minimumDists.erase(minimumDists.begin(), minimumDists.end());
                    minimumDists.push_back(elem.first);
                }
                else if (elem.second.Dist == minDist)
                    minimumDists.push_back(elem.first);
            }
        }

        auto nextElem = std::min_element(minimumDists.begin(), minimumDists.end(), [&topoOrder](uint const& r, uint const& s)
        {
            auto i = std::distance(topoOrder.begin(), std::find(topoOrder.begin(), topoOrder.end(), r));
            auto j = std::distance(topoOrder.begin(), std::find(topoOrder.begin(), topoOrder.end(), s));
            return i < j;
        });

        Delivery::OrderMap::iterator dest = reachableWithIgarapes.find(*nextElem);
        Path path1;
        if (dest != reachableWithIgarapes.end())
            path1 = shrtPathWithIgarapes.GetPath(dijkstraSrc, *nextElem);
        else
            path1 = shrtPath.GetPath(dijkstraSrc, *nextElem);

        if (path1.size() > 0)
        {
            if (dest == reachableWithIgarapes.end())
            {
                for (Path::const_iterator verIdIt = path1.begin(); verIdIt != (path1.end() - 1); verIdIt++)
                {
                    Path::const_reference verId = *verIdIt;
                    Path::const_reference nextVerId = *(verIdIt + 1);

                    DeliveryRoute::PathInfo pi;
                    pi.villageId = verId;
                    pi.transportedWeight = numberOfBoats.at(path1.back()).second;

                    const Vertex& ver = *_vertices.at(verId);

                    std::vector<Edge>::const_iterator it = std::find_if(ver.adj.begin(), ver.adj.end(), [nextVerId] (const Edge& edge)
                    {
                        return edge.idDest == nextVerId;
                    });

                    pi.followsFlow = it->weight.FollowsFlow;
                    pi.isIgarape = false;

                    path.push_back(pi);
                }

                dijkstraSrc = *nextElem;
            }
            else
            {
                uint totalVolume = std::accumulate(dest->second.begin(), dest->second.end(), 0, [](uint val, const Order& ord) { return val + ord.GetVolume(); });
                uint numberOfTravels = static_cast<uint>(ceil(totalVolume / supportVesselCapacity));

                Path::const_iterator igarapeSrc = path1.end();
                size_t igarapeIndex = -1;
                size_t destIndex = -1;

                for (Path::const_iterator verIdIt = path1.begin(); verIdIt != (path1.end() - 1); verIdIt++)
                {
                    Path::const_reference verId = *verIdIt;
                    Path::const_reference nextVerId = *(verIdIt + 1);

                    DeliveryRoute::PathInfo pi;
                    pi.villageId = verId;


                    const Vertex& ver = *_vertices.at(verId);

                    std::vector<Edge>::const_iterator it = std::find_if(ver.adj.begin(), ver.adj.end(),
                        [nextVerId] (const Edge& edge)
                    {
                        return edge.idDest == nextVerId;
                    });

                    pi.followsFlow = it->weight.FollowsFlow;

                    RiversContainer::const_iterator riverIt = _rivers.find(it->weight.RiverId);

                    uint cap = riverIt->second.GetMaxCapacity();

                    pi.isIgarape = cap >= supportVesselCapacity && cap < boatCapacity;

                    if (igarapeSrc == path1.end() && pi.isIgarape)
                    {
                        igarapeSrc = verIdIt;
                        igarapeIndex = path.size();
                    }

                    pi.transportedWeight = (igarapeSrc == path1.end() ? numberOfBoats.at(path1.back()).second : numberOfBoats.at(path1.back()).second / numberOfTravels);

                    path.push_back(pi);
                }

                destIndex = path.size() - 1;

                DeliveryRoute::PathInfo pi;
                pi.villageId = path1.back();
                pi.followsFlow = !path.back().followsFlow;
                pi.transportedWeight = 0;
                pi.isIgarape = path.back().isIgarape;

                path.push_back(pi);


                size_t endIndex = path.size() - 2;

                for (size_t i = endIndex; i > igarapeIndex; --i)
                {
                    path.push_back(path[i]);
                    path.back().transportedWeight = 0;
                    path.back().followsFlow = !path[i].followsFlow;
                }

                endIndex = path.size() - 1;

                for (uint i = 0; i < numberOfTravels - 1; ++i)
                    path.insert(path.end(), path.begin() + igarapeIndex, path.begin() + endIndex + 1);

                dijkstraSrc = path[igarapeIndex].villageId;
            }
        }

        std::unordered_map<uint, std::vector<Order>>::iterator srcOrderIt;
        if (*nextElem != src && ((srcOrderIt = orders.find(*nextElem)) != orders.end()))
        {
            for (const Order& ord : srcOrderIt->second)
                totalWeight -= ord.GetWeight();
            orders.erase(srcOrderIt);
        }

        if (*nextElem != src && ((srcOrderIt = reachableWithIgarapes.find(*nextElem)) != reachableWithIgarapes.end()))
        {
            for (const Order& ord : srcOrderIt->second)
                totalWeight -= ord.GetWeight();
            reachableWithIgarapes.erase(srcOrderIt);
        }

        if (path1.size() != 0 && i == (number_of_times - 1))
        {
            DeliveryRoute::PathInfo pi;
            pi.villageId = dijkstraSrc;
            pi.transportedWeight = 0.;
            path.push_back(pi);
        }
    }

    _igarapeMaxCapacity = 0.;

    std::unordered_map<uint, std::vector<DeliveryRoute::PathInfo>> returnPath;
    if (!path.empty())
        returnPath.insert(std::make_pair(path.back().villageId, path));

    return returnPath;
}

DeliveryRoute HydrographicNetwork::GetDeliveryPath(Delivery& delivery)
{
    uint src = delivery.GetSourceVillage();
    Delivery::OrderMap& orders = delivery.GetOrders();
    double boatCapacity = delivery.GetBoatCapacity();
    double supportVesselCapacity = delivery.GetSupportVesselCapacity();
    uint numberOfSupportVessels = delivery.GetNumberOfSupportVessels();

    if (_vertices.empty())
        throw std::runtime_error("No Villages found!");

    // Test to see which destinations are reachable without transversing igarapes
    _igarapeMaxCapacity = boatCapacity;

    Delivery::OrderMap unreacheable = FilterUnreachable(src, orders);

    Delivery::OrderMap reachableWithIgarapes;

    if (numberOfSupportVessels > 0 && !unreacheable.empty())
    {
        _igarapeMaxCapacity = supportVesselCapacity;
        std::swap(unreacheable, reachableWithIgarapes);
        unreacheable = FilterUnreachable(src, reachableWithIgarapes);
    }

    Delivery::OrderMap::iterator it = orders.find(src);
    if (it != orders.end())
        orders.erase(it);

    BoatMap numberOfBoats;
    for (Delivery::OrderMap::reference dest : orders)
    {
        uint ordersVolume = 0;
        uint ordersWeight = 0;

        for (std::vector<Order>::iterator it = dest.second.begin(); it != dest.second.end(); it++)
        {
            uint orderVolume = it->GetVolume();
            if (orderVolume > boatCapacity)
            {
                it = dest.second.erase(it);
                it--;
                continue;
            }

            ordersVolume += it->GetVolume();
            ordersWeight += it->GetWeight();
        }

        numberOfBoats.emplace(dest.first, std::make_pair(static_cast<uint>(ceil(ordersVolume / static_cast<double>(boatCapacity))), ordersWeight));
    }

    for (Delivery::OrderMap::reference dest : reachableWithIgarapes)
    {
        double numberOfBoatsForOrder = 0.;
        uint ordersWeight = 0;

        for (std::vector<Order>::iterator it = dest.second.begin(); it != dest.second.end(); it++)
        {
            uint orderVolume = it->GetVolume();
            if (orderVolume > supportVesselCapacity)
            {
                it = dest.second.erase(it);
                if (!dest.second.empty() && it != dest.second.begin())
                    it--;
                else if(dest.second.empty())
                    break;
                continue;
            }

            double inc = ceil(orderVolume / static_cast<double>(boatCapacity));

            numberOfBoatsForOrder += inc;
            ordersWeight += it->GetWeight();
        }

        if (!dest.second.empty())
        {
            numberOfSupportVessels -= static_cast<uint>(ceil(numberOfBoatsForOrder));
            numberOfBoats.emplace(dest.first, std::make_pair(static_cast<uint>(ceil(numberOfBoatsForOrder)), ordersWeight));
        }

        if (numberOfSupportVessels == 0)
        {
            auto it = reachableWithIgarapes.find(dest.first);
            reachableWithIgarapes.erase(it, reachableWithIgarapes.end());
            break;
        }
    }

    for (Delivery::OrderMap::iterator it = reachableWithIgarapes.begin(); it != reachableWithIgarapes.end();)
    {
        Delivery::OrderMap::iterator eraseit = it++;
        if(eraseit->second.empty())
        {
            reachableWithIgarapes.erase(eraseit);
        }
    }

    _igarapeMaxCapacity = boatCapacity;

    DeliveryRoute::PathInfoMap deliveries;
    DijkstraShortestPath shrtPath = dijkstraShortestPath(src);
    for (Delivery::OrderMap::const_reference dest : orders)
    {
        Path path = shrtPath.GetPath(src, dest.first);

        std::vector<DeliveryRoute::PathInfo> pathInfos;

        if (path.size() > 0)
        {
            for (Path::const_iterator verIdIt = path.begin(); verIdIt != (path.end() - 1); verIdIt++)
            {
                Path::const_reference verId = *verIdIt;
                Path::const_reference nextVerId = *(verIdIt + 1);

                DeliveryRoute::PathInfo pi;
                pi.villageId = verId;
                pi.transportedWeight = numberOfBoats.at(path.back()).second;

                const Vertex& ver = *_vertices.at(verId);

                std::vector<Edge>::const_iterator it = std::find_if(ver.adj.begin(), ver.adj.end(), [nextVerId] (const Edge& edge)
                {
                    return edge.idDest == nextVerId;
                });

                pi.followsFlow = it->weight.FollowsFlow;
                pi.isIgarape = false;

                pathInfos.push_back(pi);
            }

            DeliveryRoute::PathInfo pi;
            pi.villageId = path.back();
            pi.followsFlow = false;
            pi.transportedWeight = 0;
            pi.isIgarape = false;

            pathInfos.push_back(pi);
        }

        deliveries.insert(std::make_pair(dest.first, std::move(pathInfos)));
    }

    if (numberOfSupportVessels <= 0 || reachableWithIgarapes.empty())
        return DeliveryRoute(std::move(deliveries));

    _igarapeMaxCapacity = supportVesselCapacity;

    shrtPath = dijkstraShortestPath(src);
    for (Delivery::OrderMap::const_reference dest : reachableWithIgarapes)
    {
        Path path = shrtPath.GetPath(src, dest.first);
        uint totalVolume = std::accumulate(dest.second.begin(), dest.second.end(), 0, [](uint val, const Order& ord) { return val + ord.GetVolume(); });
        uint numberOfTravels = static_cast<uint>(ceil(totalVolume / supportVesselCapacity));
        std::vector<DeliveryRoute::PathInfo> pathInfos;

        Path::const_iterator igarapeSrc = path.end();
        size_t igarapeIndex = -1;
        size_t destIndex = -1;

        if (path.size() > 0)
        {
            for (Path::const_iterator verIdIt = path.begin(); verIdIt != (path.end() - 1); verIdIt++)
            {
                Path::const_reference verId = *verIdIt;
                Path::const_reference nextVerId = *(verIdIt + 1);

                DeliveryRoute::PathInfo pi;
                pi.villageId = verId;


                const Vertex& ver = *_vertices.at(verId);

                std::vector<Edge>::const_iterator it = std::find_if(ver.adj.begin(), ver.adj.end(),
                    [nextVerId] (const Edge& edge)
                {
                    return edge.idDest == nextVerId;
                });

                pi.followsFlow = it->weight.FollowsFlow;

                RiversContainer::const_iterator riverIt = _rivers.find(it->weight.RiverId);

                uint cap = riverIt->second.GetMaxCapacity();

                pi.isIgarape = cap >= supportVesselCapacity && cap < boatCapacity;

                if (igarapeSrc == path.end() && pi.isIgarape)
                {
                    igarapeSrc = verIdIt;
                    igarapeIndex = pathInfos.size();
                }

                pi.transportedWeight = (igarapeSrc == path.end() ? numberOfBoats.at(path.back()).second : numberOfBoats.at(path.back()).second / numberOfTravels);

                pathInfos.push_back(pi);
            }

            DeliveryRoute::PathInfo pi;
            pi.villageId = path.back();
            pi.followsFlow = !pathInfos.back().followsFlow;
            pi.transportedWeight = 0;
            pi.isIgarape = pathInfos.back().followsFlow;

            pathInfos.push_back(pi);


            size_t destIndex = pathInfos.size() - 1;

            for (size_t i = destIndex - 1; i > igarapeIndex; --i)
            {
                pathInfos.push_back(pathInfos[i]);
                pathInfos.back().transportedWeight = 0;
                pathInfos.back().followsFlow = !pathInfos[i].followsFlow;
            }

            size_t endIndex = pathInfos.size() - 1;

            for (uint i = 0; i < numberOfTravels - 2; ++i)
                pathInfos.insert(pathInfos.end(), pathInfos.begin() + igarapeIndex, pathInfos.begin() + endIndex + 1);

            pathInfos.insert(pathInfos.end(), pathInfos.begin() + igarapeIndex, pathInfos.begin() + destIndex + 1);

            deliveries.insert(std::make_pair(dest.first, std::move(pathInfos)));
        }


    }

    return DeliveryRoute(std::move(deliveries));
}

void HydrographicNetwork::ViewGraph()
{
    if (!_graphViewer)
    {
        // new window
        _graphViewer = new GraphViewer(800, 600, false);
        //_graphViewer->setBackground("water1.jpg");
        _graphViewer->createWindow(800, 600);
    }
    else
    {
        // remove existing nodes/edges
        for (auto& v : _vertices)
            _graphViewer->removeNode(v.first);

        for (auto& r : _rivers)
            _graphViewer->removeEdge(r.first);
    }

    _graphViewer->defineVertexColor(DARK_GRAY);
    _graphViewer->defineEdgeColor(BLUE);

    for (std::map<uint, Vertex*>::value_type v : _vertices)
    {
        _graphViewer->addNode(v.first, static_cast<int>(v.second->info.GetX()), static_cast<int>(v.second->info.GetY()));
        _graphViewer->setVertexLabel(v.first, v.second->info.GetName());

        for (Edge& e : v.second->adj)
        {
            if (!e.weight.FollowsFlow)
                continue;

            _graphViewer->addEdge(_tempEdgeId, v.first, e.idDest, EdgeType::DIRECTED);
            _graphViewer->setEdgeThickness(_tempEdgeId, 50 / GetRiver(e.weight.RiverId)->GetMaxCapacity());
            _graphViewer->setEdgeLabel(_tempEdgeId, GetRiver(e.weight.RiverId)->GetName());

            _tempEdgeId += 1;
        }
    }

    _graphViewer->rearrange();
}

void HydrographicNetwork::ViewGraph(DeliveryRoute& delivery, const std::string& color)
{
    if (_graphViewer == nullptr)
        return; // throw?

    for (auto& v : _vertices)
        _graphViewer->setVertexLabel(v.first, WHITESPACE_GRAPHVIEWER);

    for (auto& r : _rivers)
        _graphViewer->setEdgeLabel(r.first, WHITESPACE_GRAPHVIEWER);

    for (std::pair<uint, std::vector<DeliveryRoute::PathInfo>> d : delivery.Path)
    {
        _graphViewer->setVertexColor(d.second.front().villageId, GREEN); // source
        _graphViewer->setVertexColor(d.first, RED); // destination
    }

    for (std::pair<uint, std::vector<DeliveryRoute::PathInfo>> d : delivery.Path)
    {
        for (size_t i = 0; i < d.second.size() - 1; ++i)
        {
            _graphViewer->addEdge(_tempEdgeId, d.second[i].villageId, d.second[i + 1].villageId, EdgeType::DIRECTED);
            _graphViewer->setEdgeColor(_tempEdgeId, color);
            _graphViewer->setEdgeLabel(_tempEdgeId, std::to_string(i + 1));

            // Animation:
            Sleep(800);
            _graphViewer->rearrange();

            _tempEdgeId += 1;
        }

        _graphViewer->rearrange();
    }
}

std::vector<uint> HydrographicNetwork::topologicalOrder() const
{
    std::vector<uint> res;

    if(!isDag())
        return res;

    for (const auto& ver : _vertices)
        ver.second->indegree = 0;

    for (const auto& ver : _vertices)
        for (std::vector<Edge>::const_reference edge : ver.second->adj)
            if (edge.weight.FollowsFlow && _rivers.at(edge.weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
                _vertices.at(edge.idDest)->indegree++;

    std::queue<uint> q;

    std::vector<uint> sources = getSources();
    while(!sources.empty())
    {
        q.push(sources.back());
        sources.pop_back();
    }

    while(!q.empty())
    {
        uint vertexId = q.front();
        q.pop();

        res.push_back(vertexId);

        for(const Edge& edge : _vertices.at(vertexId)->adj)
        {
            if (edge.weight.FollowsFlow && _rivers.at(edge.weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
            {
                Vertex* dest = _vertices.at(edge.idDest);
                dest->indegree--;
                if(dest->indegree == 0)
                    q.push( edge.idDest );
            }
        }
    }

    if (res.size() != _vertices.size())
        res.erase(res.begin(), res.end());

    resetIndegrees();

    return res;
}

int HydrographicNetwork::getNumCycles() const
{
    int result = 0;
    std::map<uint, bool> visitedVertices;
    std::map<uint, bool> processingVertices;

    for (const auto& ver: _vertices)
    {
        visitedVertices[ver.first] = false;
        processingVertices[ver.first] = false;
    }

    std::stack<uint> stateStk;

    for (std::map<uint, Vertex*>::const_reference elem: _vertices)
    {
        Vertex* vertex = elem.second;
        uint vertexId = elem.first;
        do
        {
            visitedVertices[vertexId] = true;
            processingVertices[vertexId] = true;
            stateStk.push(vertexId);

            for (std::vector<Edge>::reverse_iterator edgeIt = vertex->adj.rbegin(); edgeIt != vertex->adj.rend(); edgeIt++)
            {
                if (edgeIt->weight.FollowsFlow && _rivers.at(edgeIt->weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
                {
                    if (! visitedVertices[edgeIt->idDest])
                        stateStk.push(edgeIt->idDest);
                    else if (processingVertices[edgeIt->idDest])
                        result++;
                }
            }

            while (!stateStk.empty())
            {
                vertexId = stateStk.top();
                vertex = _vertices.at(vertexId);
                stateStk.pop();

                if (processingVertices[vertexId])
                    processingVertices[vertexId] = false;
                else break;
            }

        }
        while (!stateStk.empty());
    }

    return result;
}

uint HydrographicNetwork::AddRiver(uint sourceVillage, uint destVillage, const River& weight)
{
    uint riverId = _nextRiverId++;

    double x1 = _vertices[sourceVillage]->info.GetX();
    double y1 = _vertices[sourceVillage]->info.GetY();

    double x2 = _vertices[destVillage]->info.GetX();
    double y2 = _vertices[destVillage]->info.GetY();

    double d = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    if (AddEdge(sourceVillage, destVillage, RiverEdge(d, riverId, true)))
    {
        if (!AddEdge(destVillage, sourceVillage, RiverEdge(d, riverId, false)))
        {
            RemoveEdge(sourceVillage, destVillage);
            _nextRiverId--;
            return -1;
        }
    }

    _rivers.insert(std::make_pair(riverId, weight));
    return riverId;
}

HydrographicNetwork::~HydrographicNetwork()
{
    if (_graphViewer)
        delete _graphViewer;
}

HydrographicNetwork* HydrographicNetwork::Load(ByteBuffer& bb)
{
    return nullptr;
}

/*
bool HydrographicNetwork::Load(std::istream& source, HydrographicNetwork& hn)
{
    try
    {
        uint villageCount;
        source >> villageCount;

        for (uint i = 0; i < villageCount; ++i)
        {
            uint id;
            double x, y;
            std::string name;
            source >> id >> x >> y >> name;
            x *= 15;
            y *= 15;
            assert(hn.AddVillage(Village(name, x, y)) == id);
        }

        uint riverCount;
        source >> riverCount;

        for (uint i = 0; i < riverCount; ++i)
        {
            uint idS, idD, capacity;
            std::string name;
            source >> idS >> idD >> capacity >> name;
            hn.AddRiver(idS, idD, River(name, capacity));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception """ << e.what() << """ occurred when loading Hydrographic Network." << std::endl;
        return false;
    }

    return true;
}
*/

Graph<Village, RiverEdge>::DijkstraShortestPath HydrographicNetwork::dijkstraShortestPath(uint srcId) const
{
    struct VertexAux
    {
        VertexAux(uint p = std::numeric_limits<uint>::max(), double d = std::numeric_limits<double>::infinity(), bool pr = false) : pathId(p), dist(d), processing(pr) { }
        uint pathId;
        double dist;
        bool processing;
    };

    std::map<uint, VertexAux> vertexAux;

    for (const auto& ver : _vertices)
        vertexAux[ver.first] = VertexAux();

    uint v = srcId;
    vertexAux[v].dist = 0;

    std::vector<uint> pq;
    pq.push_back(v);

    std::make_heap(pq.begin(), pq.end());

    while (! pq.empty())
    {
        v = pq.front();
        pop_heap(pq.begin(), pq.end());
        pq.pop_back();

        VertexAux& vAux = vertexAux[v];
        vAux.processing = false;

        for (std::vector<Edge>::const_reference edge : _vertices.at(v)->adj)
        {
            if (_rivers.at(edge.weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
            {
                uint w = edge.idDest;
                VertexAux& wAux = vertexAux[w];

                if (vAux.dist + edge.weight.GetWeight() < wAux.dist)
                {
                    wAux.dist = vAux.dist + edge.weight.GetWeight();
                    wAux.pathId = v;

                    if (! wAux.processing)
                    {
                        wAux.processing = true;
                        pq.push_back(w);
                    }

                    std::make_heap (pq.begin(),pq.end());
                }
            }
        }
    }

    DijkstraShortestPath result;
    for (const auto& verAux : vertexAux)
        result.Vertices.insert(std::make_pair(verAux.first, DijkstraShortestPath::DijkstraVertex(verAux.second.pathId, verAux.second.dist)));

    return result;
}

std::unordered_set<uint> HydrographicNetwork::GetVisitable(uint srcId) const
{
    std::unordered_set<uint> result;
    std::queue<uint> vers;
    vers.push(srcId);

    while(!vers.empty())
    {
        uint ver = vers.front();
        vers.pop();

        result.insert(ver);

        for (std::vector<Edge>::const_reference edge : _vertices.at(ver)->adj)
            if ((result.find(edge.idDest) == result.end()) && _rivers.at(edge.weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
                vers.push(edge.idDest);
    }

    return result;
}

void HydrographicNetwork::ChangeRiversCapacity(double factor)
{
    if (factor != 1.0)
        for (RiversContainer::iterator rit = _rivers.begin(); rit != _rivers.end(); rit++)
            rit->second.MultiplyMaxCapacity(factor);
}

Delivery::OrderMap HydrographicNetwork::FilterUnreachable(uint src, Delivery::OrderMap& orders)
{
    VertexSet visitable = GetVisitable(src);

    Delivery::OrderMap unreacheable;

    std::swap(unreacheable, orders);
    for (VertexSet::value_type verId : visitable)
    {
        Delivery::OrderMap::iterator val = unreacheable.find(verId);
        if (val != unreacheable.end())
        {
            orders.insert(std::make_pair(verId, std::move(unreacheable[verId])));
            unreacheable.erase(verId);
        }
    }

    return std::move(unreacheable);
}

const Village* HydrographicNetwork::GetVillage(uint id) const
{
    auto it = _vertices.find(id);
    if (it == _vertices.end())
        return nullptr;
    else
        return &(*it).second->info;
}

const River* HydrographicNetwork::GetRiver(uint id) const
{
    auto it = _rivers.find(id);
    if (it == _rivers.end())
        return nullptr;
    else
        return &(*it).second;
}
