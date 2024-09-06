#ifndef LINKED_LISTS_HPP
#define LINKED_LISTS_HPP

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <stdint.h>

#define BASE_ARRAY_LEN 16

/*******************************************************************************
**                                   CLASSES                                  **
*******************************************************************************/
template <class T>
class Link
{
public:
    T *elts[BASE_ARRAY_LEN] = { 0 };
    Link<T> *next = nullptr;

    Link(){};
    ~Link()
    {
        for (unsigned char i = 0; i < BASE_ARRAY_LEN; ++i)
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
    Link<T> *head = nullptr;
    Link<T> *tail = nullptr;

    // TODO: Find after how many deletion we should defragment
    // FIX: Heap buffer overflow
    void defragment()
    {
        // Counts the number of empty elements (nullptr) followed by a non-null
        // element
        T **tmp_elts = new T *[size]();
        uint64_t insert_index = 0;
        Link<T> *link = head;
        while (link != nullptr)
        {
            for (unsigned char i = 0; i < BASE_ARRAY_LEN; ++i)
            {
                if (link->elts[i] != nullptr)
                {
                    tmp_elts[insert_index++] = link->elts[i];
                    link->elts[i] = nullptr;
                }
            }
            link = link->next;
        }

        // Refills the linked list without any gap
        link = head;
        // We re-use insert_idx but it does not serve the same purpose as before
        insert_index = 0;
        while (link != nullptr)
        {
            for (unsigned char i = 0; i < BASE_ARRAY_LEN; ++i)
            {
                link->elts[i] = tmp_elts[insert_index++];
            }

            if (insert_index == size)
            {
                break;
            }
            link = link->next;
        }

        // Removes the links that are not used and are empty
        if (link->next != nullptr)
        {
            Link<T> *tmp = link->next;
            while (tmp != nullptr)
            {
                Link<T> *t = tmp;
                tmp = tmp->next;
                delete t;
            }
            link->next = nullptr;
        }
        delete[] tmp_elts;
    }

public:
    LinkedList(){};
    ~LinkedList()
    {
        Link<T> *tmp = head;
        while (tmp != nullptr)
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

#ifdef DEBUG
    uint64_t getNbLinks()
    {
        if (head == nullptr)
        {
            return 0;
        }
        uint64_t nb_links = 0;
        Link<T> *link = head;
        while (link != nullptr)
        {
            link = link->next;
            ++nb_links;
        }
        return nb_links;
    }
#endif

    /**
    ** \brief Iterates over the arrays in the linked list and increments the
    **        number of elements encountered when the current element is not
    **        nullptr
    ** \param index The index of the element we want. This is NOT the index
    **              if the lists were put together in one, but the index of
    **              non-nullptr elements
    **              Ex: with the array [0, 4, 3, nullptr, 5, NULL, NULL, 8],
    **                  the element of index 4 is '8'
    ** \returns The element at the given index if it exists, nullptr otherwise
    */
    T *get(uint64_t index)
    {
        if (head == nullptr || index >= size)
        {
            return nullptr;
        }

        Link<T> *link = head;
        uint64_t nb_encountered = 0;
        while (link != nullptr)
        {
            for (unsigned char i = 0; i < BASE_ARRAY_LEN; ++i)
            {
                if (link->elts[i] != nullptr)
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
        return nullptr;
    }

    T **getAsArray()
    {
        if (head == nullptr)
        {
            return nullptr;
        }

        T **array = new T *[size]();
        if (array == nullptr)
        {
            return nullptr;
        }

        uint64_t local_insert_idx = 0;
        Link<T> *link = head;
        while (link != nullptr)
        {
            for (unsigned char i = 0; i < BASE_ARRAY_LEN; ++i)
            {
                if (link->elts[i] != nullptr)
                {
                    array[local_insert_idx++] = link->elts[i];
                }
            }
            link = link->next;
        }
        return array;
    }

    void add(T *value)
    {
        if (value == nullptr)
        {
            return;
        }

        if (head == nullptr)
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
        if (head == nullptr || index >= size)
        {
            return;
        }

        Link<T> *link = head;
        uint64_t nb_encountered = 0;
        bool done = false;
        while (link != nullptr)
        {
            for (unsigned char i = 0; i < BASE_ARRAY_LEN; ++i)
            {
                if (link->elts[i] != nullptr)
                {
                    if (nb_encountered == index)
                    {
                        delete link->elts[i];
                        link->elts[i] = nullptr;
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
