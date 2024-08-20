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

public:
    Item(string key)
        : key(key)
    {}

    string getKey()
    {
        return key;
    }
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

    void print()
    {
        cout << key << ": " << value << endl;
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

    void print()
    {
        cout << key << ": " << value << endl;
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

    void print()
    {
        cout << key << ": " << (value ? "true" : "false") << endl;
    }
};

class JSONDict
{
private:
    size_t n_elts;
};

#endif // !JSON_HPP
