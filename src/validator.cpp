#include "validator.hpp"

#include "types.hpp"

bool json_valid_buff(char *b, uint_fast64_t len, bool is_in_dict)
{
    if (b == nullptr || len < 2)
    {
        return false;
    }

    uint_nested_arrays_t nb_arrays = is_in_dict ? 0 : 1;
    uint_nested_dicts_t nb_dicts = is_in_dict ? 1 : 0;
    uint_nested_dicts_t nb_quotes = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    char c = 0;
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
            }
            else
            {
                --nb_quotes;
            }
        }
        else if (is_backslashing)
        {
            is_backslashing = 0;
        }
        else if (!is_in_string)
        {
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
            }
            else if (c == '}')
            {
                --nb_dicts;
            }
        }
    }
    printf("\n");
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
