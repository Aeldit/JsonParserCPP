#ifndef JSON_TYPES_H
#define JSON_TYPES_H

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <string>

/*******************************************************************************
**                              DEFINES / MACROS                              **
*******************************************************************************/
#define T_STR 0
#define T_INT 1
#define T_DOUBLE 2
#define T_BOOL 3
#define T_NULL 4
#define T_ARR 5
#define T_DICT 6

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

    virtual void printNoFlush() = 0;
    // Overriden by the Array and Dict class (TypedValue and Item)
    virtual void print();
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
    std::string key;

public:
    Item(std::string key, unsigned char type);
    virtual ~Item() = default;

    std::string getKey();

    void printKey();
};

#endif // !JSON_TYPES_H
