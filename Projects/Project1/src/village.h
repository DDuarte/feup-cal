#ifndef VILLAGE_H_
#define VILLAGE_H_

#include <string>

class Village
{
public:
	Village(const std::string& name) : _name(name) { }

	const std::string& GetName() const { return _name; }
	void SetName(const std::string& val) { _name = val; }

private:
	std::string _name;
};

#endif // VILLAGE_H_
