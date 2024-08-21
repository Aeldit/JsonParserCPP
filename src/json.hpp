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
    Item(string key);

    string getKey();

    void printKey();

    virtual void print() = 0;
    virtual unsigned char getType() = 0;
};

class StringItem : public Item
{
private:
    string value;

public:
    StringItem(string key, string value);

    void print();
    unsigned char getType();
    string getValue();
};

class IntItem : public Item
{
private:
    int value;

public:
    IntItem(string key, int value);

    void print();
    unsigned char getType();
    int getValue();
};

class BoolItem : public Item
{
private:
    bool value;

public:
    BoolItem(string key, bool value);

    void print();
    unsigned char getType();
    bool getValue();
};

class NullItem : public Item
{
public:
    NullItem(string key);

    void print();
    unsigned char getType();
};

class JSONDict
{
private:
    size_t size;
    Item **items;
    size_t insert_idx = 0;

public:
    JSONDict(size_t size);
    ~JSONDict();

    size_t getSize();
    Item **getItems();
    Item *getItem(string key);

    void addItem(Item *item);
    void printItems();
};

#endif // !JSON_HPP
