#include "validator.hpp"

#include "types.hpp"

bool json_valid_buff(char *b, uint_fast64_t len, bool is_dict)
{
    if (b == nullptr || len < 2)
    {
        return false;
    }

    // These 3 check if the number of array square brackets / dict curly
    // brackets / quotes are even
    uint_nested_arrays_t nb_arrays = is_dict ? 0 : 1;
    uint_nested_dicts_t nb_dicts = is_dict ? 1 : 0;
    uint_nested_dicts_t nb_quotes = 0;

    char is_in_string = 0;
    char is_backslashing = 0;

    // 1 = key | 2 = key + : | 3 = key + : + value
    // This variable is used to check for missing keys or values in json objects
    // It is set to 1 once the key is encountered, to 2 when the ':' is
    // encountered and to 3 when the final value is encountered
    char elts_encountered = 0;
    char string_encountered = 0;

    char c = 0;
    char prev_c = 0;
    // TODO: Check for missing commas
    while (len--)
    {
        // Gets the current char and makes the pointer point to the next one
        c = *b++;
        printf("%c", c);

        if (c == '\\' && !is_backslashing)
        {
            is_backslashing = !is_backslashing;
        }
        else if (c == '"' && !is_backslashing)
        {
            is_in_string = !is_in_string;
            if (is_in_string)
            {
                ++nb_quotes;
                string_encountered = 1;
            }
            else
            {
                --nb_quotes;
            }
            is_backslashing = 0;
        }
        else if (is_backslashing)
        {
            is_backslashing = 0;
        }
        else if (!is_in_string)
        {
            if (c == ' ' || c == '\n' || c == '\t')
            {
                continue;
            }

            // Key or string element encountered
            if (string_encountered)
            {
                elts_encountered = elts_encountered == 2 ? 3 : 1;
                string_encountered = 0;
            }
            // Other type of element encountered
            else if (elts_encountered == 2
                     && (('0' <= c && c <= '9') || c == 't' || c == 'f'
                         || c == 'n' || c == '[' || c == '{'))
            {
                elts_encountered = 3;
            }

            if (c == '[')
            {
                ++nb_arrays;
            }
            else if (c == ']')
            {
                --nb_arrays;
            }
            else if (c == '{')
            {
                ++nb_dicts;
                // Key:value pairs check
                if (elts_encountered && elts_encountered != 3)
                {
                    return false;
                }
            }
            else if (c == '}')
            {
                --nb_dicts;
            }
            else if (c == ':')
            {
                // Key:value pairs check
                if (elts_encountered != 1)
                {
                    return false;
                }
                elts_encountered = 2;
            }
            else if (c == ',' && prev_c != '}' && prev_c != ']')
            {
                // Key:value pairs check
                if (elts_encountered > 1 && elts_encountered != 3)
                {
                    return false;
                }
                elts_encountered = 0;
            }
        }
        prev_c = c;
    }
    return nb_arrays == 0 && nb_dicts == 0 && nb_quotes == 0;
}

bool json_valid(FILE *f)
{
    if (f == nullptr)
    {
        return false;
    }
    return true;
}
