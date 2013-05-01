#ifndef ORDER_H_
#define ORDER_H_

/**
* Represents an order with weight and volume
*/
class Order
{
public:
    Order(uint weight, uint volume) : _weight(weight), _volume(volume) { } ///< Constructor

    uint GetWeight() const { return _weight; } ///< Returns the order weight
    uint GetVolume() const { return _volume; } ///< Returns the order volume

private:
    uint _weight;
    uint _volume;
};

#endif // ORDER_H_
