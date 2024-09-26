#include "types.hpp"

#include <iostream>

String::String(const char *str, uint_strlen_t len)
    : string(str)
    , length(len)
{}

String::~String()
{
    delete[] string;
}

const char *String::str()
{
    return string;
}

uint_strlen_t String::len()
{
    return length;
}

bool String::operator==(String s)
{
    if (length != s.len())
    {
        return false;
    }

    const char *a = string;
    const char *b = s.str();
    for (uint_strlen_t i = 0; i < length; ++i)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

TypedValue::TypedValue(unsigned char type)
    : type(type)
{}

unsigned char TypedValue::getType()
{
    return type;
}

void TypedValue::print()
{
    printNoFlush();
    std::cout.flush();
}

Item::Item(String *key, unsigned char type)
    : TypedValue(type)
    , key(key)
{}

Item::~Item()
{
    delete key;
}

String *Item::getKey()
{
    return key;
}

void Item::printKey()
{
    std::cout << "\"" << (key == nullptr ? "" : key->str()) << "\""
              << ": ";
}
