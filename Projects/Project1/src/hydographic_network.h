#ifndef HYDOGRAPHIC_NETWORK_H_
#define HYDOGRAPHIC_NETWORK_H_

#include "graph.h"
#include "village.h"
#include "river.h"
#include "order.h"

#include <istream>
#include <vector>


class HydographicNetwork : public Graph<Village, River>
{
public:

	/*class MinCostMaxFlow : public Graph<Village, River> 
	{
	public:
		MinCostMaxFlow(const HydographicNetwork& hN, const Village& src, const std::map<Village, std::vector<Order> > ordersWithDests);
		MinCostMaxFlow(const HydographicNetwork& hN, const Village& src, const std::map<Village, std::vector<Order> > ordersWithDests);
	};*/
	
	/**
	 * HydographicNetwork
	 */
	HydographicNetwork() { }

	static HydographicNetwork Load(std::istream& source);

private:
};

#endif // HYDOGRAPHIC_NETWORK_H_
