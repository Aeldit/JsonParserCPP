#ifndef JSON_HPP
#define JSON_HPP

#include <any>
#include <iostream>
#include <stdio.h>
#include <string>

#include "types.h"

using namespace std;

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
    size_t nb_elts = 0;
    Item *items[ARRAY_SIZE];

public:
    size_t getNbElts()
    {
        return nb_elts;
    }

    Item **getItems()
    {
        return items;
    }

    Item *getItem(string key)
    {
        for (size_t i = 0; i < nb_elts; ++i)
        {
            if (key.compare(items[i]->getKey()) == 0)
            {
                return items[i];
            }
        }
    }

    void addItem(Item *item)
    {
        if (nb_elts == ARRAY_SIZE - 1)
        {
            return;
        }

        items[nb_elts++] = item;
    }

    void printItems()
    {
        cout << "{" << endl;

        for (size_t i = 0; i < nb_elts; ++i)
        {
            cout << "\t";
            items[i]->print();

            if (i < nb_elts - 1)
            {
                cout << "," << endl;
            }
        }

        cout << "\n}" << endl;
    }
};

#endif // !JSON_HPP
