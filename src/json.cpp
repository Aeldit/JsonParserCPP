#include "json.hpp"

/*******************************************************************************
**                                DECLARATIONS                                **
*******************************************************************************/
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
IntItem::IntItem(string key, int value)
    : Item(key)
    , value(value)
{}

int IntItem::getValue()
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
**             JSONDICT              **
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

        if (items[i]->getType() == TYPE_OBJ)
        {
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