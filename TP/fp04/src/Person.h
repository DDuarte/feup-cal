/*
 * Person.h
 */

#ifndef PERSON_H_
#define PERSON_H_

#include <string>
#include <ostream>

class Person
{
public:
    Person();
    Person (std::string n, int a);
    std::string getName() const;
    bool operator == (const Person &p2) const;
    friend std::ostream & operator << (std::ostream &os, Person &p);
private:
    std::string _name;
    int _age;
};

#endif /* NETWORK_H_ */
