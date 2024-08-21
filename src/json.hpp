#ifndef JSON_HPP
#define JSON_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <stdio.h>
#include <string>

#include "items.hpp"

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
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
