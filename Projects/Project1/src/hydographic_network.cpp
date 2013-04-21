#include "hydographic_network.h"

#include "graphviewer.h"

HydographicNetwork HydographicNetwork::Load(std::istream& source)
{
    return HydographicNetwork();
}

void HydographicNetwork::ViewGraph() const
{
    GraphViewer gv(600, 600, true);
    gv.createWindow(600, 600);

    gv.defineVertexColor(DARK_GRAY);
    gv.defineEdgeColor(BLUE);

    int edgeId = 1;

    for (std::map<uint, Vertex*>::value_type v : _vertices)
    {
        gv.addNode(v.first);
        gv.setVertexLabel(v.first, v.second->info.GetName());
        
        for (Edge& e : v.second->adj)
        {
            gv.addEdge(edgeId, v.first, e.idDest, EdgeType::DIRECTED);
            gv.setEdgeThickness(edgeId, 50 / e.weight.GetMaxCapacity());
            gv.setEdgeLabel(edgeId, e.weight.GetName());

            edgeId += 1;
        }
    }

    gv.rearrange();
}
