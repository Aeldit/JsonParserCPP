#include "parser.hpp"

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "json.hpp"

using namespace std;

/*******************************************************************************
**                              DEFINES / MACROS                              **
*******************************************************************************/
#define IS_NUMBER_START(c) (('0' <= (c) && (c) <= '9') || (c) == '-')
#define IS_BOOL_START(c) ((c) == 't' || (c) == 'f')

#define ARRAY_END_REACHED                                                      \
    (!is_in_string && !is_in_array && (c == '\n' || c == ','))
#define DICT_END_REACHED                                                       \
    (!is_in_string && !is_in_dict && (c == '\n' || c == ','))

/**
** \def Calculates the size of the given value (string, number or boolean).
**      These defines are used to limit the number of function calls
*/
#define STR_LEN                                                                \
    uint64_t size = (*pos);                                                    \
    char c = '\0';                                                             \
    char prev_c = '\0';                                                        \
    while ((c = fgetc(f)) != EOF)                                              \
    {                                                                          \
        if (c == '"' && prev_c != '\\')                                        \
        {                                                                      \
            break;                                                             \
        }                                                                      \
        if (fseek(f, size++, SEEK_SET) != 0)                                   \
        {                                                                      \
            break;                                                             \
        }                                                                      \
        prev_c = c;                                                            \
    }                                                                          \
    uint64_t len = size - (*pos) - 1

#define VAL_LEN                                                                \
    uint64_t size = (*pos);                                                    \
    if (fseek(f, size++, SEEK_SET) != 0)                                       \
    {                                                                          \
        return 0;                                                              \
    }                                                                          \
    char c = '\0';                                                             \
    while ((c = fgetc(f)) != EOF)                                              \
    {                                                                          \
        if (c == ',' || c == '\n')                                             \
        {                                                                      \
            break;                                                             \
        }                                                                      \
        if (fseek(f, size++, SEEK_SET) != 0)                                   \
        {                                                                      \
            break;                                                             \
        }                                                                      \
    }                                                                          \
    uint64_t len = size - (*pos) - 1

#define ARR_LEN                                                                \
    uint64_t size = offset;                                                    \
    if (fseek(f, size++, SEEK_SET) != 0)                                       \
    {                                                                          \
        return 0;                                                              \
    }                                                                          \
    char c = '\0';                                                             \
    char is_in_array = 0;                                                      \
    char is_in_string = 0;                                                     \
    while ((c = fgetc(f)) != EOF)                                              \
    {                                                                          \
        if (ARRAY_END_REACHED)                                                 \
        {                                                                      \
            break;                                                             \
        }                                                                      \
        if (c == '"')                                                          \
        {                                                                      \
            is_in_string = !is_in_string;                                      \
        }                                                                      \
        else if (c == '[')                                                     \
        {                                                                      \
            ++is_in_array;                                                     \
        }                                                                      \
        else if (c == ']')                                                     \
        {                                                                      \
            --is_in_array;                                                     \
        }                                                                      \
        if (fseek(f, size++, SEEK_SET) != 0)                                   \
        {                                                                      \
            break;                                                             \
        }                                                                      \
    }                                                                          \
    uint64_t len = size - offset - 1

/*******************************************************************************
**                           FUNCTIONS DECLARATIONS                           **
*******************************************************************************/
JSONDict *parse_json_dict(FILE *f, uint64_t *pos);

/*******************************************************************************
**                              LOCAL FUNCTIONS **
*******************************************************************************/
/**
** \brief Reads the string at position 'pos' in the given file, and adds it to
**        the given dict
** \param pos The pos of the '"' that starts the string of which we are
**            currently acquiring the length
*/
string parse_string(FILE *f, uint64_t *pos)
{
    if (f == NULL || pos == NULL)
    {
        return string();
    }

    STR_LEN;
    if (len == 0)
    {
        return string();
    }

    char *str = (char *)calloc(len + 1, sizeof(char));
    if (str == NULL)
    {
        return string();
    }

    for (uint64_t i = 0; i < len; ++i)
    {
        if (fseek(f, (*pos)++, SEEK_SET) != 0)
        {
            break;
        }
        str[i] = fgetc(f);
    }
    ++(*pos); // Because otherwise, we end up reading the last '"' of the str
    string fstr(str); // Converts from char * to string
    free(str);
    return fstr;
}

int64_t str_to_long(char *str, uint64_t len)
{
    if (str == NULL || len == 0)
    {
        return 0;
    }
    int64_t res = 0;
    char is_negative = 1;

    for (uint64_t i = 0; i < len; ++i)
    {
        if (str[i] == '-')
        {
            is_negative = -1;
        }
        else if ('0' <= str[i] && str[i] <= '9')
        {
            res = res * 10 + str[i] - '0';
        }
    }
    return res * is_negative;
}

// TODO: Handle floats
int64_t parse_number(FILE *f, uint64_t *pos)
{
    if (f == NULL || pos == NULL)
    {
        return 0;
    }

    // Because we already read the first digit (or sign)
    --(*pos);

    VAL_LEN;
    if (len == 0)
    {
        return 0;
    }

    char *str = (char *)calloc(len + 1, sizeof(char));
    if (str == NULL)
    {
        return 0;
    }

    for (uint64_t i = 0; i < len; ++i)
    {
        if (fseek(f, (*pos)++, SEEK_SET) != 0)
        {
            break;
        }
        str[i] = fgetc(f);
    }
    int64_t res = str_to_long(str, len);
    free(str);
    return res;
}

/**
** \returns 5 if false, 4 if true, 0 otherwise
**/
uint64_t parse_boolean(FILE *f, uint64_t *pos)
{
    if (f == NULL || pos == NULL)
    {
        return 0;
    }

    // Because we already read the first character
    --(*pos);

    VAL_LEN;
    (*pos) += len;
    return len;
}

uint64_t get_nb_elts_array(FILE *f, uint64_t pos)
{
    if (f == NULL)
    {
        return 0;
    }

    uint64_t offset = pos;
    if (fseek(f, offset++, SEEK_SET) != 0)
    {
        return 0;
    }

    uint64_t size = 0;

    char c = '\0';
    char prev_c = '\0';
    char is_in_array = 1;
    char is_in_dict = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    while ((c = fgetc(f)) != EOF)
    {
        if (ARRAY_END_REACHED)
        {
            break;
        }

        if (c == '\\')
        {
            is_backslashing = !is_backslashing;
        }

        // If we are not in a string or if the string just ended
        if (!is_in_string || (is_in_string && c == '"' && !is_backslashing))
        {
            if (c == '"')
            {
                is_in_string = !is_in_string;
            }
            else if (c == '[')
            {
                ++is_in_array;
                if (is_in_array == 2)
                {
                    ++size;
                }
            }
            else if (c == ']')
            {
                --is_in_array;
            }
            else if (c == '{')
            {
                ++is_in_dict;
                if (is_in_array == 2)
                {
                    ++size;
                }
            }
            else if (c == '}')
            {
                --is_in_dict;
            }
            else if (!is_in_dict && is_in_array == 1 && c == ',')
            {
                ++size;
            }
        }

        if (fseek(f, offset++, SEEK_SET) != 0)
        {
            break;
        }

        if (c != ' ' && c != '\t' && c != ']')
        {
            prev_c = c;
        }
    }
    // If there was only one value, there was no ',', so the element wasn't
    // detected
    if ((prev_c == '\n') && size == 0)
    {
        ++size;
    }
    return size < 2 ? size : size + 1;
}

JSONArray *parse_array(FILE *f, uint64_t *pos)
{
    if (f == NULL || pos == NULL)
    {
        return NULL;
    }

    uint64_t nb_elts = get_nb_elts_array(f, *pos);
    uint64_t nb_elts_parsed = 0;

    JSONArray *ja = new JSONArray(nb_elts);

    if (fseek(f, (*pos)++, SEEK_SET) != 0)
    {
        return NULL;
    }

    char c = '\0';
    while ((c = fgetc(f)) != EOF)
    {
        if (c == '"')
        {
            ja->add(new StringTypedValue(parse_string(f, pos)));
        }
        else if (IS_NUMBER_START(c))
        {
            ja->add(new IntTypedValue(parse_number(f, pos)));
        }
        else if (IS_BOOL_START(c))
        {
            uint64_t len = parse_boolean(f, pos);
            if (len == 0 || (c == 'f' && len != 5) || (c == 't' && len != 4))
            {
                continue;
            }
            ja->add(new BoolTypedValue(len == 4 ? true : false));
        }
        else if (c == 'n')
        {
            ja->add(new NullTypedValue());
        }
        else if (c == '[')
        {
            ja->add(new ArrayTypedValue(parse_array(f, pos)));
        }
        else if (c == '{')
        {
            ja->add(new DictTypedValue(parse_json_dict(f, pos)));
        }
        else if (c == ',')
        {
            ++nb_elts_parsed;
        }

        if (nb_elts_parsed >= nb_elts)
        {
            break;
        }

        if (fseek(f, (*pos)++, SEEK_SET) != 0)
        {
            break;
        }
    }
    --(*pos);
    return ja;
}

uint64_t get_nb_elts_dict(FILE *f, uint64_t pos)
{
    if (f == NULL)
    {
        return 0;
    }

    uint64_t offset = pos;
    if (fseek(f, offset++, SEEK_SET) != 0)
    {
        return 0;
    }

    uint64_t size = 0;
    // Used for the case where the dict contains only one element, and so does
    // not contain a ','
    uint64_t single_elt_found = 0;

    char c = '\0';
    char prev_c = '\0';
    char is_in_dict = 1;
    char is_in_array = 0;
    char is_in_string = 0;
    while ((c = fgetc(f)) != EOF)
    {
        if (DICT_END_REACHED)
        {
            break;
        }

        // If we are not in a string or if the string just ended
        if (!is_in_string || (is_in_string && c == '"' && prev_c != '\\'))
        {
            if (c == '"')
            {
                is_in_string = !is_in_string;
                single_elt_found = 1;
            }
            else if (c == '[')
            {
                ++is_in_array;
            }
            else if (c == ']')
            {
                --is_in_array;
            }
            else if (c == '{')
            {
                ++is_in_dict;
            }
            else if (c == '}')
            {
                --is_in_dict;
            }
            else if (!is_in_array && is_in_dict == 1 && c == ',')
            {
                ++size;
            }
        }

        if (fseek(f, offset++, SEEK_SET) != 0)
        {
            break;
        }
        prev_c = c;
    }
    return size == 0 ? single_elt_found : size + 1;
}

JSONDict *parse_json_dict(FILE *f, uint64_t *pos)
{
    if (f == NULL || pos == NULL)
    {
        return NULL;
    }

    string key = string();
    uint64_t nb_elts = get_nb_elts_dict(f, *pos);
    uint64_t nb_elts_parsed = 0;

    JSONDict *jd = new JSONDict(nb_elts);

    if (fseek(f, (*pos)++, SEEK_SET) != 0)
    {
        return NULL;
    }

    char c = '\0';
    char is_waiting_key = 1;
    while ((c = fgetc(f)) != EOF)
    {
        if (c == '"')
        {
            if (is_waiting_key)
            {
                key = parse_string(f, pos);
                is_waiting_key = 0;
            }
            else
            {
                jd->addItem(new StringItem(key, parse_string(f, pos)));
            }
        }
        else if (IS_NUMBER_START(c))
        {
            jd->addItem(new IntItem(key, parse_number(f, pos)));
        }
        else if (IS_BOOL_START(c))
        {
            uint64_t len = parse_boolean(f, pos);
            if (len == 0 || (c == 'f' && len != 5) || (c == 't' && len != 4))
            {
                continue;
            }
            jd->addItem(new BoolItem(key, len == 4 ? true : false));
        }
        else if (c == 'n')
        {
            jd->addItem(new NullItem(key));
        }
        else if (c == '[')
        {
            jd->addItem(new ArrayItem(key, parse_array(f, pos)));
        }
        else if (c == '{')
        {
            jd->addItem(new DictItem(key, parse_json_dict(f, pos)));
        }
        else if (c == ',')
        {
            is_waiting_key = 1;
            ++nb_elts_parsed;
        }

        if (nb_elts_parsed >= nb_elts)
        {
            break;
        }

        if (fseek(f, (*pos)++, SEEK_SET) != 0)
        {
            break;
        }
    }
    --(*pos);
    return jd;
}

/*******************************************************************************
**                                 FUNCTIONS                                  **
*******************************************************************************/
// TODO: Fix parsing of arrays and dicts of empty arrays and dicts
JSON *parse(char *file)
{
    FILE *f = fopen(file, "r");
    if (f == NULL)
    {
        return NULL;
    }

    uint64_t offset = 0;
    if (fseek(f, offset++, SEEK_SET) != 0)
    {
        fclose(f);
        return NULL;
    }

    char c = fgetc(f);
    if (c == '{')
    {
        JSONDict *jd = parse_json_dict(f, &offset);
        if (jd == NULL)
        {
            fclose(f);
            return NULL;
        }
        return jd;
    }
    else if (c == '[')
    {
        // TODO -> Fix parsing when dicts without keys are inside an array
        JSONArray *ja = parse_array(f, &offset);
        if (ja == NULL)
        {
            fclose(f);
            return NULL;
        }
        return ja;
    }

    fclose(f);
    return NULL;
}
