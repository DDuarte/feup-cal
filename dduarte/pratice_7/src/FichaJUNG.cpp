#include <cstdio>
#include "graphviewer.h"
#include <fstream>
#include <iostream>
#include <sstream>

void exercicio1();
void exercicio2();
void exercicio3();


void exercicio1()
{
    GraphViewer* gv = new GraphViewer(600, 600, true);
    gv->setBackground("background.jpg");
    gv->createWindow(600, 600);

    gv->defineVertexColor(BLUE);
    gv->defineEdgeColor(BLACK);

    // i
    gv->addNode(0);

    // ii
    gv->addNode(1);

    // iii
    gv->addEdge(0, 0, 1, EdgeType::UNDIRECTED);

    // iv
    gv->removeNode(1);

    // v
    gv->addNode(2);

    // vi
    gv->addEdge(1, 0, 2, EdgeType::DIRECTED);

    // vii
    gv->setVertexLabel(2, "Isto é um nó.");

    // viii
    gv->setEdgeLabel(1, "Isto é uma <BR> aresta.");

    // ix
    gv->setVertexColor(2, GREEN);

    // x
    gv->setEdgeColor(1, YELLOW);

    gv->rearrange();
}

void exercicio2()
{
    GraphViewer* gv = new GraphViewer(600, 600, false);
    gv->createWindow(600, 600);

    gv->addNode(0,  300, 50);
    gv->addNode(1,  318, 58);
    gv->addNode(2,  325, 75);
    gv->addNode(3,  318, 93);
    gv->addNode(4,  300, 100);
    gv->addNode(5,  282, 93);
    gv->addNode(6,  275, 75);
    gv->addNode(7,  282, 58);
    gv->addNode(8,  150, 200);
    gv->addNode(9,  300, 200);
    gv->addNode(10, 450, 200);
    gv->addNode(11, 300, 400);
    gv->addNode(12, 200, 550);
    gv->addNode(13, 400, 550);

    gv->addEdge(0,  0,  1,  EdgeType::UNDIRECTED);
    gv->addEdge(1,  1,  2,  EdgeType::UNDIRECTED);
    gv->addEdge(2,  2,  3,  EdgeType::UNDIRECTED);
    gv->addEdge(3,  3,  4,  EdgeType::UNDIRECTED);
    gv->addEdge(4,  4,  5,  EdgeType::UNDIRECTED);
    gv->addEdge(5,  5,  6,  EdgeType::UNDIRECTED);
    gv->addEdge(6,  6,  7,  EdgeType::UNDIRECTED);
    gv->addEdge(7,  7,  0,  EdgeType::UNDIRECTED);
    gv->addEdge(8,  4,  9,  EdgeType::UNDIRECTED);
    gv->addEdge(9,  9,  8,  EdgeType::UNDIRECTED);
    gv->addEdge(10, 9,  10, EdgeType::UNDIRECTED);
    gv->addEdge(11, 9,  11, EdgeType::UNDIRECTED);
    gv->addEdge(12, 11, 12, EdgeType::UNDIRECTED);
    gv->addEdge(13, 11, 13, EdgeType::UNDIRECTED);


    gv->removeNode(12);
    gv->removeNode(13);

    int i = 14;
    while (true)
    {
        gv->addNode(i, 250, 550);
        gv->addNode(i + 1, 350, 550);
        gv->rearrange();

        Sleep(1000);

        gv->removeNode(i);
        gv->removeNode(i + 1);
        gv->rearrange();

        Sleep(1000);

        i += 2;
    }




}

void exercicio3()
{
    GraphViewer* gv = new GraphViewer(600, 600, false);
    gv->createWindow(600, 600);

    std::ifstream nodesFile("nos.txt");
    if (!nodesFile.is_open())
    {
        std::cout << "Can't open nos.txt" << std::endl;
        return;
    }


    while (nodesFile.good())
    {
        int id = -1;
        int x;
        int y;
        for (int i = 0; i < 3; ++i)
        {
            std::string line;
            std::getline(nodesFile, line, ';');

            if (!line.empty())
            {
                if (i == 0)
                    id = atoi(line.c_str());
                else if (i == 1)
                    x = atoi(line.c_str());
                else if (i == 2)
                    y = atoi(line.c_str());
            }
        }

        if (!nodesFile.good())
            break;

        if (id != -1)
            gv->addNode(id, x, y);
    }

    nodesFile.close();

    // ...

    std::ifstream edgesFile("arestas.txt");
    if (!edgesFile.is_open())
    {
        std::cout << "Can't open arestas.txt" << std::endl;
        return;
    }


    while (edgesFile.good())
    {
        int id = -1;
        int x;
        int y;
        for (int i = 0; i < 3; ++i)
        {
            std::string line;
            std::getline(edgesFile, line, ';');

            if (!line.empty())
            {
                if (i == 0)
                    id = atoi(line.c_str());
                else if (i == 1)
                    x = atoi(line.c_str());
                else if (i == 2)
                    y = atoi(line.c_str());
            }
        }

        if (!edgesFile.good())
            break;

        if (id != -1)
            gv->addEdge(id, x, y, EdgeType::UNDIRECTED);
    }

    edgesFile.close();

    gv->rearrange();
}


int main() {
    //exercicio1();
    //exercicio2();
    exercicio3();
    getchar();
    return 0;
}
