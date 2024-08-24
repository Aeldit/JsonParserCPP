#ifndef JSON_HPP
#define JSON_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <stdio.h>
#include <string>

using namespace std;

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
class Item
{
private:
    string key;
    unsigned char type;

public:
    Item(string key, unsigned char type);

    string getKey();
    unsigned char getType();

    void printKey();

    virtual void print() {}; // Not implemented by array and dict objects
};

class TypedValue
{
private:
    unsigned char type;

public:
    TypedValue(unsigned char type);

    unsigned char getType();
    virtual void print() {};
};

/**************************************
**               JSON                **
**************************************/
class JSON
{
private:
    bool is_array;

public:
    JSON(bool is_array);

    bool isArray();
};

class JSONArray : public JSON
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
    void printValuesIndent(int indent, bool fromDict);
};

class JSONDict : public JSON
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
    void printItemsIndent(int indent, bool fromDict);
};

/**************************************
**               ITEMS               **
**************************************/
class StringItem : public Item
{
private:
    string value;

public:
    StringItem(string key, string value);

    void print();
    string getValue();
};

class IntItem : public Item
{
private:
    int64_t value;

public:
    IntItem(string key, int64_t value);

    void print();
    int64_t getValue();
};

class BoolItem : public Item
{
private:
    bool value;

public:
    BoolItem(string key, bool value);

    void print();
    bool getValue();
};

class NullItem : public Item
{
public:
    NullItem(string key);

    void print();
};

class ArrayItem : public Item
{
private:
    JSONArray *ja;

public:
    ArrayItem(string key, JSONArray *ja_arg);

    JSONArray *getValue();
};

class DictItem : public Item
{
private:
    JSONDict *jd;

public:
    DictItem(string key, JSONDict *jd_arg);

    JSONDict *getValue();
};

/**************************************
**           TYPED VALUES            **
**************************************/
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
    JSONArray *ja;

public:
    ArrayTypedValue(JSONArray *ja_arg);

    JSONArray *getValue();
};

class DictTypedValue : public TypedValue
{
private:
    JSONDict *jd;

public:
    DictTypedValue(JSONDict *jd_arg);

    JSONDict *getValue();
};

#endif // !JSON_HPP
