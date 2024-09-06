#include "custom_string.hpp"

#include <string>

bool stringsEqual(std::string a, std::string b)
{
    if (a.length() != b.length())
    {
        return false;
    }

    uint64_t len = a.length();
    const char *a_str = a.c_str();
    const char *b_str = b.c_str();
    for (uint64_t i = 0; i < len; ++i)
    {
        if (a_str[i] != b_str[i])
        {
            return false;
        }
    }
    return true;
}
