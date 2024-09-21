#include "json.hpp"

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <iomanip>
#include <iostream>

using namespace std;

/*******************************************************************************
**                                   VALUES                                   **
*******************************************************************************/
/**************************************
**            STRING VALUE           **
**************************************/
StringTypedValue::StringTypedValue(String *value)
    : TypedValue(T_STR)
    , value(value)
{}

StringTypedValue::~StringTypedValue()
{
    delete value;
}

String *StringTypedValue::getValue()
{
    return value;
}

void StringTypedValue::printNoFlush()
{
    cout << "\"" << value->str() << "\"";
}

/**************************************
**             INT VALUE             **
**************************************/
IntTypedValue::IntTypedValue(int_fast64_t value)
    : TypedValue(T_INT)
    , value(value)
{}

int_fast64_t IntTypedValue::getValue()
{
    return value;
}

void IntTypedValue::printNoFlush()
{
    cout << value;
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
    if (ja != nullptr)
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
    if (jd != nullptr)
    {
        jd->printItems();
    }
}

/*******************************************************************************
**                                   ITEMS                                    **
*******************************************************************************/

/**************************************
**            STRING ITEM            **
**************************************/
StringItem::StringItem(String *key, String *value)
    : Item(key, T_STR)
    , value(value)
{}

StringItem::~StringItem()
{
    delete value;
}

String *StringItem::getValue()
{
    return value;
}

void StringItem::printNoFlush()
{
    printKey();
    cout << "\"" << value->str() << "\"";
}

/**************************************
**             INT ITEM              **
**************************************/
IntItem::IntItem(String *key, int_fast64_t value)
    : Item(key, T_INT)
    , value(value)
{}

int_fast64_t IntItem::getValue()
{
    return value;
}

void IntItem::printNoFlush()
{
    printKey();
    cout << value;
}

/**************************************
**           DOUBLE ITEM             **
**************************************/
DoubleItem::DoubleItem(String *key, double value)
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

/**************************************
**             BOOL ITEM             **
**************************************/
BoolItem::BoolItem(String *key, bool value)
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

/**************************************
**             NULL ITEM             **
**************************************/
NullItem::NullItem(String *key)
    : Item(key, T_NULL)
{}

void NullItem::printNoFlush()
{
    printKey();
    cout << "null";
}

/**************************************
**            ARRAY ITEM             **
**************************************/
ArrayItem::ArrayItem(String *key, JSONArray *ja_arg)
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
    if (ja != nullptr)
    {
        ja->printValues();
    }
}

/**************************************
**             DICT ITEM             **
**************************************/
DictItem::DictItem(String *key, JSONDict *jd_arg)
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
    if (jd != nullptr)
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
    values = LinkedList<TypedValue>();
}

JSONArray::~JSONArray()
{}

uint64_t JSONArray::getSize()
{
    return values.getSize();
}

void JSONArray::addValue(TypedValue *value)
{
    if (value != nullptr)
    {
        values.add(value);
    }
}

TypedValue **JSONArray::getValues()
{
    return values.getAsArray();
}

TypedValue *JSONArray::getValueAt(uint_fast64_t index)
{
    return values.get(index);
}

void JSONArray::printValues()
{
#ifndef VALGRING_DISABLE_PRINT
    printValuesIndent(1, false);
#endif
}

void JSONArray::printValuesIndent(int indent, bool fromDict)
{
    // Obtains the number of tab characters that will be printed
    char *tabs = new char[indent]();
    if (tabs == nullptr)
    {
        return;
    }
    for (int i = 0; i < indent - 1; ++i)
    {
        tabs[i] = '\t';
    }
    tabs[indent - 1] = '\0';

    uint_fast64_t size = getSize();
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

    for (uint_fast64_t i = 0; i < size; ++i)
    {
        TypedValue *value = values.get(i);
        if (value == nullptr)
        {
            continue;
        }

        if (IS_ARR(value))
        {
            JSONArray *j = ((ArrayTypedValue *)value)->getValue();
            if (j != nullptr)
            {
                j->printValuesIndent(indent + 1, false);
            }
        }
        else if (IS_DICT(value))
        {
            JSONDict *jd = ((DictTypedValue *)value)->getValue();
            if (jd != nullptr)
            {
                jd->printItemsIndent(indent + 1, false);
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
    items = LinkedList<Item>();
}

JSONDict::~JSONDict()
{}

uint64_t JSONDict::getSize()
{
    return items.getSize();
}

void JSONDict::addItem(Item *item)
{
    if (item != nullptr)
    {
        // If an item with the same key already exists we don't add the item
        if (getItem(item->getKey()) == nullptr)
        {
            items.add(item);
        }
        else
        {
#ifdef DEBUG
            cout << "The item with key '" << item->getKey()
                 << "' already exists, not adding it and freeing allocated "
                    "memory"
                 << endl;
#endif
            delete item;
        }
    }
}

Item **JSONDict::getItems()
{
    return items.getAsArray();
}

Item *JSONDict::getItem(String *key)
{
    uint_fast64_t size = getSize();
    for (uint_fast64_t i = 0; i < size; ++i)
    {
        Item *it = items.get(i);
        if (it != nullptr)
        {
            String *k = it->getKey();
            if (key == k)
            {
                return it;
            }
        }
    }
    return nullptr;
}

void JSONDict::printItems()
{
#ifndef VALGRING_DISABLE_PRINT
    printItemsIndent(1, false);
#endif
}

void JSONDict::printItemsIndent(int indent, bool fromDict)
{
    // Obtains the number of tab characters that will be printed
    char *tabs = new char[indent]();
    if (tabs == nullptr)
    {
        return;
    }
    for (int i = 0; i < indent - 1; ++i)
    {
        tabs[i] = '\t';
    }
    tabs[indent - 1] = '\0';

    uint_fast64_t size = getSize();
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

    for (uint_fast64_t i = 0; i < size; ++i)
    {
        Item *it = items.get(i);
        if (it == nullptr)
        {
            continue;
        }

        if (IS_ARR(it))
        {
            JSONArray *value = ((ArrayItem *)it)->getValue();
            if (value != nullptr)
            {
                cout << "\t" << tabs;
                it->printKey();
                value->printValuesIndent(indent + 1, true);
            }
        }
        else if (IS_DICT(it))
        {
            JSONDict *value = ((DictItem *)it)->getValue();
            if (value != nullptr)
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
