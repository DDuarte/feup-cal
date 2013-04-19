#ifndef ORDER_H_
#define ORDER_H_

class Order
{
public:
	Order(double weight, int volume) : _weight(weight), _volume(volume) { }

	double	GetWeight() const { return _weight; }
	int		GetVolume() const { return _volume; }

	void SetWeight(double newWeight)  { _weight = newWeight; }
	void SetVolume(int newVolume) { _volume = newVolume; }

private:
	double	_weight;
	int		_volume;
};

#endif // ORDER_H_
