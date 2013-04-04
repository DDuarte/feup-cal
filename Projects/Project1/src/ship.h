#ifndef SHIP_H_
#define SHIP_H_

#include "order.h"

#include <string>

class Ship
{
public:
	Ship(const std::string& name, int capacity, bool hasSupportVessel) : _name(name), _capacity(capacity), _hasSupportVessel(hasSupportVessel) { }

	std::string GetName() const { return _name; }
	int GetCapacity() const { return _capacity; }
	bool GetHasSupportVessel() const { return _hasSupportVessel; }

	void SetName(const std::string& val) { _name = val; }
	void SetCapacity(int val) { _capacity = val; }
	void SetHasSupportVessel(bool val) { _hasSupportVessel = val; }

	/**
	 * Comsumption
	 * @param order
	 * @param followsFlow
	 * @return [double]
	 */
	double Comsumption(const Order& order, bool followsFlow);

private:
	std::string _name;
	int			_capacity;
	bool		_hasSupportVessel;

};

#endif // SHIP_H_
