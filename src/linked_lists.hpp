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
    uint64_t nb_deletion = 0;
    Link<T> *head = NULL;
    Link<T> *tail = NULL;

    // TODO: Find after how many deletion we should defragment
    // FIX: Heap buffer overflow
    void defragment()
    {
        // Counts the number of empty elements (NULL) followed by a non-null
        // element
        T **tmp_elts = new T *[size]();
        uint64_t insert_index = 0;
        Link<T> *link = head;
        while (link != NULL)
        {
            for (uint64_t i = 0; i < BASE_ARRAY_LEN; ++i)
            {
                if (link->elts[i] != NULL)
                {
                    tmp_elts[insert_index++] = link->elts[i];
                    link->elts[i] = NULL;
                }
            }
            link = link->next;
        }

        printf("equals : %lu %lu\n", insert_index, size);

        // Refills the linked list without any gap
        link = head;
        // We re-use insert_idx but it does not serve the same purpose as before
        insert_index = 0;
        while (link != NULL)
        {
            for (uint64_t i = 0; i < BASE_ARRAY_LEN; ++i)
            {
                printf("%lu ", insert_index);
                link->elts[i] = tmp_elts[insert_index++];
            }

            if (insert_index == size)
            {
                break;
            }
            link = link->next;
        }
        printf("\n");

        // Removes the links that are not used and are empty
        if (link->next != NULL)
        {
            Link<T> *tmp = link->next;
            while (tmp != NULL)
            {
                Link<T> *t = tmp;
                tmp = tmp->next;
                delete t;
            }
            link->next = NULL;
        }
        delete[] tmp_elts;
    }

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

    uint64_t getNbLinks()
    {
        if (head == NULL)
        {
            return 0;
        }
        uint64_t nb_links = 0;
        Link<T> *link = head;
        while (link != NULL)
        {
            link = link->next;
            ++nb_links;
        }
        return nb_links;
    }

    /**
    ** \brief Iterates over the arrays in the linked list and increments the
    **        number of elements encountered when the current element is not
    **        NULL
    ** \param index The index of the element we want. This is NOT the index
    **              if the lists were put together in one, but the index of
    **              non-NULL elements
    **              Ex: with the array [0, 4, 3, NULL, 5, NULL, NULL, 8],
    **                  the element of index 4 is '8'
    ** \returns The element at the given index if it exists, NULL otherwise
    */
    T *get(uint64_t index)
    {
        if (head == NULL || index >= size)
        {
            return NULL;
        }

        Link<T> *link = head;
        uint64_t nb_encountered = 0;
        while (link != NULL)
        {
            for (short i = 0; i < BASE_ARRAY_LEN; ++i)
            {
                if (link->elts[i] != NULL)
                {
                    if (nb_encountered == index)
                    {
                        return link->elts[i];
                    }
                    ++nb_encountered;
                }
            }
            link = link->next;
        }
        return NULL;
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

    void remove(uint64_t index)
    {
        if (head == NULL || index >= size)
        {
            return;
        }

        Link<T> *link = head;
        uint64_t nb_encountered = 0;
        bool done = false;
        while (link != NULL)
        {
            for (short i = 0; i < BASE_ARRAY_LEN; ++i)
            {
                if (link->elts[i] != NULL)
                {
                    if (nb_encountered == index)
                    {
                        delete link->elts[i];
                        link->elts[i] = NULL;
                        done = true;
                        break;
                    }
                    ++nb_encountered;
                }
            }
            if (done)
            {
                break;
            }
            link = link->next;
        }
        --size;

        ++nb_deletion;
        if (nb_deletion == BASE_ARRAY_LEN)
        {
            // defragment();
        }
    }
};

#endif // !LINKED_LISTS_HPP
