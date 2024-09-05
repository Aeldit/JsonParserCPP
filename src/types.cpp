#include "types.hpp"

#include <iostream>

TypedValue::TypedValue(unsigned char type)
    : type(type)
{}

unsigned char TypedValue::getType()
{
    return type;
}

Item::Item(FastCompString key, unsigned char type)
    : TypedValue(type)
    , key(key)
{}

FastCompString Item::getKey()
{
    return key;
}

void Item::printKey()
{
    std::cout << "\"" << key.string() << "\""
              << ": ";
}
