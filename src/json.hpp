#ifndef JSON_HPP
#define JSON_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <stdio.h>
#include <string>

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
/**************************************
**               ITEMS               **
**************************************/
class Item
{
protected:
    string key;
    unsigned char type;

public:
    Item(string key);

    string getKey();

    void printKey();

    virtual void print() {}; // Not implemented by array and dict objects
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

class ArrayItem : public Item
{
private:
    JSONArray ja;

public:
    ArrayItem(string key, JSONArray ja);

    void print();
    unsigned char getType();
    JSONArray *getValue();
};

class DictItem : public Item
{
private:
    JSONDict jd;

public:
    DictItem(string key, JSONDict jd);

    void print();
    unsigned char getType();
    JSONDict *getValue();
};

/**************************************
**           TYPED VALUES            **
**************************************/
class TypedValue
{
protected:
    unsigned char type;

public:
    TypedValue(unsigned char type);

    unsigned char getType();

    virtual void print() {};
};

class StringTypedValue : public TypedValue
{
private:
    string value;

public:
    StringTypedValue(string value);

    void print();
    string getValue();
};

class IntTypedValue : public TypedValue
{
private:
    int64_t value;

public:
    IntTypedValue(int64_t value);

    void print();
    int64_t getValue();
};

class BoolTypedValue : public TypedValue
{
private:
    bool value;

public:
    BoolTypedValue(bool value);

    void print();
    bool getValue();
};

class NullTypedValue : public TypedValue
{
public:
    NullTypedValue();

    void print();
};

class ArrayTypedValue : public TypedValue
{
private:
    JSONArray ja;

public:
    ArrayTypedValue(JSONArray ja);

    void print();
    JSONArray *getValue();
};

class DictTypedValue : public TypedValue
{
private:
    JSONDict jd;

public:
    DictTypedValue(JSONDict jd);

    void print();
    JSONDict *getValue();
};

/**************************************
**               JSON                **
**************************************/
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

class JSONArray
{
private:
    size_t size;
    TypedValue **values;
    size_t insert_idx = 0;

public:
    JSONArray(size_t size);
    ~JSONArray();

    size_t getSize();
    TypedValue **getValues();
    TypedValue *getValueAt(size_t index);

    void add(TypedValue *value);
    void printValues();
};

#endif // !JSON_HPP
