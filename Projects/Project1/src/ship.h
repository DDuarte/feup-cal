#ifndef SHIP_H_
#define SHIP_H_

#include "order.h"

#include <string>

class Ship
{
public:
    Ship(const std::string& name, int capacity, int supportVesselCapacity = 0) : _name(name), _capacity(capacity), _supportVesselCapacity(supportVesselCapacity) { }

    std::string GetName() const { return _name; }
    int GetCapacity() const { return _capacity; }
    bool HasSupportVessel() const { return _supportVesselCapacity != 0; }
    int GetSupportVesselCapacity() const { return _supportVesselCapacity; }
private:
    const std::string _name;
    const int            _capacity;
    const int        _supportVesselCapacity;

};

#endif // SHIP_H_
