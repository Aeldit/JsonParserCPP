#include "custom_string.hpp"

FastCompString::FastCompString(const char *s, uint64_t len)
    : s(s)
    , len(len)
{}

bool FastCompString::operator==(FastCompString string)
{
    const char *c = string.s;
    if (s == nullptr || c == nullptr)
    {
        return false;
    }

    if (len != string.len)
    {
        return false;
    }

    for (uint64_t i = 0; i < len; ++i)
    {
        if (s[i] != c[i])
        {
            return false;
        }
    }
    return true;
}

const char *FastCompString::string()
{
    return s;
}
