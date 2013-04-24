#ifndef ORDER_H_
#define ORDER_H_

class Order
{
public:
    Order(double weight, uint volume) : _weight(weight), _volume(volume) { }
    Order(const Order& other) : _weight(other._weight), _volume(other._volume) { }
    Order operator=(const Order& other) const { return Order(other); }

    double    GetWeight() const { return _weight; }
    uint      GetVolume() const { return _volume; }

private:
    const double    _weight;
    const uint      _volume;
};

#endif // ORDER_H_
