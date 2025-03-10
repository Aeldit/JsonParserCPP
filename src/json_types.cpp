#include "json_types.hpp"

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
    if (a == nullptr || b == nullptr)
    {
        return false;
    }

    for (uint_strlen_t i = 0; i < length; ++i)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

Value::Value(unsigned char type)
    : type(type)
{}

unsigned char Value::getType()
{
    return type;
}

void Value::print()
{
    printNoFlush();
    std::cout.flush();
}

Item::Item(String *key, unsigned char type)
    : Value(type)
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
void print_err_bits(uint_fast16_t err)
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
              << (ERR_NULL_VALUE & err ? 1 : 0) << " : ERR_NULL_VALUE\n"
              << (ERR_NULL_ITEM & err ? 1 : 0) << " : ERR_NULL_ITEM\n"
              << std::endl;
}
