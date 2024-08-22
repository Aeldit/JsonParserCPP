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
    cout << "\"" << key << "\"" << ": ";
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
ArrayItem::ArrayItem(string key, JSONArray *ja)
    : Item(key, TYPE_ARR)
    , ja(*ja)
{
    free(ja);
}

JSONArray *ArrayItem::getValue()
{
    return &ja;
}

/**************************************
**             DICT ITEM             **
**************************************/
DictItem::DictItem(string key, JSONDict *jd)
    : Item(key, TYPE_DICT)
    , jd(*jd)
{
    free(jd);
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
    printValuesIndent(1, false);
}

void JSONArray::printValuesIndent(int indent, bool fromDict)
{
    // Obtains the number of tab characters that will be printed
    char *tabs = (char *)calloc(indent, sizeof(char));
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
    free(tabs);
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
        cout << "B" << endl;
        return NULL;
    }

    for (size_t i = 0; i < size; ++i)
    {
        if (items[i] == NULL)
        {
            continue;
        }

        cout << key << key.size() << items[i]->getKey()
             << items[i]->getKey().size() << key.compare(items[i]->getKey())
             << endl;
        if (key.compare(items[i]->getKey()) == 0)
        {
            return items[i];
        }
    }
    cout << "C" << endl;
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
    char *tabs = (char *)calloc(indent, sizeof(char));
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
    free(tabs);
}
