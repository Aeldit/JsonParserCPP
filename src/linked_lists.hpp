#ifndef LINKED_LISTS_HPP
#define LINKED_LISTS_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BASE_ARRAY_LEN 16

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
template <class T>
class Link
{
public:
    T *elts[BASE_ARRAY_LEN] = { 0 };
    Link<T> *next = NULL;

    Link(){};
    ~Link()
    {
        for (int i = 0; i < BASE_ARRAY_LEN; ++i)
        {
            delete elts[i];
        }
    };
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
    }

    uint64_t getSize()
    {
        return size;
    }

    T *get(uint64_t index)
    {
        if (head == NULL || index > size)
        {
            return NULL;
        }

        uint64_t link_number = index / BASE_ARRAY_LEN;
        // Goes the the link of number 'link_number' from the head
        Link<T> *link = head;
        for (uint64_t i = 0; i < link_number; ++i)
        {
            if (link == NULL)
            {
                return NULL;
            }
            link = link->next;
        }
        return link->elts[index % BASE_ARRAY_LEN];
    }

    void add(T *value)
    {
        if (value == NULL)
        {
            return;
        }

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

        tail->elts[insert_idx++] = value;
        ++size;
    }

    void print()
    {
        Link<T> *tmp = head;
        for (uint64_t i = 0; i < size; ++i)
        {
            if (tmp == NULL)
            {
                return;
            }

            if (tmp->elts[i % BASE_ARRAY_LEN] == NULL)
            {
                continue;
            }

            if (i != 0 && i % BASE_ARRAY_LEN == 0)
            {
                tmp = tmp->next;
            }
            tmp->elts[i % BASE_ARRAY_LEN]->print();
        }
    }
};

#endif // !LINKED_LISTS_HPP
