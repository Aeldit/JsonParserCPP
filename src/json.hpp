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
class TypedValueBase
{
private:
    unsigned char type;

public:
    TypedValueBase(unsigned char type)
        : type(type){};
    virtual ~TypedValueBase() = default;

    template <class T>
    T getValue();

    unsigned char getType()
    {
        return type;
    };

    bool isString()
    {
        return type == T_STR;
    }

    bool isInt()
    {
        return type == T_NUM;
    }

    bool isDouble()
    {
        return type == T_DOUBLE;
    }

    bool isBool()
    {
        return type == T_BOOL;
    }

    bool isNull()
    {
        return type == T_NULL;
    }

    bool isArray()
    {
        return type == T_ARR;
    }

    bool isDict()
    {
        return type == T_DICT;
    }

    virtual void print();
};

class ItemBase
{
private:
    string key;
    unsigned char type;

public:
    ItemBase(string key, unsigned char type)
        : key(key)
        , type(type){};
    virtual ~ItemBase() = default;

    string getKey()
    {
        return key;
    }

    template <class T>
    T getValue();

    unsigned char getType()
    {
        return type;
    };

    bool isString()
    {
        return type == T_STR;
    }

    bool isInt()
    {
        return type == T_NUM;
    }

    bool isDouble()
    {
        return type == T_DOUBLE;
    }

    bool isBool()
    {
        return type == T_BOOL;
    }

    bool isNull()
    {
        return type == T_NULL;
    }

    bool isArray()
    {
        return type == T_ARR;
    }

    bool isDict()
    {
        return type == T_DICT;
    }

    void printKey()
    {
        cout << '"' << key << "\": ";
    }

    virtual void print();
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
    TypedValueBase **values;

public:
    JSONArray(size_t size);
    virtual ~JSONArray();

    size_t getSize();
    TypedValueBase **getValues();
    TypedValueBase *getValueAt(size_t index);

    void add(TypedValueBase *value);
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
    ItemBase **items;

public:
    JSONDict(size_t size);
    virtual ~JSONDict();

    size_t getSize();
    ItemBase **getItems();
    ItemBase *getItem(string key);

    void addItem(ItemBase *item);
    void printItems();
    void printItemsIndent(int indent, bool fromDict);
};

template <class T>
class TypedValue : public TypedValueBase
{
private:
    T value;

public:
    TypedValue(unsigned char type, T value)
        : TypedValueBase(type)
        , value(value){};
    virtual ~TypedValue(){};

    T getValue()
    {
        return value;
    }

    // TODO: Ensure 'endl'
    void print()
    {
        cout << value;
        /*if (isString())
        {
            cout << "\"" << ((string)value) << "\"";
        }
        else if (isInt() || isBool())
        {
            cout << value;
        }
        else if (isDouble())
        {
            cout << setprecision(16) << ((double)value);
        }
        else if (isNull())
        {
            cout << "null";
        }
        else if (isArray())
        {
            JSONArray *j = dynamic_cast<JSONArray *>(value);
            if (j != NULL)
            {
                j->printValues();
            }
        }
        else if (isDict())
        {
            JSONDict *j = dynamic_cast<JSONDict *>(value);
            if (j != NULL)
            {
                j->printItems();
            }
        }*/
        cout << endl;
    }
};

template <class T>
class Item : public ItemBase
{
private:
    T value;

public:
    Item(string key, unsigned char type, T value)
        : ItemBase(key, type)
        , value(value){};
    virtual ~Item(){};

    T getValue()
    {
        return value;
    };

    void print()
    {
        printKey();
        cout << value << endl;
    }
};

#endif // !JSON_HPP
