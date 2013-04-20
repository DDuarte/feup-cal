/*
 * labirinth.cpp
 */

#include "Labirinth.h"

#include <iostream>
using namespace std;



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




void Labirinth::printLabirinth()
{
	for (int i = 0; i < 10; i++)
	{
		for (int a = 0; a < 10; a++)
			cout << labirinth[i][a] << " ";

		cout << endl;
	}
}


bool Labirinth::findGoal(int x, int y)
{
    initializeVisited(x, y);
    foundTheSolution = false;
    return findGoalAux(x, y);
}

bool Labirinth::findGoalAux(int x, int y)
{
    int res = labirinth[x][y + 1]; // right
    if (res == OUT_OF_BOUNDS)
        return false;

    if (res == 2)
    {
        foundTheSolution = true;
        return true;
    }

    if (res == 1 && !visited[x][y + 1])
    {
        visited[x][y + 1] = true;
        return findGoal(x, y + 1);
    }

	return foundTheSolution;
}


