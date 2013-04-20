#ifndef RIVER_H_
#define RIVER_H_

#include <string>

class River
{
public:
	River(const std::string& name, int maxCapacity) : _name(name), _maxCapacity(maxCapacity) { }

	const std::string& GetName() const { return _name; }
	int GetMaxCapacity()  const { return _maxCapacity; }

	void SetName(const std::string& newName) { _name = newName; }
	void SetMaxCapacity(int newMaxCapacity) { _maxCapacity = newMaxCapacity; }

private:
	std::string _name;
	int			_maxCapacity;
};

#endif // RIVER_H_
