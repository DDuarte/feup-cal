#ifndef HYDOGRAPHIC_NETWORK_H_
#define HYDOGRAPHIC_NETWORK_H_

#include "graph.h"
#include "village.h"
#include "river.h"

class HydographicNetwork
{
public:
	HydographicNetwork() { }

private:
	Graph<Village, River> _graph;
};

#endif // HYDOGRAPHIC_NETWORK_H_
