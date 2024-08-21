#include "items.hpp"

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <iostream>
#include <string>

#include "types.h"

using namespace std;

/**************************************
**               ITEM                **
**************************************/
Item::Item(string key)
    : key(key)
{}

string Item::getKey()
{
    return key;
}

void Item::printKey()
{
    cout << "\"" << key << "\"" << ": ";
}

/**************************************
**            STRING ITEM            **
**************************************/
StringItem::StringItem(string key, string value)
    : Item(key)
    , value(value)
{}

string StringItem::getValue()
{
    return value;
}

unsigned char StringItem::getType()
{
    return TYPE_STR;
}

void StringItem::print()
{
    printKey();
    cout << "\"" << value << "\"";
}

/**************************************
**             INT ITEM              **
**************************************/
IntItem::IntItem(string key, int64_t value)
    : Item(key)
    , value(value)
{}

int64_t IntItem::getValue()
{
    return value;
}

unsigned char IntItem::getType()
{
    return TYPE_NUM;
}

void IntItem::print()
{
    printKey();
    cout << value;
}

/**************************************
**             BOOL ITEM             **
**************************************/
BoolItem::BoolItem(string key, bool value)
    : Item(key)
    , value(value)
{}

bool BoolItem::getValue()
{
    return value;
}

unsigned char BoolItem::getType()
{
    return TYPE_BOOL;
}

void BoolItem::print()
{
    printKey();
    cout << (value ? "true" : "false");
}

/**************************************
**             NULL ITEM             **
**************************************/
NullItem::NullItem(string key)
    : Item(key)
{}

unsigned char NullItem::getType()
{
    return TYPE_NULL;
}

void NullItem::print()
{
    printKey();
    cout << "null";
}