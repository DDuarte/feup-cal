/*
 * labirinth.h
 *
 */

#ifndef LABIRINTH_H_
#define LABIRINTH_H_

#include <vector>
#include <utility>

#define REACHED_GOAL 1
#define OK 0
#define OUT_OF_BOUNDS -1
#define WALL -2


class Labirinth {
    static const int SIZE = 10;
    int labirinth[SIZE][SIZE];
    bool visited[SIZE][SIZE];
    void initializeVisited(int x, int y);
    bool wasVisited(std::pair<int, int> pos);
    void visit(std::pair<int, int> pos);
    std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> pos);
    bool foundTheSolution;
public:
    Labirinth(int values[10][10]);
    void printLabirinth();
    bool findGoal(int x, int y);
};

#endif /* LABIRINTH_H_ */
