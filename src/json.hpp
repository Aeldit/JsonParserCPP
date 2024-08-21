#ifndef JSON_HPP
#define JSON_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <iostream>
#include <stdio.h>
#include <string>

#include "types.h"

using namespace std;

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
class Item
{
protected:
    string key;
    unsigned char type;

public:
    Item(string key)
        : key(key)
    {}

    string getKey()
    {
        return key;
    }

    void printKey()
    {
        cout << "\"" << key << "\"" << ": ";
    }

    virtual void print() = 0;

    virtual unsigned char getType() = 0;
};

class StringItem : public Item
{
private:
    string value;

public:
    StringItem(string key, string value)
        : Item(key)
        , value(value)
    {}

    string getValue()
    {
        return value;
    }

    unsigned char getType()
    {
        return TYPE_STR;
    }

    void print()
    {
        printKey();
        cout << "\"" << value << "\"";
    }
};

class IntItem : public Item
{
private:
    int value;

public:
    IntItem(string key, int value)
        : Item(key)
        , value(value)
    {}

    int getValue()
    {
        return value;
    }

    unsigned char getType()
    {
        return TYPE_NUM;
    }

    void print()
    {
        printKey();
        cout << value;
    }
};

class BoolItem : public Item
{
private:
    bool value;

public:
    BoolItem(string key, bool value)
        : Item(key)
        , value(value)
    {}

    bool getValue()
    {
        return value;
    }

    unsigned char getType()
    {
        return TYPE_BOOL;
    }

    void print()
    {
        printKey();
        cout << (value ? "true" : "false");
    }
};

class NullItem : public Item
{
public:
    NullItem(string key)
        : Item(key)
    {}

    unsigned char getType()
    {
        return TYPE_NULL;
    }

    void print()
    {
        printKey();
        cout << "null";
    }
};

class JSONDict
{
private:
    size_t size;
    Item **items;
    size_t insert_idx = 0;

public:
    JSONDict(size_t size)
        : size(size)
    {
#ifdef DEBUG
        cout << "Initializing " << size << " items" << endl;
#endif
        items = (Item **)calloc(size, sizeof(Item *));
    }

    ~JSONDict()
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

    size_t getSize()
    {
        return size;
    }

    Item **getItems()
    {
        return items;
    }

    Item *getItem(string key)
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
    }

    void addItem(Item *item)
    {
        if (items == NULL || item == NULL || insert_idx >= size)
        {
            return;
        }

        items[insert_idx++] = item;
    }

    void printItems()
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
};

#endif // !JSON_HPP
