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
    : TypedValue(T_STR)
    , value(value)
{}

string StringTypedValue::getValue()
{
    return value;
}

void StringTypedValue::printNoFlush()
{
    cout << "\"" << value << "\"";
}

void StringTypedValue::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**             INT VALUE             **
**************************************/
IntTypedValue::IntTypedValue(int64_t value)
    : TypedValue(T_INT)
    , value(value)
{}

int64_t IntTypedValue::getValue()
{
    return value;
}

void IntTypedValue::printNoFlush()
{
    cout << value;
}

void IntTypedValue::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**           DOUBLE VALUE            **
**************************************/
DoubleTypedValue::DoubleTypedValue(double value)
    : TypedValue(T_DOUBLE)
    , value(value)
{}

double DoubleTypedValue::getValue()
{
    return value;
}

void DoubleTypedValue::printNoFlush()
{
    cout << setprecision(16) << value;
}

void DoubleTypedValue::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**             BOOL VALUE            **
**************************************/
BoolTypedValue::BoolTypedValue(bool value)
    : TypedValue(T_BOOL)
    , value(value)
{}

bool BoolTypedValue::getValue()
{
    return value;
}

void BoolTypedValue::printNoFlush()
{
    cout << (value ? "true" : "false");
}

void BoolTypedValue::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**             NULL VALUE            **
**************************************/
NullTypedValue::NullTypedValue()
    : TypedValue(T_NULL)
{}

void NullTypedValue::printNoFlush()
{
    cout << "null";
}

void NullTypedValue::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**            ARRAY VALUE            **
**************************************/
ArrayTypedValue::ArrayTypedValue(JSONArray *ja_arg)
    : TypedValue(T_ARR)
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

void ArrayTypedValue::printNoFlush()
{
    print();
}

void ArrayTypedValue::print()
{
    if (ja != NULL)
    {
        ja->printValues();
    }
}

/**************************************
**            DICT VALUE             **
**************************************/
DictTypedValue::DictTypedValue(JSONDict *jd_arg)
    : TypedValue(T_DICT)
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

void DictTypedValue::printNoFlush()
{
    print();
}

void DictTypedValue::print()
{
    if (jd != NULL)
    {
        jd->printItems();
    }
}

/*******************************************************************************
**                                   ITEMS                                    **
*******************************************************************************/
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

/**************************************
**            STRING ITEM            **
**************************************/
StringItem::StringItem(string key, string value)
    : Item(key, T_STR)
    , value(value)
{}

string StringItem::getValue()
{
    return value;
}

void StringItem::printNoFlush()
{
    printKey();
    cout << "\"" << value << "\"";
}

void StringItem::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**             INT ITEM              **
**************************************/
IntItem::IntItem(string key, int64_t value)
    : Item(key, T_INT)
    , value(value)
{}

int64_t IntItem::getValue()
{
    return value;
}

void IntItem::printNoFlush()
{
    printKey();
    cout << value;
}

void IntItem::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**           DOUBLE ITEM             **
**************************************/
DoubleItem::DoubleItem(string key, double value)
    : Item(key, T_DOUBLE)
    , value(value)
{}

double DoubleItem::getValue()
{
    return value;
}

void DoubleItem::printNoFlush()
{
    printKey();
    cout << setprecision(16) << value;
}

void DoubleItem::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**             BOOL ITEM             **
**************************************/
BoolItem::BoolItem(string key, bool value)
    : Item(key, T_BOOL)
    , value(value)
{}

bool BoolItem::getValue()
{
    return value;
}

void BoolItem::printNoFlush()
{
    printKey();
    cout << (value ? "true" : "false");
}

void BoolItem::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**             NULL ITEM             **
**************************************/
NullItem::NullItem(string key)
    : Item(key, T_NULL)
{}

void NullItem::printNoFlush()
{
    printKey();
    cout << "null";
}

void NullItem::print()
{
    printNoFlush();
    cout.flush();
}

/**************************************
**            ARRAY ITEM             **
**************************************/
ArrayItem::ArrayItem(string key, JSONArray *ja_arg)
    : Item(key, T_ARR)
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

void ArrayItem::printNoFlush()
{
    print();
}

void ArrayItem::print()
{
    if (ja != NULL)
    {
        ja->printValues();
    }
}

/**************************************
**             DICT ITEM             **
**************************************/
DictItem::DictItem(string key, JSONDict *jd_arg)
    : Item(key, T_DICT)
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

void DictItem::printNoFlush()
{
    print();
}

void DictItem::print()
{
    if (jd != NULL)
    {
        jd->printItems();
    }
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
JSONArray::JSONArray()
    : JSON(true)
{
#ifdef DEBUG
    cout << "Initializing array of " << getSize() << " values" << endl;
#endif
    values = LinkedList<TypedValue *>();
}

JSONArray::~JSONArray()
{
    uint64_t size = getSize();
    for (size_t i = 0; i < size; ++i)
    {
        TypedValue *value = values.get(i);
        if (value == NULL)
        {
            continue;
        }
        delete value;
    }
}

uint64_t JSONArray::getSize()
{
    return values.getSize();
}

LinkedList<TypedValue *> JSONArray::getValues()
{
    return values;
}

TypedValue *JSONArray::getValueAt(uint64_t index)
{
    return values.get(index);
}

void JSONArray::addValue(TypedValue *value)
{
    values.add(value);
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

    uint64_t size = getSize();
    // Empty array
    if (size == 0)
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

    for (uint64_t i = 0; i < size; ++i)
    {
        TypedValue *value = values.get(i);
        if (value == NULL)
        {
            continue;
        }

        if (IS_ARR(value))
        {
            JSONArray *value = ((ArrayTypedValue *)value)->getValue();
            if (value != NULL)
            {
                value->printValuesIndent(indent + 1, false);
            }
        }
        else if (IS_DICT(value))
        {
            JSONDict *value = ((DictTypedValue *)value)->getValue();
            if (value != NULL)
            {
                value->printItemsIndent(indent + 1, false);
            }
        }
        else
        {
            cout << "\t" << tabs;
            value->printNoFlush();
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
JSONDict::JSONDict()
    : JSON(false)
{
#ifdef DEBUG
    cout << "Initializing dict of " << getSize() << " items" << endl;
#endif
    items = LinkedList<Item *>();
}

JSONDict::~JSONDict()
{
    uint64_t size = getSize();
    for (size_t i = 0; i < size; ++i)
    {
        Item *it = items.get(i);
        if (it == NULL)
        {
            continue;
        }
        delete it;
    }
}

uint64_t JSONDict::getSize()
{
    return items.getSize();
}

LinkedList<Item *> JSONDict::getItems()
{
    return items;
}

Item *JSONDict::getItem(string key)
{
    uint64_t size = getSize();
    for (size_t i = 0; i < size; ++i)
    {
        Item *it = items.get(i);
        if (it == NULL)
        {
            continue;
        }

        if (key.compare(it->getKey()) == 0)
        {
            return it;
        }
    }
    return NULL;
}

void JSONDict::addItem(Item *item)
{
    items.add(item);
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

    uint64_t size = getSize();
    if (size == 0)
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

    for (uint64_t i = 0; i < size; ++i)
    {
        Item *it = items.get(i);
        if (it == NULL)
        {
            continue;
        }

        if (it->getType() == T_ARR)
        {
            JSONArray *value = ((ArrayItem *)it)->getValue();
            if (value != NULL)
            {
                cout << "\t" << tabs;
                it->printKey();
                value->printValuesIndent(indent + 1, true);
            }
        }
        else if (it->getType() == T_DICT)
        {
            JSONDict *value = ((DictItem *)it)->getValue();
            if (value != NULL)
            {
                cout << "\t" << tabs;
                it->printKey();
                value->printItemsIndent(indent + 1, true);
            }
        }
        else
        {
            cout << "\t" << tabs;
            it->printNoFlush();
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
