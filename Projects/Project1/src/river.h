#ifndef RIVER_H_
#define RIVER_H_

#include <string>

class River
{
public:
    River(const std::string& name = "", double maxCapacity = 0.0) : _name(name), _maxCapacity(maxCapacity) { }

    const std::string& GetName() const { return _name; }
    double GetMaxCapacity()  const { return _maxCapacity; }

    void SetName(const std::string& newName) { _name = newName; }
	void SetMaxCapacity(double newMaxCapacity) { _maxCapacity = newMaxCapacity; }

private:
    std::string _name;
    double            _maxCapacity;
};

#endif // RIVER_H_
