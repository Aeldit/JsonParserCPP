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
        // TODO: Test while (length--)
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

/*******************************************************************************
**                                 FUNCTIONS                                  **
*******************************************************************************/
void print_err_bits(uint_fast8_t err)
{
    std::cout << (ERR_FSEEK & err ? 1 : 0) << " : ERR_FSEEK\n"
              << (ERR_NULL_KEY & err ? 1 : 0) << " : ERR_NULL_KEY\n"
              << (ERR_NULL_STR & err ? 1 : 0) << " : ERR_NULL_STR\n"
              << (ERR_NULL_ARR & err ? 1 : 0) << " : ERR_NULL_ARR\n"
              << (ERR_NULL_DICT & err ? 1 : 0) << " : ERR_NULL_DICT\n"
              << (ERR_ITEM_EXISTS & err ? 1 : 0) << " : ERR_ITEM_EXISTS\n"
              << (ERR_MAX_NESTED_ARRAYS_REACHED & err ? 1 : 0)
              << " : ERR_MAX_NESTED_ARRAYS_REACHED\n"
              << (ERR_MAX_NESTED_DICTS_REACHED & err ? 1 : 0)
              << " : ERR_MAX_NESTED_DICTS_REACHED\n"
              << std::endl;
}
