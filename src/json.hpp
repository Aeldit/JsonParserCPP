#ifndef JSON_HPP
#define JSON_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <cstddef>
#include <iostream>
#include <string>

#include "types.h"

using namespace std;

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
/**
** \class Item Base class representing a JSONDict item
** \brief The following classes are derived from this one :
**        - StringItem
**        - IntItem
**        - DoubleItem
**        - BoolItem
**        - NullItem
**        - ArrayItem
**        - DictItem
** \param key The key of the item (string)
** \param type The type of the item's value (see types.h)
*/
class Item
{
private:
    string key;
    unsigned char type;

public:
    Item(string key, unsigned char type);
    virtual ~Item() = default;

    string getKey();
    unsigned char getType();

    void printKey();

    bool isString();
    bool isInt();
    bool isDouble();
    bool isBool();
    bool isNull();
    bool isArray();
    bool isDict();

    virtual void print() = 0;
};

template <class T>
class ItemT
{
private:
    string key;
    unsigned char type;
    T value;

public:
    ItemT(string key, unsigned char type, T value)
        : key(key)
        , type(type)
        , value(value){};
    ~ItemT(){
        /*if (type == TYPE_ARR || type == TYPE_DICT)
        {
            delete value;
        }*/
    };

    string getKey()
    {
        return key;
    };

    unsigned char getType()
    {
        return type;
    };

    T getValue()
    {
        return value;
    };

    void printKey()
    {
        cout << "\"" << key << "\""
             << ": ";
    };

    bool isString()
    {
        return type == TYPE_STR;
    }

    bool isInt()
    {
        return type == TYPE_NUM;
    }

    bool isDouble()
    {
        return type == TYPE_DOUBLE;
    }

    bool isBool()
    {
        return type == TYPE_BOOL;
    }

    bool isNull()
    {
        return type == TYPE_NULL;
    }

    bool isArray()
    {
        return type == TYPE_ARR;
    }

    bool isDict()
    {
        return type == TYPE_DICT;
    }

    void print()
    {
        cout << value << endl;
    }
};

/**
** \class TypedValue Base class representing a JSONArray's value
** \brief The following classes are derived from this one :
**        - StringTypedValue
**        - IntTypedValue
**        - DoubleTypedValue
**        - BoolTypedValue
**        - NullTypedValue
**        - ArrayTypedValue
**        - DictTypedValue
** \param type The type of the value (see types.h)
*/
class TypedValue
{
private:
    unsigned char type;

public:
    TypedValue(unsigned char type);
    virtual ~TypedValue() = default;

    unsigned char getType();

    bool isString();
    bool isInt();
    bool isDouble();
    bool isBool();
    bool isNull();
    bool isArray();
    bool isDict();

    virtual void print() = 0;
};

/**************************************
**               JSON                **
**************************************/
/**
** \class JSON Base class representing a JSON object (dict or array)
** \brief The following classes are derived from this one :
**        - JSONArray
**        - JSONDict
** \param is_array Whether the JSON object is an array or a dict
*/
class JSON
{
private:
    bool is_array;

public:
    JSON(bool is_array);
    virtual ~JSON() = default;

    bool isArray();
};

/**
** \class JSONArray
** \implements JSON
** \param size The number of values inside the array
** \param insert_idx The index where the next value will be added
** \param values An array of TypedValues pointers (only contains objects of
**               classes that are derived from the TypedValue class)
*/
class JSONArray : public JSON
{
private:
    size_t size;
    size_t insert_idx = 0;
    TypedValue **values;

public:
    JSONArray(size_t size);
    virtual ~JSONArray();

    size_t getSize();
    TypedValue **getValues();
    TypedValue *getValueAt(size_t index);

    void add(TypedValue *value);
    void printValues();
    void printValuesIndent(int indent, bool fromDict);
};

/**
** \class JSONDict
** \implements JSON
** \param size The number of items inside the dict
** \param insert_idx The index where the next item will be added
** \param items An array of Items pointers (only contains objects of
**               classes that are derived from the Item class)
*/
class JSONDict : public JSON
{
private:
    size_t size;
    size_t insert_idx = 0;
    Item **items;

public:
    JSONDict(size_t size);
    virtual ~JSONDict();

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

class DoubleItem : public Item
{
private:
    double value;

public:
    DoubleItem(string key, double value);

    void print();
    double getValue();
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
    virtual ~ArrayItem();

    void print();
    JSONArray *getValue();
};

class DictItem : public Item
{
private:
    JSONDict *jd;

public:
    DictItem(string key, JSONDict *jd_arg);
    virtual ~DictItem();

    void print();
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

class DoubleTypedValue : public TypedValue
{
private:
    double value;

public:
    DoubleTypedValue(double value);

    void print();
    double getValue();
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
    virtual ~ArrayTypedValue();

    void print();
    JSONArray *getValue();
};

class DictTypedValue : public TypedValue
{
private:
    JSONDict *jd;

public:
    DictTypedValue(JSONDict *jd_arg);
    virtual ~DictTypedValue();

    void print();
    JSONDict *getValue();
};

#endif // !JSON_HPP
