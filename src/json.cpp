#include "json.hpp"

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <iostream>
#include <string>

#include "types.h"

using namespace std;

/*******************************************************************************
**                                DECLARATIONS                                **
*******************************************************************************/
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