#include <cstdio>
#include "graphviewer.h"

int main() {
  GraphViewer *gv = new GraphViewer(600, 600, true, 7772);
  
  gv->createWindow(600, 600);  
  gv->defineEdgeColor("blue");
  gv->defineVertexColor("lightGray");

  gv->addNode(0);
  gv->addNode(1);
  gv->addNode(2);
  gv->addEdge(0, 0, 1, EdgeType::UNDIRECTED);
  gv->addEdge(3, 0, 1, EdgeType::UNDIRECTED);
  gv->addEdge(1, 0, 2, EdgeType::UNDIRECTED);
  gv->addEdge(2, 1, 2, EdgeType::DIRECTED);

  gv->setEdgeLabel(0, "Edge number 0");
  gv->setEdgeColor(1, "yellow");
  gv->setEdgeThickness(1, 5);

  gv->setVertexColor(0, "green");
 
  gv->rearrange();

  GraphViewer *gv2 = new GraphViewer(600, 600, true, 7773);
  
  gv2->createWindow(600, 600);  
  gv2->defineEdgeColor("green");
  gv2->defineVertexColor("yellow");

  gv2->addNode(0);
  gv2->addNode(1);
  gv2->addNode(2);
  gv2->addEdge(0, 0, 1, EdgeType::UNDIRECTED);
  gv2->addEdge(3, 0, 1, EdgeType::UNDIRECTED);
  gv2->addEdge(1, 0, 2, EdgeType::UNDIRECTED);
  gv2->addEdge(2, 1, 2, EdgeType::DIRECTED);

  gv2->setEdgeLabel(0, "Edge number 0");
  gv2->setEdgeColor(1, "yellow");
  gv2->setEdgeThickness(1, 5);

  gv2->setVertexColor(0, "green");
 
  gv2->rearrange();

  

  getchar();

  return 0;
}
