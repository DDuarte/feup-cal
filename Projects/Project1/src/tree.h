#ifndef TREE_H_
#define TREE_H_

#include "graph.h"

class Tree
{
private:
	Tree(int val, int dests, int treeLevel)
	{
		_val = val;
		for (int i = 1; i <= dests + treeLevel; ++i)
			if (i != val)
				_children.push_back(Tree(i, dests - 1, treeLevel + 1));
	}

public:
	Tree(int id, int dest) : Tree(id, dest, 0) {}

private:
	int _val;
	std::vector<Tree> _children;

};
#endif // TREE_H_
