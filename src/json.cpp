#include "json.hpp"

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <iomanip>
#include <iostream>
#include <string>

#include "types.h"

using namespace std;

/*******************************************************************************
**                                   ITEMS                                    **
*******************************************************************************/
Item::Item(string key, unsigned char type)
    : key(key)
    , type(type)
{}

string Item::getKey()
{
    return key;
}

unsigned char Item::getType()
{
    return type;
}

void Item::printKey()
{
    cout << "\"" << key << "\""
         << ": ";
}

/**************************************
**            STRING ITEM            **
**************************************/
StringItem::StringItem(string key, string value)
    : Item(key, TYPE_STR)
    , value(value)
{}

string StringItem::getValue()
{
    return value;
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
    : Item(key, TYPE_NUM)
    , value(value)
{}

int64_t IntItem::getValue()
{
    return value;
}

void IntItem::print()
{
    printKey();
    cout << value;
}

/**************************************
**           DOUBLE ITEM             **
**************************************/
DoubleItem::DoubleItem(string key, double value)
    : Item(key, TYPE_DOUBLE)
    , value(value)
{}

double DoubleItem::getValue()
{
    return value;
}

void DoubleItem::print()
{
    printKey();
    cout << setprecision(16) << value;
}

/**************************************
**             BOOL ITEM             **
**************************************/
BoolItem::BoolItem(string key, bool value)
    : Item(key, TYPE_BOOL)
    , value(value)
{}

bool BoolItem::getValue()
{
    return value;
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
    : Item(key, TYPE_NULL)
{}

void NullItem::print()
{
    printKey();
    cout << "null";
}

/**************************************
**            ARRAY ITEM             **
**************************************/
ArrayItem::ArrayItem(string key, JSONArray *ja_arg)
    : Item(key, TYPE_ARR)
    , ja(ja_arg)
{}

ArrayItem::~ArrayItem()
{
    delete ja;
}

JSONArray *ArrayItem::getValue()
{
    return ja;
}

/**************************************
**             DICT ITEM             **
**************************************/
DictItem::DictItem(string key, JSONDict *jd_arg)
    : Item(key, TYPE_DICT)
    , jd(jd_arg)
{}

DictItem::~DictItem()
{
    delete jd;
}

JSONDict *DictItem::getValue()
{
    return jd;
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
**           DOUBLE VALUE            **
**************************************/
DoubleTypedValue::DoubleTypedValue(double value)
    : TypedValue(TYPE_DOUBLE)
    , value(value)
{}

double DoubleTypedValue::getValue()
{
    return value;
}

void DoubleTypedValue::print()
{
    cout << setprecision(16) << value;
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
ArrayTypedValue::ArrayTypedValue(JSONArray *ja_arg)
    : TypedValue(TYPE_ARR)
    , ja(ja_arg)
{}

ArrayTypedValue::~ArrayTypedValue()
{
    delete ja;
}

JSONArray *ArrayTypedValue::getValue()
{
    return ja;
}

/**************************************
**            DICT VALUE             **
**************************************/
DictTypedValue::DictTypedValue(JSONDict *jd_arg)
    : TypedValue(TYPE_DICT)
    , jd(jd_arg)
{}

DictTypedValue::~DictTypedValue()
{
    delete jd;
}

JSONDict *DictTypedValue::getValue()
{
    return jd;
}

/*******************************************************************************
**                                    JSON                                    **
*******************************************************************************/
JSON::JSON(bool is_array)
    : is_array(is_array)
{}

bool JSON::isArray()
{
    return is_array;
}

/**************************************
**              ARRAY                **
**************************************/
JSONArray::JSONArray(size_t size)
    : JSON(true)
    , size(size)
{
#ifdef DEBUG
    cout << "Initializing array of " << size << " values" << endl;
#endif
    values = new TypedValue *[size]();
}

JSONArray::~JSONArray()
{
    if (values == NULL)
    {
        return;
    }

    for (size_t i = 0; i < size; ++i)
    {
        if (values[i] == NULL)
        {
            continue;
        }
        delete values[i];
    }
    delete[] values;
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
    printValuesIndent(1, false);
}

void JSONArray::printValuesIndent(int indent, bool fromDict)
{
    // Obtains the number of tab characters that will be printed
    char *tabs = new char[indent]();
    if (tabs == NULL)
    {
        return;
    }
    for (int i = 0; i < indent - 1; ++i)
    {
        tabs[i] = '\t';
    }
    tabs[indent - 1] = '\0';

    // Empty array
    if (values == NULL || size == 0)
    {
        cout << (fromDict ? "" : tabs) << "[]";
        if (indent == 1)
        {
            cout << endl;
        }
        delete[] tabs;
        return;
    }

    cout << (fromDict ? "" : tabs) << "[\n";

    for (size_t i = 0; i < size; ++i)
    {
        if (values[i] == NULL)
        {
            continue;
        }

        if (values[i]->getType() == TYPE_ARR)
        {
            JSONArray *value = ((ArrayTypedValue *)values[i])->getValue();
            if (value != NULL)
            {
                value->printValuesIndent(indent + 1, false);
            }
        }
        else if (values[i]->getType() == TYPE_DICT)
        {
            JSONDict *value = ((DictTypedValue *)values[i])->getValue();
            if (value != NULL)
            {
                value->printItemsIndent(indent + 1, false);
            }
        }
        else
        {
            cout << "\t" << tabs;
            values[i]->print();
        }

        if (i < size - 1)
        {
            cout << ",\n";
        }
    }

    cout << "\n" << tabs << "]";

    if (indent == 1)
    {
        cout << endl;
    }
    delete[] tabs;
}

/**************************************
**               DICT                **
**************************************/
JSONDict::JSONDict(size_t size)
    : JSON(false)
    , size(size)
{
#ifdef DEBUG
    cout << "Initializing dict of " << size << " items" << endl;
#endif
    items = new Item *[size]();
}

JSONDict::~JSONDict()
{
    if (items == NULL)
    {
        return;
    }

    for (size_t i = 0; i < size; ++i)
    {
        if (items[i] == NULL)
        {
            continue;
        }
        delete items[i];
    }
    delete[] items;
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
    printItemsIndent(1, false);
}

void JSONDict::printItemsIndent(int indent, bool fromDict)
{
    // Obtains the number of tab characters that will be printed
    char *tabs = new char[indent]();
    if (tabs == NULL)
    {
        return;
    }
    for (int i = 0; i < indent - 1; ++i)
    {
        tabs[i] = '\t';
    }
    tabs[indent - 1] = '\0';

    if (items == NULL || size == 0)
    {
        cout << (fromDict ? "" : tabs) << "{}";
        if (indent == 1)
        {
            cout << endl;
        }
        delete[] tabs;
        return;
    }

    cout << (fromDict ? "" : tabs) << "{\n";

    for (size_t i = 0; i < size; ++i)
    {
        if (items[i] == NULL)
        {
            continue;
        }

        if (items[i]->getType() == TYPE_ARR)
        {
            JSONArray *value = ((ArrayItem *)items[i])->getValue();
            if (value != NULL)
            {
                cout << "\t" << tabs;
                items[i]->printKey();
                value->printValuesIndent(indent + 1, true);
            }
        }
        else if (items[i]->getType() == TYPE_DICT)
        {
            JSONDict *value = ((DictItem *)items[i])->getValue();
            if (value != NULL)
            {
                cout << "\t" << tabs;
                items[i]->printKey();
                value->printItemsIndent(indent + 1, true);
            }
        }
        else
        {
            cout << "\t" << tabs;
            items[i]->print();
        }

        if (i < size - 1)
        {
            cout << ",\n";
        }
    }

    cout << "\n" << tabs << "}";

    if (indent == 1)
    {
        cout << endl;
    }
    delete[] tabs;
}
