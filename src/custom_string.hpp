#ifndef CUSTOM_STRING_H
#define CUSTOM_STRING_H

#include <stdint.h>

class FastCompString
{
private:
    const char *s;
    uint64_t len;

public:
    FastCompString(const char *s, uint64_t len);
    /**
    ** \returns Whether the given string is equal to the current one.
    **          Unlike the method string::compare, this stops as soon as the
    **          string is different
    */
    bool operator==(FastCompString string);
    const char *string();
};

#endif // !CUSTOM_STRING_H
