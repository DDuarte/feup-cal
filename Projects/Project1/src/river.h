#ifndef RIVER_H_
#define RIVER_H_

#include <string>

class River
{
public:
    River(const std::string& name, uint maxCapacity) : _name(name), _maxCapacity(static_cast<double>(maxCapacity)) { }
    River(const River& other) : _name(other._name), _maxCapacity(other._maxCapacity) { }

    const std::string& GetName() const { return _name; }
    uint GetMaxCapacity()  const { return static_cast<uint>(_maxCapacity); }

    void MultiplyMaxCapacity(double factor) { _maxCapacity *= factor; }

private:
    const std::string _name;
    double _maxCapacity;
};

#endif // RIVER_H_
