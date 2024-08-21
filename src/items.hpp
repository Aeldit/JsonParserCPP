#ifndef ITEMS_HPP
#define ITEMS_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <string>

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
    int64_t value;

public:
    IntItem(string key, int64_t value);

    void print();
    unsigned char getType();
    int64_t getValue();
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

#endif // !ITEMS_HPP