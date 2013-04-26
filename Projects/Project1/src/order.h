#ifndef ORDER_H_
#define ORDER_H_

/**
* Represents an order with weight and volume
*/
class Order
{
public:
    Order(uint weight, uint volume) : _weight(weight), _volume(volume) { } ///< Constructor
    Order(const Order& other) : _weight(other._weight), _volume(other._volume) { } ///< Copy constructor
    Order operator=(const Order& other) const { return Order(other); } ///< Assignment operator

    uint GetWeight() const { return _weight; } ///< Returns the order weight
    uint GetVolume() const { return _volume; } ///< Returns the order volume

private:
    const uint _weight;
    const uint _volume;
};

#endif // ORDER_H_
