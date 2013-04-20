/*
 * labirinth.cpp
 */

#include "Labirinth.h"

#include <iostream>
#include <vector>
#include <stack>


Labirinth::Labirinth(int values[10][10])
{
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            labirinth[i][j]=values[i][j];
}


void Labirinth::initializeVisited(int x, int y)
{
    for (int i = 0; i < 10; i++)
        for (int a = 0; a < 10; a++)
            visited[i][a] = false;
    visited[y][x] = true;
}

void  Labirinth::printLabirinth()
{
    for (int i = 0; i < 10; i++)
    {
        for (int a = 0; a < 10; a++)
            std::cout << labirinth[i][a] << " ";

        std::cout << std::endl;
    }
}


bool Labirinth::findGoal(int x, int y)
{
    struct helper
    {
        std::pair<int, int> currentPosition;
        std::vector<std::pair<int,int>> neighbors;
    };

    if (labirinth[y][x] == 0)
        return false;

    std::stack<helper> stk;
    initializeVisited(x, y);
    helper h;
    h.currentPosition = std::make_pair(x,y);
    visit(h.currentPosition);
    h.neighbors = getNeighbors(h.currentPosition);
    bool found = false;

    do
    {
        bool success = false;
        std::pair<int, int> chosen;

        if (labirinth[h.currentPosition.second][h.currentPosition.first] == 2)
        {
            found = true;
            break;
        }

        while (!success && !h.neighbors.empty())
        {
            chosen = h.neighbors[h.neighbors.size() - 1];
            h.neighbors.erase(h.neighbors.begin() + h.neighbors.size() - 1);
            success = !wasVisited(chosen);
        }

        if (success)
        {
            stk.push(h);
            h.currentPosition = chosen;
            h.neighbors = getNeighbors(h.currentPosition);
            visit(h.currentPosition);
        }
        else if (!stk.empty())
        {
            h = stk.top();
            stk.pop();
        }
    } while (!stk.empty() && !found);

    return found;
}

bool Labirinth::wasVisited( std::pair<int, int> pos )
{
    return visited[pos.second][pos.first];
}

std::vector<std::pair<int, int>> Labirinth::getNeighbors( std::pair<int, int> pos )
{
    std::vector<std::pair<int, int>> result;

    if (pos.first > 0 && (labirinth[pos.second][pos.first - 1] != 0)) result.push_back(std::make_pair(pos.first - 1, pos.second));
    if (pos.second > 0 && (labirinth[pos.second - 1][pos.first] != 0)) result.push_back(std::make_pair(pos.first, pos.second - 1));
    if (pos.first < SIZE - 1 && (labirinth[pos.second][pos.first + 1] != 0)) result.push_back(std::make_pair(pos.first + 1, pos.second));
    if (pos.second < SIZE - 1 && (labirinth[pos.second + 1][pos.first] != 0)) result.push_back(std::make_pair(pos.first, pos.second + 1));

    return result;
}

void Labirinth::visit( std::pair<int, int> pos )
{
    visited[pos.second][pos.first]  = true;
}


