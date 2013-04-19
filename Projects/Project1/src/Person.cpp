/*
 * Person.cpp
 */

#include "Person.h"

#include <string>
#include <ostream>

Person::Person(std::string n, int a): _name(n), _age(a) {}
Person::Person(){}

std::string Person::getName() const { return _name; }

bool Person::operator == (const Person &p2) const { return (_name==p2._name && _age==p2._age); }

std::ostream & operator << (std::ostream &os, Person &p) 
{
   os << p.getName();
   return os;
}
