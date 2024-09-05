#include "json.hpp"

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <iomanip>
#include <iostream>
#include <string>

#include "custom_string.hpp"

using namespace std;

/*******************************************************************************
**                                   VALUES                                   **
*******************************************************************************/

/**************************************
**            STRING VALUE           **
**************************************/
StringTypedValue::StringTypedValue(std::string value)
    : TypedValue(T_STR)
    , value(value)
{}

std::string StringTypedValue::getValue()
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

/**************************************
**            STRING ITEM            **
**************************************/
StringItem::StringItem(FastCompString key, std::string value)
    : Item(key, T_STR)
    , value(value)
{}

std::string StringItem::getValue()
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
IntItem::IntItem(FastCompString key, int64_t value)
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
DoubleItem::DoubleItem(FastCompString key, double value)
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
BoolItem::BoolItem(FastCompString key, bool value)
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
NullItem::NullItem(FastCompString key)
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
ArrayItem::ArrayItem(FastCompString key, JSONArray *ja_arg)
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
DictItem::DictItem(FastCompString key, JSONDict *jd_arg)
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
    values = new LinkedList<TypedValue>();
}

JSONArray::~JSONArray()
{
    delete values;
}

uint64_t JSONArray::getSize()
{
    if (values == NULL)
    {
        return 0;
    }
    return values->getSize();
}

void JSONArray::addValue(TypedValue *value)
{
    if (value != NULL)
    {
        values->add(value);
    }
}

LinkedList<TypedValue> *JSONArray::getValues()
{
    return values;
}

TypedValue *JSONArray::getValueAt(uint64_t index)
{
    return values->get(index);
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
        TypedValue *value = values->get(i);
        if (value == NULL)
        {
            continue;
        }

        if (IS_ARR(value))
        {
            JSONArray *j = ((ArrayTypedValue *)value)->getValue();
            if (j != NULL)
            {
                j->printValuesIndent(indent + 1, false);
            }
        }
        else if (IS_DICT(value))
        {
            JSONDict *jd = ((DictTypedValue *)value)->getValue();
            if (jd != NULL)
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
    items = new LinkedList<Item>();
}

JSONDict::~JSONDict()
{
    delete items;
}

uint64_t JSONDict::getSize()
{
    if (items == NULL)
    {
        return 0;
    }
    return items->getSize();
}

void JSONDict::addItem(Item *item)
{
    if (item != NULL)
    {
        // If an item with the same key already exists we don't add the item
        if (!keyExists(item->getKey()))
        {
            items->add(item);
        }
    }
}

LinkedList<Item> *JSONDict::getItems()
{
    return items;
}

bool JSONDict::keyExists(FastCompString key)
{
    uint64_t size = getSize();
    for (size_t i = 0; i < size; ++i)
    {
        Item *it = items->get(i);
        if (it == NULL)
        {
            continue;
        }

        if (key == it->getKey())
        {
            return true;
        }
    }
    return false;
}

Item *JSONDict::getItem(FastCompString key)
{
    uint64_t size = getSize();
    for (size_t i = 0; i < size; ++i)
    {
        Item *it = items->get(i);
        if (it == NULL)
        {
            continue;
        }

        if (key == it->getKey())
        {
            return it;
        }
    }
    return NULL;
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
        Item *it = items->get(i);
        if (it == NULL)
        {
            continue;
        }

        if (IS_ARR(it))
        {
            JSONArray *value = ((ArrayItem *)it)->getValue();
            if (value != NULL)
            {
                cout << "\t" << tabs;
                it->printKey();
                value->printValuesIndent(indent + 1, true);
            }
        }
        else if (IS_DICT(it))
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
