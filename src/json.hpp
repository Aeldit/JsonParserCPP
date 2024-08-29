#ifndef JSON_HPP
#define JSON_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <string>

#include "linked_lists.hpp"

using namespace std;

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
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

    virtual void printNoFlush() = 0;
    virtual void print() = 0;
};

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
class Item : public TypedValue
{
private:
    string key;

public:
    Item(string key, unsigned char type);
    virtual ~Item() = default;

    string getKey();

    void printKey();
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
    LinkedList<TypedValue *> values;

public:
    JSONArray();
    virtual ~JSONArray();

    uint64_t getSize();
    LinkedList<TypedValue *> getValues();
    TypedValue *getValueAt(uint64_t index);

    void addValue(TypedValue *value);
    void printValues();
    void printValuesIndent(int indent, bool fromDict);
};

/**
** \class JSONDict
** \implements JSON
** \param items An array of Items pointers (only contains objects of
**              classes that are derived from the Item class)
*/
class JSONDict : public JSON
{
private:
    LinkedList<Item *> items;

public:
    JSONDict();
    virtual ~JSONDict();

    uint64_t getSize();
    LinkedList<Item *> getItems();
    Item *getItem(string key);

    void addItem(Item *item);
    void printItems();
    void printItemsIndent(int indent, bool fromDict);
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

    void printNoFlush();
    void print();
    string getValue();
};

class IntTypedValue : public TypedValue
{
private:
    int64_t value;

public:
    IntTypedValue(int64_t value);

    void printNoFlush();
    void print();
    int64_t getValue();
};

class DoubleTypedValue : public TypedValue
{
private:
    double value;

public:
    DoubleTypedValue(double value);

    void printNoFlush();
    void print();
    double getValue();
};

class BoolTypedValue : public TypedValue
{
private:
    bool value;

public:
    BoolTypedValue(bool value);

    void printNoFlush();
    void print();
    bool getValue();
};

class NullTypedValue : public TypedValue
{
public:
    NullTypedValue();

    void printNoFlush();
    void print();
};

class ArrayTypedValue : public TypedValue
{
private:
    JSONArray *ja;

public:
    ArrayTypedValue(JSONArray *ja_arg);
    virtual ~ArrayTypedValue();

    void printNoFlush();
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

    void printNoFlush();
    void print();
    JSONDict *getValue();
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

    void printNoFlush();
    void print();
    string getValue();
};

class IntItem : public Item
{
private:
    int64_t value;

public:
    IntItem(string key, int64_t value);

    void printNoFlush();
    void print();
    int64_t getValue();
};

class DoubleItem : public Item
{
private:
    double value;

public:
    DoubleItem(string key, double value);

    void printNoFlush();
    void print();
    double getValue();
};

class BoolItem : public Item
{
private:
    bool value;

public:
    BoolItem(string key, bool value);

    void printNoFlush();
    void print();
    bool getValue();
};

class NullItem : public Item
{
public:
    NullItem(string key);

    void printNoFlush();
    void print();
};

class ArrayItem : public Item
{
private:
    JSONArray *ja;

public:
    ArrayItem(string key, JSONArray *ja_arg);
    virtual ~ArrayItem();

    void printNoFlush();
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

    void printNoFlush();
    void print();
    JSONDict *getValue();
};

#endif // !JSON_HPP
