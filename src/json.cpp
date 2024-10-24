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
StringValue::StringValue(String *value)
    : Value(T_STR)
    , value(value)
{}

StringValue::~StringValue()
{
    delete value;
}

String *StringValue::getValue()
{
    return value;
}

void StringValue::printNoFlush()
{
    cout << "\"" << (value == nullptr ? "" : value->str()) << "\"";
}

/**************************************
**             INT VALUE             **
**************************************/
IntValue::IntValue(int_fast64_t value)
    : Value(T_INT)
    , value(value)
{}

int_fast64_t IntValue::getValue()
{
    return value;
}

void IntValue::printNoFlush()
{
    cout << value;
}

/**************************************
**           DOUBLE VALUE            **
**************************************/
DoubleValue::DoubleValue(double value)
    : Value(T_DOUBLE)
    , value(value)
{}

double DoubleValue::getValue()
{
    return value;
}

void DoubleValue::printNoFlush()
{
    cout << setprecision(16) << value;
}

/**************************************
**             BOOL VALUE            **
**************************************/
BoolValue::BoolValue(bool value)
    : Value(T_BOOL)
    , value(value)
{}

bool BoolValue::getValue()
{
    return value;
}

void BoolValue::printNoFlush()
{
    cout << (value ? "true" : "false");
}

/**************************************
**             NULL VALUE            **
**************************************/
NullValue::NullValue()
    : Value(T_NULL)
{}

void NullValue::printNoFlush()
{
    cout << "null";
}

/**************************************
**            ARRAY VALUE            **
**************************************/
ArrayValue::ArrayValue(JSONArray *ja_arg)
    : Value(T_ARR)
    , ja(ja_arg)
{}

ArrayValue::~ArrayValue()
{
    delete ja;
}

JSONArray *ArrayValue::getValue()
{
    return ja;
}

void ArrayValue::printNoFlush()
{
    print();
}

void ArrayValue::print()
{
    if (ja != nullptr)
    {
        ja->printValues();
    }
}

/**************************************
**            DICT VALUE             **
**************************************/
DictValue::DictValue(JSONDict *jd_arg)
    : Value(T_DICT)
    , jd(jd_arg)
{}

DictValue::~DictValue()
{
    delete jd;
}

JSONDict *DictValue::getValue()
{
    return jd;
}

void DictValue::printNoFlush()
{
    print();
}

void DictValue::print()
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
    cout << "\"" << (value == nullptr ? "" : value->str()) << "\"";
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
    values = LinkedList<Value>();
}

JSONArray::~JSONArray()
{}

uint64_t JSONArray::getSize()
{
    return values.getSize();
}

uint_fast16_t JSONArray::addValue(Value *value)
{
    if (value == nullptr)
    {
        return ERR_NULL_VALUE;
    }

    if (IS_STRING(value) && ((StringValue *)value)->getValue() == nullptr)
    {
        delete value;
        return ERR_NULL_STR;
    }

    if (IS_ARR(value) && ((ArrayValue *)value)->getValue() == nullptr)
    {
        delete value;
        return ERR_NULL_ARR;
    }

    if (IS_DICT(value) && ((DictValue *)value)->getValue() == nullptr)
    {
        delete value;
        return ERR_NULL_DICT;
    }

    values.add(value);
    return 0;
}

Value **JSONArray::getValues()
{
    return values.getAsArray();
}

Value *JSONArray::getValueAt(uint_fast64_t index)
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
        Value *value = values.get(i);
        if (value == nullptr)
        {
            continue;
        }

        if (IS_ARR(value))
        {
            JSONArray *j = ((ArrayValue *)value)->getValue();
            if (j != nullptr)
            {
                j->printValuesIndent(indent + 1, false);
            }
        }
        else if (IS_DICT(value))
        {
            JSONDict *jd = ((DictValue *)value)->getValue();
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

uint_fast16_t JSONDict::addItem(Item *item)
{
    if (item == nullptr)
    {
        return ERR_NULL_ITEM;
    }

    if (item->getKey() == nullptr)
    {
        delete item;
        return ERR_NULL_KEY;
    }

    if (IS_STRING(item) && ((StringItem *)item)->getValue() == nullptr)
    {
        delete item;
        return ERR_NULL_STR;
    }

    if (IS_ARR(item) && ((ArrayItem *)item)->getValue() == nullptr)
    {
        delete item;
        return ERR_NULL_ARR;
    }

    if (IS_DICT(item) && ((DictItem *)item)->getValue() == nullptr)
    {
        delete item;
        return ERR_NULL_DICT;
    }

    // If an item with the same key already exists we don't add the item
    if (getItem(item->getKey()) != nullptr)
    {
#ifdef DEBUG
        cout << "The item with key '" << item->getKey()->str()
             << "' already exists, not adding it and freeing allocated "
                "memory. (Existing item : '"
             << item->getKey()->str() << ": ";
        getItem(item->getKey())->print();
        cout << endl;
#endif
        delete item;
        return ERR_ITEM_EXISTS;
    }

    items.add(item);
    return 0;
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
            if (key == it->getKey())
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
