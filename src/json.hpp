#ifndef JSON_HPP
#define JSON_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <string>

#include "custom_string.hpp"
#include "linked_lists.hpp"
#include "types.hpp"

/*******************************************************************************
**                              DEFINES / MACROS                              **
*******************************************************************************/
#define IS_STRING(v) (dynamic_cast<TypedValue *>(v) && (v)->getType() == T_STR)
#define IS_INT(v) (dynamic_cast<TypedValue *>(v) && (v)->getType() == T_INT)
#define IS_DOUBLE(v)                                                           \
    (dynamic_cast<TypedValue *>(v) && (v)->getType() == T_DOUBLE)
#define IS_BOOL(v) (dynamic_cast<TypedValue *>(v) && (v)->getType() == T_BOOL)
#define IS_NULL(v) (dynamic_cast<TypedValue *>(v) && (v)->getType() == T_NULL)
#define IS_ARR(v) (dynamic_cast<TypedValue *>(v) && (v)->getType() == T_ARR)
#define IS_DICT(v) (dynamic_cast<TypedValue *>(v) && (v)->getType() == T_DICT)

#define IS_JSON_ARRAY(j) (dynamic_cast<JSON *>(j) && (j)->isArray())

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
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
    LinkedList<TypedValue> *values;

public:
    JSONArray();
    ~JSONArray();

    uint64_t getSize();
    LinkedList<TypedValue> *getValues();
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
    LinkedList<Item> *items;

public:
    JSONDict();
    ~JSONDict();

    uint64_t getSize();
    LinkedList<Item> *getItems();
    Item *getItem(FastCompString key);
    bool keyExists(FastCompString key);

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
    std::string value;

public:
    StringTypedValue(std::string value);

    void printNoFlush();
    void print();
    std::string getValue();
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
    std::string value;

public:
    StringItem(FastCompString key, std::string value);

    void printNoFlush();
    void print();
    std::string getValue();
};

class IntItem : public Item
{
private:
    int64_t value;

public:
    IntItem(FastCompString key, int64_t value);

    void printNoFlush();
    void print();
    int64_t getValue();
};

class DoubleItem : public Item
{
private:
    double value;

public:
    DoubleItem(FastCompString key, double value);

    void printNoFlush();
    void print();
    double getValue();
};

class BoolItem : public Item
{
private:
    bool value;

public:
    BoolItem(FastCompString key, bool value);

    void printNoFlush();
    void print();
    bool getValue();
};

class NullItem : public Item
{
public:
    NullItem(FastCompString key);

    void printNoFlush();
    void print();
};

class ArrayItem : public Item
{
private:
    JSONArray *ja;

public:
    ArrayItem(FastCompString key, JSONArray *ja_arg);
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
    DictItem(FastCompString key, JSONDict *jd_arg);
    virtual ~DictItem();

    void printNoFlush();
    void print();
    JSONDict *getValue();
};

#endif // !JSON_HPP
