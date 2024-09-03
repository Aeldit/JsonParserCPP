#include "types.hpp"

#include <iostream>

using namespace std;

TypedValue::TypedValue(unsigned char type)
    : type(type)
{}

unsigned char TypedValue::getType()
{
    return type;
}

Item::Item(string key, unsigned char type)
    : TypedValue(type)
    , key(key)
{}

string Item::getKey()
{
    return key;
}

void Item::printKey()
{
    cout << "\"" << key << "\""
         << ": ";
}
