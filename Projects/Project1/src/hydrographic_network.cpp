#include "hydrographic_network.h"
#include "graphviewer.h"
#include "loader.h"
#include "menu.h"

#include <sstream>
#include <numeric>
#include <set>
#include <cassert>
#include <cmath>
#include <algorithm>

#define WHITESPACE_GRAPHVIEWER " \b" ///< hack to remove vertex and edge labels (empty string or only whitespace does not work)
#define WINDOW_WIDTH 800 ///< GraphViewer width
#define WINDOW_HEIGHT 600 ///< GraphViewer height
#define WINDOW_MARGIN 40 ///< Used when redimensioning the graph node's position

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

    Delivery::BoatMap numberOfBoats;
    for (Delivery::OrderMap::reference dest : orders)
    {
        uint ordersVolume = 0;
        uint ordersWeight = 0;

        for (std::vector<Order>::iterator it = dest.second.begin(); it != dest.second.end();)
        {
            uint orderVolume = it->GetVolume();
            if (orderVolume > boatCapacity)
            {
                auto unreacheableIt = unreacheable.find(dest.first);
                if (unreacheableIt == unreacheable.end())
                {
                    unreacheable.emplace(dest.first, std::vector<Order>());
                    unreacheableIt = unreacheable.find(dest.first);
                }

                unreacheableIt->second.push_back(*it);
                it = dest.second.erase(it);
                continue;
            }
            else
            {

                ordersVolume += it->GetVolume();
                ordersWeight += it->GetWeight();

                it++;
            }
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
            unreacheable.insert(it, reachableWithIgarapes.end());
            reachableWithIgarapes.erase(it, reachableWithIgarapes.end());
            break;
        }
    }

    for (Delivery::OrderMap::iterator it = reachableWithIgarapes.begin(); it != reachableWithIgarapes.end();)
    {
        Delivery::OrderMap::iterator eraseit = it++;
        if (eraseit->second.empty())
        {
            unreacheable.insert(std::move(*eraseit));
            reachableWithIgarapes.erase(eraseit);
        }
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
    std::vector<uint> topoOrder = TopologicalOrder();
    size_t number_of_times = orders.size() + reachableWithIgarapes.size();
    uint dijkstraSrc = src;
    for (size_t i = 0; i < number_of_times; ++i)
    {
        _igarapeMaxCapacity = boatCapacity;
        DijkstraPath shrtPath = DijkstraShortestPath(dijkstraSrc);
        DijkstraPath shrtPathWithIgarapes;
        if (needsSupportVessel)
        {
            _igarapeMaxCapacity = supportVesselCapacity;
            shrtPathWithIgarapes = DijkstraShortestPath(dijkstraSrc);
        }

        double minDist = std::numeric_limits<double>::infinity();

        std::vector<uint> minimumDists;

        for (DijkstraPath::const_reference elem : shrtPath)
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

        for (DijkstraPath::const_reference elem : shrtPathWithIgarapes)
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
                    path.back().isIgarape = path[i].isIgarape;
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

    return DeliveryRoute(std::move(returnPath), std::move(numberOfBoats), std::move(unreacheable));
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

    Delivery::BoatMap numberOfBoats;
    for (Delivery::OrderMap::reference dest : orders)
    {
        uint ordersVolume = 0;
        uint ordersWeight = 0;

        for (std::vector<Order>::iterator it = dest.second.begin(); it != dest.second.end();)
        {
            uint orderVolume = it->GetVolume();
            if (orderVolume > boatCapacity)
            {
                auto unreacheableIt = unreacheable.find(dest.first);
                if (unreacheableIt == unreacheable.end())
                {
                    unreacheable.emplace(dest.first, std::vector<Order>());
                    unreacheableIt = unreacheable.find(dest.first);
                }

                unreacheableIt->second.push_back(*it);
                it = dest.second.erase(it);
                continue;
            }
            else
            {

                ordersVolume += it->GetVolume();
                ordersWeight += it->GetWeight();

                it++;
            }
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
			auto itcopy = it;
			itcopy++;
			if (itcopy != reachableWithIgarapes.end())
			{
				unreacheable.insert(it, reachableWithIgarapes.end());
				reachableWithIgarapes.erase(it, reachableWithIgarapes.end());
				break;
			}
        }
    }

    for (Delivery::OrderMap::iterator it = reachableWithIgarapes.begin(); it != reachableWithIgarapes.end();)
    {
        Delivery::OrderMap::iterator eraseit = it++;
        if(eraseit->second.empty())
        {
            unreacheable.insert(std::move(*eraseit));
            reachableWithIgarapes.erase(eraseit);
        }
    }

    _igarapeMaxCapacity = boatCapacity;

    DeliveryRoute::PathInfoMap deliveries;
    DijkstraPath shrtPath = DijkstraShortestPath(src);
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

	if (delivery.GetNumberOfSupportVessels() <= 0 || reachableWithIgarapes.empty())
        return DeliveryRoute(std::move(deliveries), std::move(numberOfBoats), std::move(unreacheable));

    _igarapeMaxCapacity = supportVesselCapacity;

    shrtPath = DijkstraShortestPath(src);
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
            pi.isIgarape = pathInfos.back().isIgarape;

            pathInfos.push_back(pi);

            size_t destIndex = pathInfos.size() - 1;

            for (size_t i = destIndex - 1; i > igarapeIndex; --i)
            {
                pathInfos.push_back(pathInfos[i]);
                pathInfos.back().transportedWeight = 0;
                pathInfos.back().followsFlow = !pathInfos[i].followsFlow;
            }

            size_t endIndex = pathInfos.size() - 1;

			if (numberOfTravels >= 2)
				for (uint i = 0; i < numberOfTravels - 2; ++i)
				    pathInfos.insert(pathInfos.end(), pathInfos.begin() + igarapeIndex, pathInfos.begin() + endIndex + 1);
			if (numberOfTravels > 1)
				pathInfos.insert(pathInfos.end(), pathInfos.begin() + igarapeIndex, pathInfos.begin() + destIndex + 1);

            deliveries.insert(std::make_pair(dest.first, std::move(pathInfos)));
        }
    }

    return DeliveryRoute(std::move(deliveries), std::move(numberOfBoats), std::move(unreacheable));
}

void HydrographicNetwork::ViewGraph()
{
    if (!_graphViewer)
    {
        // new window
        _graphViewer = new GraphViewer(WINDOW_WIDTH, WINDOW_HEIGHT, false);
        //_graphViewer->setBackground("water1.jpg");
        _graphViewer->createWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    else
    {
        // remove existing nodes/edges
        for (auto& v : _vertices)
            _graphViewer->removeNode(v.first);

        for (auto& r : _rivers)
            _graphViewer->removeEdge(r.first);
    }

    //std::pair<std::map<uint, Vertex*>::iterator, std::map<uint, Vertex*>::iterator> minmaxVerticeX = std::minmax_element(_vertices.begin(), _vertices.end(), [] (std::map<uint, Vertex*>::const_reference ver1, std::map<uint, Vertex*>::const_reference ver2)
    //{
    //    return ver1.second->info.GetX() < ver2.second->info.GetX();
    //});
    //
    //auto minmaxVerticeY = std::minmax_element(_vertices.begin(), _vertices.end(), [] (std::map<uint, Vertex*>::const_reference ver1, std::map<uint, Vertex*>::const_reference ver2)
    //{
    //    return ver1.second->info.GetY() < ver2.second->info.GetY();
    //});
    //
    //double dX = static_cast<double>(WINDOW_WIDTH)  / (minmaxVerticeX.second->second->info.GetX() - minmaxVerticeX.first->second->info.GetX() + WINDOW_MARGIN);
    //double dY = static_cast<double>(WINDOW_HEIGHT) / (minmaxVerticeY.second->second->info.GetY() - minmaxVerticeY.first->second->info.GetY() + WINDOW_MARGIN);

    _graphViewer->defineVertexColor(DARK_GRAY);
    _graphViewer->defineEdgeColor(BLUE);

    for (std::map<uint, Vertex*>::value_type v : _vertices)
    {
        AddVillageToGraphViewer(v.first, Village(v.second->info) /*, dX, dY */);

        for (Edge& e : v.second->adj)
        {
            if (!e.weight.FollowsFlow)
                continue;

            AddRiverToGraphViewer(e.weight.RiverId, v.first, e.idDest, *GetRiver(e.weight.RiverId));
        }
    }

    _graphViewer->rearrange();
}

void HydrographicNetwork::ViewGraph(DeliveryRoute& delivery)
{
    if (_graphViewer == nullptr)
        ViewGraph();

    for (auto& v : _vertices) // remove labels and reset previous run
    {
        _graphViewer->setVertexLabel(v.first, WHITESPACE_GRAPHVIEWER);
        _graphViewer->setVertexColor(v.first, DARK_GRAY);
    }

    for (auto& r : _rivers) // remove labels
        _graphViewer->setEdgeLabel(r.first, WHITESPACE_GRAPHVIEWER);

    // set different colors for source and all destination villages
    for (std::pair<uint, std::vector<DeliveryRoute::PathInfo>> d : delivery.Path)
    {
        _graphViewer->setVertexColor(d.second.front().villageId, GREEN); // source
        _graphViewer->setVertexColor(d.first, RED); // destination
    }

    static uint tempEdgeId = _nextRiverId * 2 + 1;
    static uint addedEdgesCount = 0;

    if (addedEdgesCount != 0) // cleanup previous run
    {
        for (uint i = 0; i < addedEdgesCount; ++i)
            _graphViewer->removeEdge(_nextRiverId * 2 + 1 + i);

        addedEdgesCount = 0;
        tempEdgeId = _nextRiverId * 2 + 1;

        _graphViewer->rearrange();
    }

    for (std::pair<uint, std::vector<DeliveryRoute::PathInfo>> d : delivery.Path)
    {
        for (size_t i = 0; i < d.second.size() - 1; ++i)
        {
            _graphViewer->addEdge(tempEdgeId, d.second[i].villageId, d.second[i + 1].villageId, EdgeType::DIRECTED);
            _graphViewer->setEdgeColor(tempEdgeId, d.second[i].isIgarape ? ORANGE : BLACK);
            _graphViewer->setEdgeLabel(tempEdgeId, std::to_string(i + 1));

            // Animation:
            Sleep(800);
            _graphViewer->rearrange();

            tempEdgeId += 1;
            addedEdgesCount++;
        }
    }
}

std::vector<uint> HydrographicNetwork::TopologicalOrder() const
{
    std::vector<uint> res;

    if(!IsDag())
        return res;

    for (const auto& ver : _vertices)
        ver.second->indegree = 0;

    for (const auto& ver : _vertices)
        for (std::vector<Edge>::const_reference edge : ver.second->adj)
            if (edge.weight.FollowsFlow && _rivers.at(edge.weight.RiverId).GetMaxCapacity() >= _igarapeMaxCapacity)
                _vertices.at(edge.idDest)->indegree++;

    std::queue<uint> q;

    std::vector<uint> sources = GetSources();
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

    ResetIndegrees();

    return res;
}

int HydrographicNetwork::GetNumCycles() const
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

uint HydrographicNetwork::AddRiver(uint sourceVillage, uint destVillage, const River& river)
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

    _rivers.insert(std::make_pair(riverId, river));

    if (_graphViewer)
        AddRiverToGraphViewer(riverId, sourceVillage, destVillage, river);

    return riverId;
}

HydrographicNetwork::~HydrographicNetwork()
{
    if (_graphViewer)
        delete _graphViewer;
}

HydrographicNetwork* HydrographicNetwork::Load(ByteBuffer& bb)
{
    std::istringstream source(bb);

    HydrographicNetwork* hn = nullptr;

    try
    {
        std::string name;
        source >> name;

        hn = new HydrographicNetwork(name);

        uint villageCount;
        source >> villageCount;

        for (uint i = 0; i < villageCount; ++i)
        {
            uint id;
            double x; double y;
            std::string name;
            source >> id >> x >> y >> name;
            assert(hn->AddVillage(Village(name, x, y)) == id);
        }

        uint riverCount;
        source >> riverCount;

        for (uint i = 0; i < riverCount; ++i)
        {
            uint idS, idD, capacity;
            std::string name;
            source >> idS >> idD >> capacity >> name;
            hn->AddRiver(idS, idD, River(name, capacity));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception """ << e.what() << """ occurred when loading Hydrographic Network." << std::endl;
        return nullptr;
    }

    return hn;
}

void HydrographicNetwork::Save(ByteBuffer& bb)
{
    std::ostringstream ss;

    ss << GetName() << std::endl;

    // villages
    ss << this->GetNumberOfVertices() << std::endl;
    for (auto& v : _vertices)
        ss << v.first << ' ' << v.second->info.GetX() << ' ' << v.second->info.GetY() << ' ' << v.second->info.GetName() << std::endl;

    // rivers
    ss << this->_rivers.size() << std::endl;
    for (auto& v : _vertices)
    {
        for (auto& e : v.second->adj)
        {
            if (!e.weight.FollowsFlow)
                continue;

            ss << v.first << ' ' << e.idDest << ' '
               << _rivers.at(e.weight.RiverId).GetMaxCapacity() << ' '
               << _rivers.at(e.weight.RiverId).GetName() << std::endl;
        }
    }

    std::string str = ss.str();
    bb.WriteBuffer(str.c_str(), str.size());
}

Graph<Village, RiverEdge>::DijkstraPath HydrographicNetwork::DijkstraShortestPath(uint srcId) const
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

                    std::make_heap(pq.begin(),pq.end());
                }
            }
        }
    }

    DijkstraPath result;
    for (const auto& verAux : vertexAux)
        result.Vertices.insert(std::make_pair(verAux.first, DijkstraPath::DijkstraVertex(verAux.second.pathId, verAux.second.dist)));

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

bool HydrographicNetwork::RemoveVillage(uint id)
{
    if (RemoveVertex(id))
    {
        if (_graphViewer)
            _graphViewer->removeNode(id);
        return true;
    }

    return false;
}

uint HydrographicNetwork::AddVillage(const Village& info)
{
    uint id = AddVertex(info);

    if (_graphViewer)
        AddVillageToGraphViewer(id, info /*, 1.0, 1.0 */);

    return id;
}

void HydrographicNetwork::AddVillageToGraphViewer(uint id, const Village& info /*, double dX, double dY*/) const
{
    _graphViewer->addNode(id, static_cast<int>(info.GetX() /* * dX */), static_cast<int>(info.GetY() /* * dY */));
    _graphViewer->setVertexLabel(id, info.GetName() + " (" + std::to_string(id) + ")");
}

void HydrographicNetwork::AddRiverToGraphViewer(uint id, uint src, uint dest, const River& river) const
{
    _graphViewer->addEdge(id, src, dest, EdgeType::DIRECTED);
    _graphViewer->setEdgeThickness(id, 50 / river.GetMaxCapacity());
    _graphViewer->setEdgeLabel(id, river.GetName() + " (" + std::to_string(id) + ")");
}
