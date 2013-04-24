#include "cute.h"
#include "cute_runner.h"
#include "ide_listener.h"

#undef max

#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <ctime>
#include <unordered_map>

#include "defs.h"
#include "hydrographic_network.h"
#include "delivery.h"

#define HN_FILE "graph.txt"

Delivery LoadOrdersFile(const std::string& fileName, HydrographicNetwork& hn)
{
    std::ifstream order1(fileName);
    if (!order1.is_open())
    {
        std::cerr << "Could not open file " << fileName << " for reading." << std::endl;
        return Delivery(std::unordered_map<uint, std::vector<Delivery::PathInfo>>());
    }

    return Delivery::Load(order1, hn);
}

int main(int argc, char** argv)
{
    std::ifstream hnFile(HN_FILE);
    if (!hnFile.is_open())
    {
        std::cerr << "Could not open file " << HN_FILE << " for reading." << std::endl;
        system("PAUSE");
        return EXIT_FAILURE;
    }

    HydrographicNetwork hn;
    if (!HydrographicNetwork::Load(hnFile, hn))
    {
        std::cerr << "Could not read Hydrographic Network from file " << HN_FILE << "." << std::endl;
        system("PAUSE");
        return EXIT_FAILURE;
    }

    hn.ViewGraph();

    system("PAUSE");

    std::string order1FileName = "order1.txt";
    hn.ViewGraph(LoadOrdersFile(order1FileName, hn), BLACK);

    system("PAUSE");

    //std::string order2FileName = "order2.txt";
    //hn.ViewGraph(LoadOrdersFile(order2FileName, hn), YELLOW);

    //system("PAUSE");

    //std::string order3FileName = "order3.txt";
    //hn.ViewGraph(LoadOrdersFile(order3FileName, hn), BLACK);

    system("PAUSE");

    hn.ViewGraph();

    system("PAUSE");

    hn.GetGraphViewer()->closeWindow();

    return 0;
}
