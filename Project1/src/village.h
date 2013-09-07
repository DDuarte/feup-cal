#ifndef VILLAGE_H_
#define VILLAGE_H_

#include <string>

/**
* Represents a Village with name and position
*/
class Village
{
public:
    Village(const std::string& name) : _name(name) { }
    Village(const std::string& name, double x, double y) : _name(name), _x(x), _y(y) { }

    const std::string& GetName() const { return _name; } ///< Returns the name of the Village

    double GetX() const { return _x; } ///< Returns Village's x coordinate
    double GetY() const { return _y; } ///< Returns Village's y coordinate

private:
    std::string _name;
    double _x, _y;
};

#endif // VILLAGE_H_
