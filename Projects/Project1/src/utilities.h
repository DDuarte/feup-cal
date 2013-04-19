#ifndef UTILITIES_H_
#define UTILITIES_H_

template<class Fwd>
struct Reverser {
	const Fwd &fwd;
	Reverser<Fwd>(const Fwd &fwd_): fwd(fwd_) {}
	auto begin() -> decltype(fwd.rbegin()) const { return fwd.rbegin(); } 
	auto end() -> decltype(fwd.rend()) const  { return fwd.rend(); } 
};

template<class Fwd>
Reverser<Fwd> reverse(const Fwd &fwd) { return Reverser<Fwd>(fwd); }

#endif // UTILITIES_H_
