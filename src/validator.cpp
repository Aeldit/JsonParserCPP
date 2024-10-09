#include "validator.hpp"

#include "types.hpp"

bool json_valid_buff(char *b, uint_fast64_t len, bool is_dict)
{
    if (b == nullptr || len < 2)
    {
        return false;
    }

    uint_nested_arrays_t nb_arrays = is_dict ? 0 : 1;
    uint_nested_dicts_t nb_dicts = is_dict ? 1 : 0;
    uint_nested_dicts_t nb_quotes = 0;

    char is_in_string = 0;
    char is_backslashing = 0;

    // 1 = key | 2 = key + : | 3 = key + : + value
    char elts_encountered = 0;
    uint_fast64_t nb_correct_elts = 0;

    // Used for dicts that are not associated with a key
    // char dict_has_key = 0;

    char c = 0;
    while (len--)
    {
        // Gets the current char and makes the pointer point to the next one
        c = *b++;
        printf("%c", c);

        if (!nb_dicts && elts_encountered)
        {
            elts_encountered = 0;
        }

        // TODO: Check for missing key/value in key:value pairs
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
                if (nb_dicts
                    && (elts_encountered == 0 || elts_encountered == 2))
                {
                    printf("#");
                    ++elts_encountered;
                }
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
            if (c == '[')
            {
                ++nb_arrays;
                if (elts_encountered == 2)
                {
                    printf("#");
                    ++elts_encountered;
                }
            }
            else if (c == ']')
            {
                --nb_arrays;
            }
            else if (c == '{')
            {
                ++nb_dicts;
                if (elts_encountered == 2)
                {
                    printf("#");
                    ++elts_encountered;
                }
            }
            else if (c == '}')
            {
                --nb_dicts;
            }
            else if (c == ':')
            {
                printf("#");
                ++elts_encountered;
            }
            else if ((c == ',' || c == '}' || c == '\n') && nb_dicts)
            {
                printf(" %d", elts_encountered);
                if (elts_encountered != 3)
                {
                    return false;
                }
                elts_encountered = 0;
                ++nb_correct_elts;
            }
            else if (('0' <= c && c <= '9') || c == 'n' || c == 't' || c == 'f')
            {
                if (elts_encountered == 2)
                {
                    printf("#");
                    ++elts_encountered;
                }
            }
            else if ((c == '}' || c == '\n' || c == ' ')
                     && elts_encountered == 3)
            {
                elts_encountered = 0;
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
