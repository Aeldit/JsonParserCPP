#include "json.hpp"

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <iostream>
#include <string>

#include "types.h"

using namespace std;

/*******************************************************************************
**                                   ITEMS                                    **
*******************************************************************************/
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

/**************************************
**            ARRAY ITEM             **
**************************************/
ArrayItem::ArrayItem(string key, JSONArray *ja)
    : Item(key)
    , ja(*ja)
{
    free(ja);
}

unsigned char ArrayItem::getType()
{
    return TYPE_ARR;
}

JSONArray *ArrayItem::getValue()
{
    return &ja;
}

/**************************************
**             DICT ITEM             **
**************************************/
DictItem::DictItem(string key, JSONDict *jd)
    : Item(key)
    , jd(*jd)
{
    free(jd);
}

unsigned char DictItem::getType()
{
    return TYPE_DICT;
}

JSONDict *DictItem::getValue()
{
    return &jd;
}

/*******************************************************************************
**                                   VALUES                                   **
*******************************************************************************/
TypedValue::TypedValue(unsigned char type)
    : type(type)
{}

unsigned char TypedValue::getType()
{
    return type;
}

/**************************************
**            STRING VALUE           **
**************************************/
StringTypedValue::StringTypedValue(string value)
    : TypedValue(TYPE_STR)
    , value(value)
{}

string StringTypedValue::getValue()
{
    return value;
}

void StringTypedValue::print()
{
    cout << "\"" << value << "\"";
}

/**************************************
**             INT VALUE             **
**************************************/
IntTypedValue::IntTypedValue(int64_t value)
    : TypedValue(TYPE_NUM)
    , value(value)
{}

int64_t IntTypedValue::getValue()
{
    return value;
}

void IntTypedValue::print()
{
    cout << value;
}

/**************************************
**             BOOL VALUE            **
**************************************/
BoolTypedValue::BoolTypedValue(bool value)
    : TypedValue(TYPE_BOOL)
    , value(value)
{}

bool BoolTypedValue::getValue()
{
    return value;
}

void BoolTypedValue::print()
{
    cout << (value ? "true" : "false");
}

/**************************************
**             NULL VALUE            **
**************************************/
NullTypedValue::NullTypedValue()
    : TypedValue(TYPE_NULL)
{}

void NullTypedValue::print()
{
    cout << "null";
}

/**************************************
**            ARRAY VALUE            **
**************************************/
ArrayTypedValue::ArrayTypedValue(JSONArray *ja)
    : TypedValue(TYPE_ARR)
    , ja(*ja)
{
    free(ja);
}

JSONArray *ArrayTypedValue::getValue()
{
    return &ja;
}

/**************************************
**            DICT VALUE             **
**************************************/
DictTypedValue::DictTypedValue(JSONDict *jd)
    : TypedValue(TYPE_DICT)
    , jd(*jd)
{
    free(jd);
}

JSONDict *DictTypedValue::getValue()
{
    return &jd;
}

/*******************************************************************************
**                                    JSON                                    **
*******************************************************************************/
/**************************************
**              ARRAY                **
**************************************/
JSONArray::JSONArray(size_t size)
    : size(size)
{
#ifdef DEBUG
    cout << "Initializing " << size << " values" << endl;
#endif
    values = (TypedValue **)calloc(size, sizeof(TypedValue *));
}

JSONArray::~JSONArray()
{
    if (values == NULL || size == 0)
    {
        return;
    }

    for (size_t i = 0; i < size; ++i)
    {
        if (values[i] == NULL)
        {
            continue;
        }
        free(values[i]);
    }
    free(values);
}

size_t JSONArray::getSize()
{
    return size;
}

TypedValue **JSONArray::getValues()
{
    return values;
}

TypedValue *JSONArray::getValueAt(size_t index)
{
    if (values == NULL || index >= size)
    {
        return NULL;
    }
    return values[index];
}

void JSONArray::add(TypedValue *value)
{
    if (values == NULL || value == NULL || insert_idx >= size)
    {
        return;
    }

    values[insert_idx++] = value;
}

void JSONArray::printValues()
{
    if (values == NULL || size == 0)
    {
        return;
    }

    cout << "\t[" << endl;

    for (size_t i = 0; i < size; ++i)
    {
        cout << "\t\t";

        if (values[i] == NULL)
        {
            continue;
        }

        if (values[i]->getType() == TYPE_ARR)
        {
            JSONArray *value = ((ArrayTypedValue *)values[i])->getValue();
            if (value != NULL)
            {
                value->printValues();
            }
        }
        else if (values[i]->getType() == TYPE_DICT)
        {
            JSONDict *value = ((DictTypedValue *)values[i])->getValue();
            if (value != NULL)
            {
                value->printItems();
            }
        }
        else
        {
            values[i]->print();
        }

        if (i < size - 1)
        {
            cout << "," << endl;
        }
    }

    cout << "\n]" << endl;
}

/**************************************
**               DICT                **
**************************************/
JSONDict::JSONDict(size_t size)
    : size(size)
{
#ifdef DEBUG
    cout << "Initializing " << size << " items" << endl;
#endif
    items = (Item **)calloc(size, sizeof(Item *));
}

JSONDict::~JSONDict()
{
    if (items == NULL || size == 0)
    {
        return;
    }

    for (size_t i = 0; i < size; ++i)
    {
        if (items[i] == NULL)
        {
            continue;
        }
        free(items[i]);
    }
    free(items);
}

size_t JSONDict::getSize()
{
    return size;
}

Item **JSONDict::getItems()
{
    return items;
}

Item *JSONDict::getItem(string key)
{
    if (items == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < size; ++i)
    {
        if (items[i] == NULL)
        {
            continue;
        }

        if (key.compare(items[i]->getKey()) == 0)
        {
            return items[i];
        }
    }
    return NULL;
}

void JSONDict::addItem(Item *item)
{
    if (items == NULL || item == NULL || insert_idx >= size)
    {
        return;
    }

    items[insert_idx++] = item;
}

void JSONDict::printItems()
{
    if (items == NULL || size == 0)
    {
        return;
    }

    cout << "{" << endl;

    for (size_t i = 0; i < size; ++i)
    {
        cout << "\t";

        if (items[i] == NULL)
        {
            continue;
        }

        if (items[i]->getType() == TYPE_ARR)
        {
            JSONArray *value = ((ArrayItem *)items[i])->getValue();
            if (value != NULL)
            {
                value->printValues();
            }
        }
        else if (items[i]->getType() == TYPE_DICT)
        {
            JSONDict *value = ((DictItem *)items[i])->getValue();
            if (value != NULL)
            {
                value->printItems();
            }
        }
        else
        {
            items[i]->print();
        }

        if (i < size - 1)
        {
            cout << "," << endl;
        }
    }

    cout << "\n}" << endl;
}
