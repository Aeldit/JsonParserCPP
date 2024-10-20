#ifndef JSON_HPP
#define JSON_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
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
    LinkedList<TypedValue> values;

public:
    JSONArray();
    ~JSONArray();

    uint_fast64_t getSize();
    TypedValue **getValues();
    TypedValue *getValueAt(uint_fast64_t index);

    uint_fast16_t addValue(TypedValue *value);
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
    LinkedList<Item> items;

public:
    JSONDict();
    ~JSONDict();

    uint_fast64_t getSize();
    Item **getItems();
    Item *getItem(String *key);

    uint_fast16_t addItem(Item *item);
    void printItems();
    void printItemsIndent(int indent, bool fromDict);
};

/**************************************
**           TYPED VALUES            **
**************************************/
class StringTypedValue : public TypedValue
{
private:
    String *value;

public:
    StringTypedValue(String *value);
    ~StringTypedValue();

    void printNoFlush();
    String *getValue();
};

class IntTypedValue : public TypedValue
{
private:
    int_fast64_t value;

public:
    IntTypedValue(int_fast64_t value);

    void printNoFlush();
    int_fast64_t getValue();
};

class DoubleTypedValue : public TypedValue
{
private:
    double value;

public:
    DoubleTypedValue(double value);

    void printNoFlush();
    double getValue();
};

class BoolTypedValue : public TypedValue
{
private:
    bool value;

public:
    BoolTypedValue(bool value);

    void printNoFlush();
    bool getValue();
};

class NullTypedValue : public TypedValue
{
public:
    NullTypedValue();

    void printNoFlush();
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
    String *value;

public:
    StringItem(String *key, String *value);
    ~StringItem();

    void printNoFlush();
    String *getValue();
};

class IntItem : public Item
{
private:
    int64_t value;

public:
    IntItem(String *key, int64_t value);

    void printNoFlush();
    int64_t getValue();
};

class DoubleItem : public Item
{
private:
    double value;

public:
    DoubleItem(String *key, double value);

    void printNoFlush();
    double getValue();
};

class BoolItem : public Item
{
private:
    bool value;

public:
    BoolItem(String *key, bool value);

    void printNoFlush();
    bool getValue();
};

class NullItem : public Item
{
public:
    NullItem(String *key);

    void printNoFlush();
};

class ArrayItem : public Item
{
private:
    JSONArray *ja;

public:
    ArrayItem(String *key, JSONArray *ja_arg);
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
    DictItem(String *key, JSONDict *jd_arg);
    virtual ~DictItem();

    void printNoFlush();
    void print();
    JSONDict *getValue();
};

#endif // !JSON_HPP
