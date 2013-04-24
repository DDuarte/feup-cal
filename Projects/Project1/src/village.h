#ifndef VILLAGE_H_
#define VILLAGE_H_

#include <string>

class Village
{
public:
    Village(const std::string& name) : _name(name) { }
    Village(const std::string& name, double x, double y) : _name(name), _x(x), _y(y) { }

    const std::string& GetName() const { return _name; }
    void SetName(const std::string& val) { _name = val; }

    double GetX() const { return _x; }
    double GetY() const { return _y; }

private:
    std::string _name;
    double _x, _y;
};

#endif // VILLAGE_H_
