#ifndef DELIVERY_H_
#define DELIVERY_H_

#include "defs.h"

#include <vector>
#include <unordered_map>

class HydrographicNetwork;

struct Delivery
{
    struct PathInfo
    {
        uint villageId;
        double transportedWeight;
        bool followsFlow;
        bool isIgarape;
    };

	typedef std::unordered_map<uint, std::vector<Delivery::PathInfo>> PathInfoMap;

    Delivery(const PathInfoMap& path) : Path(path) { }
    Delivery(const PathInfoMap&& path) : Path(path) { }

    PathInfoMap Path;
    static Delivery Load(std::istream& source, HydrographicNetwork& hn);
};

#endif // DELIVERY_H_
