#include "Ponto.h"

ostream& operator<<(ostream& os, const Ponto &p)
{
    os << "(" << p.x << "," << p.y << ")";
    return os;
}
