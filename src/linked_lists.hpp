#ifndef LINKED_LISTS_HPP
#define LINKED_LISTS_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BASE_ARRAY_LEN 2

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
template <class T>
class Link
{
public:
    T values[BASE_ARRAY_LEN] = { 0 };
    Link<T> *next = NULL;

    Link(){};
    ~Link()
    {
        for (short i = 0; i < BASE_ARRAY_LEN; ++i)
        {
            if (values[i] != NULL)
            {
                delete values[i];
            }
        }
    }
};

template <class T>
class LinkedList
{
private:
    uint64_t size = 0;
    uint64_t insert_idx = 0;
    Link<T> *head = NULL;
    Link<T> *tail = NULL;

public:
    LinkedList(){};
    ~LinkedList()
    {
        Link<T> *tmp = head;
        while (tmp != NULL)
        {
            Link<T> *t = tmp;
            tmp = tmp->next;
            delete t;
        }
    };

    void add(T value)
    {
        if (head == NULL)
        {
            head = new Link<T>();
            tail = head;
        }

        if (insert_idx >= BASE_ARRAY_LEN)
        {
            tail->next = new Link<T>();
            tail = tail->next;
            insert_idx = 0;
        }

        tail->values[insert_idx++] = value;
        ++size;
    }

    void print()
    {
        Link<T> *tmp = head;
        for (uint64_t i = 0; i < size; ++i)
        {
            if (tmp->values[i % BASE_ARRAY_LEN] == NULL)
            {
                continue;
            }
            tmp->values[i % BASE_ARRAY_LEN]->print();
            if (i != 0 && (i - 1) % BASE_ARRAY_LEN == 0)
            {
                tmp = tmp->next;
            }
        }
    }
};

#endif // !LINKED_LISTS_HPP
