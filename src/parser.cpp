#include "parser.hpp"

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <cmath>
#include <cstring>
#include <stdio.h>
#include <sys/stat.h>

#include "json.hpp"

/*******************************************************************************
**                                   MACROS                                   **
*******************************************************************************/
#define IS_NUMBER_START(c) (('0' <= (c) && (c) <= '9') || (c) == '-')
#define IS_BOOL_START(c) ((c) == 't' || (c) == 'f')

#define IS_END_CHAR(c)                                                         \
    ((c) == 0 || (c) == ',' || (c) == '\n' || (c) == ']' || (c) == '}')

#define IS_STRING_END(c) ((c) == 0 || ((c) == '"' && prev_c != '\\'))

#define IS_NOT_BOOLEAN(c, l)                                                   \
    ((l) == 0 || ((c) == 'f' && (l) != 5) || ((c) == 't' && (l) != 4))

/**
** \def Used by the functions that read the file using fseef() and fgetc()
**      inside while loops, it sets the cursor on the 'p' position,
**      increments 'p' and puts in 'c' the current character
*/
#define SEEK_AND_GET_CHAR(p) (!fseek(f, p++, SEEK_SET) && (c = fgetc(f)) != EOF)

#ifndef MAX_READ_BUFF_SIZE
#    define MAX_READ_BUFF_SIZE (1 << 30) // ~= ~ 1 GB
#endif

#ifndef MAX_NESTED_ARRAYS
#    define MAX_NESTED_ARRAYS UINT_FAST8_MAX // 255
#endif

#ifndef MAX_NESTED_DICTS
#    define MAX_NESTED_DICTS UINT_FAST8_MAX // 255
#endif

#if MAX_NESTED_ARRAYS <= UINT_FAST8_MAX
typedef uint_fast8_t uint_nested_arrays_t;
#elif MAX_NESTED_ARRAYS <= UINT_FAST16_MAX
typedef uint_fast16_t uint_nested_arrays_t;
#elif MAX_NESTED_ARRAYS <= UINT_FAST32_MAX
typedef uint_fast32_t uint_nested_arrays_t;
#else
typedef uint_fast64_t uint_nested_arrays_t;
#endif

#if MAX_NESTED_DICTS <= UINT_FAST8_MAX
typedef uint_fast8_t uint_nested_dicts_t;
#elif MAX_NESTED_DICTS <= UINT_FAST16_MAX
typedef uint_fast16_t uint_nested_dicts_t;
#elif MAX_NESTED_DICTS <= UINT_FAST32_MAX
typedef uint_fast32_t uint_nested_dicts_t;
#else
typedef uint_fast64_t uint_nested_dicts_t;
#endif

/*******************************************************************************
**                                 STRUCTURES                                 **
*******************************************************************************/
/**
** \class StrAndLenTuple
** \brief Used by the parse_number() function to return multiple informations
*/
class StrAndLenTuple
{
public:
    char *str;
    uint_strlen_t len;
    bool is_float;
    bool has_exponent;

    StrAndLenTuple(char *str, uint_strlen_t len, bool is_float,
                   bool has_exponent)
        : str(str)
        , len(len)
        , is_float(is_float)
        , has_exponent(has_exponent)
    {}

    StrAndLenTuple()
        : str(nullptr)
        , len(0)
        , is_float(false)
        , has_exponent(false)
    {}

    ~StrAndLenTuple()
    {
        delete[] str;
        str = nullptr;
    }
};

/*******************************************************************************
**                           FUNCTIONS DECLARATIONS                           **
*******************************************************************************/
JSONDict *parse_dict_buff(char *b, uint_fast64_t *pos, uint_fast16_t *err);
JSONDict *parse_dict(FILE *f, uint_fast64_t *pos, uint_fast16_t *err);

/*******************************************************************************
**                              LOCAL FUNCTIONS                               **
*******************************************************************************/
/**
** \brief Takes sl's char array and transforms it into an int64_t.
**        If the number has an exponent, the exponent is parsed as well and the
**        number is elevated to that exponent
** \param sl A pointer to an StrAndLenTuple object
** \returns The 0 in case of error (or if the number was 0), the number
**          otherwise
*/
int_fast64_t str_to_long(StrAndLenTuple *sl)
{
    if (sl == nullptr)
    {
        return 0;
    }

    char *str = sl->str;
    uint_fast64_t len = sl->len;
    if (str == nullptr || len == 0)
    {
        return 0;
    }

    int_fast64_t res = 0;
    uint_fast64_t exponent = 0;
    char is_negative = str[0] == '-' ? -1 : 1;
    char is_in_exponent = 0;
    char c = 0;
    for (uint_fast64_t i = 0; i < len; ++i)
    {
        c = str[i];
        if (sl->has_exponent && (c == 'e' || c == 'E'))
        {
            is_in_exponent = 1;
        }
        else if ('0' <= c && c <= '9')
        {
            if (is_in_exponent)
            {
                exponent = exponent * 10 + c - '0';
            }
            else
            {
                res = res * 10 + c - '0';
            }
        }
    }
    return sl->has_exponent ? pow(res * is_negative, exponent)
                            : res * is_negative;
}

/**
** \brief Takes sl's char array and transforms it into a double.
**        If the number has an exponent, the exponent is parsed as well and the
**        number is elevated to that exponent
** \param sl A pointer to an StrAndLenTuple object
** \returns The 0 in case of error (or if the number was 0), the number
**          otherwise
*/
// FIX: Precision error
double str_to_double(StrAndLenTuple *sl)
{
    if (sl == nullptr)
    {
        return 0;
    }

    char *str = sl->str;
    uint_fast64_t len = sl->len;
    if (str == nullptr || len == 0)
    {
        return 0;
    }

    double res = 0; // Integer part
    double dot_res = 0; // Decimal part
    uint_fast64_t exponent = 0; // Only used if sl->has_exponent() is true
    uint_fast64_t nb_digits_dot = 1;
    // If the number is negative, this is set to -1 and the final res is
    // multiplied by it
    char is_negative = str[0] == '-' ? -1 : 1;
    char dot_reached = 0;
    char is_in_exponent = 0;
    char c = 0;
    for (uint_fast64_t i = 0; i < len; ++i)
    {
        c = str[i];
        if (c == '.')
        {
            dot_reached = 1;
        }
        else if (sl->has_exponent && (c == 'e' || c == 'E'))
        {
            is_in_exponent = 1;
        }
        else if ('0' <= c && c <= '9')
        {
            if (is_in_exponent)
            {
                exponent = exponent * 10 + c - '0';
            }
            else if (dot_reached)
            {
                dot_res = dot_res * 10 + c - '0';
                nb_digits_dot *= 10;
            }
            else
            {
                res = res * 10 + c - '0';
            }
        }
    }
    return sl->has_exponent
        ? pow(is_negative * (res + (dot_res / nb_digits_dot)), exponent)
        : is_negative * (res + (dot_res / nb_digits_dot));
}

bool is_float(char *str, uint_fast64_t len)
{
    if (str == nullptr)
    {
        return false;
    }

    for (uint_fast64_t i = 0; i < len; ++i)
    {
        if (str[i] == '.')
        {
            return true;
        }
    }
    return false;
}

bool has_exponent(char *str, uint_fast64_t len)
{
    if (str == nullptr)
    {
        return false;
    }

    for (uint_fast64_t i = 0; i < len; ++i)
    {
        if (str[i] == 'e' || str[i] == 'E')
        {
            return true;
        }
    }
    return false;
}

bool max_nested_arrays_reached(uint_nested_arrays_t is_in_array,
                               uint_fast16_t *err)
{
    if (err == nullptr)
    {
        return true;
    }

    if (is_in_array == MAX_NESTED_ARRAYS)
    {
#ifdef DEBUG
        printf("Max number of nested arrays reached, aborting "
               "parsing\n");
#endif
        *err = ERR_MAX_NESTED_ARRAYS_REACHED;
        return true;
    }
    return false;
}

bool max_nested_dicts_reached(uint_nested_dicts_t is_in_dict,
                              uint_fast16_t *err)
{
    if (err == nullptr)
    {
        return true;
    }

    if (is_in_dict == MAX_NESTED_DICTS)
    {
#ifdef DEBUG
        printf("Max number of nested dicts reached, aborting "
               "parsing\n");
#endif
        *err = ERR_MAX_NESTED_DICTS_REACHED;
        return true;
    }
    return false;
}

/**
** \brief Parses the string starting at 'pos + 1' (first char after the '"')
** \param buff The buffer containing the current json file or object
** \param idx A pointer to the uint_fast64_t containing the index of the '"'
**            that started the string we want to parse
** \returns An empty string in case of error, the parsed string otherwise
*/
String *parse_string_buff(char *buff, uint_fast64_t *idx)
{
    if (buff == nullptr || idx == nullptr)
    {
        return nullptr;
    }

    uint_fast64_t start_idx = *idx + 1;
    uint_fast64_t len = 0;
    char c = 0;
    char prev_c = 0;
    // Counts the number of characters until the first one that is an 'end char'
    while (1)
    {
        c = buff[start_idx + len];
        if (IS_STRING_END(c))
        {
            break;
        }
        prev_c = c;
        ++len;
    }

    if (len == 0)
    {
        return nullptr;
    }

    char *str = new char[len + 1]();
    if (str == nullptr)
    {
        return nullptr;
    }
    std::memcpy(str, buff + start_idx, len);

    // + 1 to not read the last '"' when returning in the calling function
    *idx += len + 1;
    return new String(str, len);
}

/**
** \brief Reads the buffer from the given pos - 1
** \param buff The buffer containing the current json file or object
** \param idx The index of the second character of the number (the first one
**            was already read)
** \returns An instance of the StrAndLenTuple class containing the number as a
**          char array, the length of the char array and whether the number is a
**          float and has an exponent
*/
StrAndLenTuple parse_number_buff(char *buff, uint_fast64_t *idx)
{
    if (buff == nullptr || idx == nullptr)
    {
        return StrAndLenTuple();
    }

    // Counts the number of characters until the first one that is an 'end char'
    uint_fast64_t end_idx = *idx;
    uint_fast64_t initial_i = end_idx;
    char c = 0;
    while (1)
    {
        c = buff[end_idx];
        if (IS_END_CHAR(c))
        {
            break;
        }
        ++end_idx;
    }

    // Number of chars
    uint_fast64_t len = end_idx - initial_i;
    if (len == 0)
    {
        return StrAndLenTuple();
    }

    // Puts the value in the form of a char array
    char *str = new char[len + 1]();
    if (str == nullptr)
    {
        return StrAndLenTuple();
    }
    std::memcpy(str, buff + initial_i, len);

    *idx += len - 1;
    return StrAndLenTuple(str, len, is_float(str, len), has_exponent(str, len));
}

/**
** \returns 5 if false, 4 if true, 0 otherwise
**/
uint_fast64_t parse_boolean_buff(char *buff, uint_fast64_t *idx)
{
    if (buff == nullptr || idx == nullptr)
    {
        return 0;
    }

    uint_fast64_t end_idx = *idx;
    char c = 0;
    while (1)
    {
        c = buff[end_idx];
        if (IS_END_CHAR(c))
        {
            break;
        }
        ++end_idx;
    }
    uint_fast64_t len = end_idx - *idx;
    *idx += len - 1;
    return len;
}

/**
** \param buff The buffer containing the object currently being parsed
** \param idx The index of the character just after the '[' that begins the
**            current array
** \returns The number of elements of the current array
*/
uint_fast64_t get_nb_elts_array_buff(char *buff, uint_fast64_t idx,
                                     uint_fast16_t *err)
{
    if (buff == nullptr || err == nullptr || buff[idx] == ']')
    {
        return 0;
    }

    uint_fast64_t nb_elts = 0;
    uint_nested_arrays_t is_in_array = 1;
    uint_nested_dicts_t is_in_dict = 0;
    char c = 0;
    char prev_c = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    char comma_encountered = 0;
    while (1)
    {
        c = buff[idx];
        if (!is_in_array || c == 0)
        {
            break;
        }

        if (c == '\\')
        {
            is_backslashing = !is_backslashing;
        }
        else if (!comma_encountered && c == ',' && is_in_array == 1)
        {
            comma_encountered = 1;
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
                if (max_nested_arrays_reached(is_in_array, err))
                {
                    return 0;
                }
                ++is_in_array;
            }
            else if (c == ']')
            {
                --is_in_array;
            }
            else if (c == '{')
            {
                if (max_nested_dicts_reached(is_in_dict, err))
                {
                    return 0;
                }
                ++is_in_dict;
            }
            else if (c == '}')
            {
                --is_in_dict;
            }
            else if (!is_in_dict && is_in_array == 1 && c == ',')
            {
                ++nb_elts;
            }
        }
        ++idx;

        if (c != ' ' && c != '\t' && c != '\n')
        {
            prev_c = c;
        }
    }

    // If there was only one value, there was no ',', so the element wasn't
    // detected or if at least one element was found, it means that a ',' was
    // found
    if ((nb_elts == 0 && prev_c != 0) || (nb_elts >= 1 && comma_encountered))
    {
        ++nb_elts;
    }
    return nb_elts;
}

/**
** \param buff The buffer containing the object currently being parsed
** \param idx The index of the character just after the '{' that begins the
**            current dict
** \returns The number of elements of the current dict
*/
uint_fast64_t get_nb_elts_dict_buff(char *buff, uint_fast64_t idx,
                                    uint_fast16_t *err)
{
    if (buff == nullptr || err == nullptr || idx >= MAX_READ_BUFF_SIZE
        || buff[idx] == '}')
    {
        return 0;
    }

    uint_fast64_t nb_elts = 0;
    // Used for the case where the dict contains only one element, and so does
    // not contain a ','
    uint_fast64_t single_elt_found = 0;

    uint_nested_dicts_t is_in_dict = 1;
    uint_nested_arrays_t is_in_array = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    char c = 0;
    while (1)
    {
        c = buff[idx];
        if (!is_in_dict || c == 0)
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
                single_elt_found = 1;
            }
            else if (c == '[')
            {
                if (max_nested_arrays_reached(is_in_array, err))
                {
                    return 0;
                }
                ++is_in_array;
            }
            else if (c == ']')
            {
                --is_in_array;
            }
            else if (c == '{')
            {
                if (max_nested_dicts_reached(is_in_dict, err))
                {
                    return 0;
                }
                ++is_in_dict;
            }
            else if (c == '}')
            {
                --is_in_dict;
            }
            else if (!is_in_array && is_in_dict == 1 && c == ',')
            {
                ++nb_elts;
            }
        }
        ++idx;
    }
    return nb_elts == 0 ? single_elt_found : nb_elts + 1;
}

/**
** \param buff The buffer containing the object currently being parsed
** \param idx The index of the character '[' that begins the current array
** \returns The json array parsed from the position
*/
JSONArray *parse_array_buff(char *b, uint_fast64_t *idx, uint_fast16_t *err)
{
    if (b == nullptr || err == nullptr)
    {
        return nullptr;
    }

    uint_fast64_t i = idx == nullptr ? 0 : *idx + 1;

    uint_fast64_t nb_elts_parsed = 0;
    uint_fast64_t nb_elts = get_nb_elts_array_buff(b, i, err);
    if (*err)
    {
        return nullptr;
    }

    JSONArray *ja = new JSONArray();
    if (nb_elts == 0)
    {
        return ja;
    }

    char c = 0;
    // We start at 1 because if we entered this function, it means that we
    // already read a '['
    uint_fast64_t initial_i = i;
    while (1)
    {
        c = b[i];
        if (c == 0 || nb_elts_parsed >= nb_elts)
        {
            break;
        }

        if (c == '"')
        {
            ja->addValue(new StringValue(parse_string_buff(b, &i)));
            ++nb_elts_parsed;
        }
        else if (IS_NUMBER_START(c))
        {
            StrAndLenTuple sl = parse_number_buff(b, &i);
            if (sl.str == nullptr)
            {
                continue;
            }

            if (sl.is_float)
            {
                ja->addValue(new DoubleValue(str_to_double(&sl)));
            }
            else
            {
                ja->addValue(new IntValue(str_to_long(&sl)));
            }
            ++nb_elts_parsed;
        }
        else if (IS_BOOL_START(c))
        {
            uint_fast64_t len = parse_boolean_buff(b, &i);
            if (IS_NOT_BOOLEAN(c, len))
            {
                continue;
            }
            ja->addValue(new BoolValue(len == 4 ? true : false));
            ++nb_elts_parsed;
        }
        else if (c == 'n')
        {
            ja->addValue(new NullValue());
            i += 3;
            ++nb_elts_parsed;
        }
        else if (c == '[')
        {
            JSONArray *tmp_ja = parse_array_buff(b, &i, err);
            if (tmp_ja == nullptr)
            {
                *err = 1;
                break;
            }
            ja->addValue(new ArrayValue(tmp_ja));
            ++nb_elts_parsed;
        }
        else if (c == '{')
        {
            JSONDict *tmp_jd = parse_dict_buff(b, &i, err);
            if (tmp_jd == nullptr)
            {
                *err = 1;
                break;
            }
            ja->addValue(new DictValue(tmp_jd));
            ++nb_elts_parsed;
        }
        ++i;
    }
    if (*err)
    {
        delete ja;
        return nullptr;
    }
    if (idx != nullptr)
    {
        *idx += i - initial_i;
    }
    return ja;
}

/**
** \param b The buffer containing the object currently being parsed
** \param idx A pointer to the index of the character '{' that begins the
**            current dict.
**            If the given pointer is a nullptr, it means that the buffer is a
**            new one created just before calling this function, meaning the
**            index starts at 0
** \returns The json dict parsed from the index
*/
JSONDict *parse_dict_buff(char *b, uint_fast64_t *idx, uint_fast16_t *err)
{
    if (b == nullptr || err == nullptr)
    {
        return nullptr;
    }

    uint_fast64_t i = idx == nullptr ? 0 : *idx + 1;

    uint_fast64_t nb_elts_parsed = 0;
    uint_fast64_t nb_elts = get_nb_elts_dict_buff(b, i, err);
    if (*err)
    {
        return nullptr;
    }

    JSONDict *jd = new JSONDict();
    if (nb_elts == 0)
    {
        return jd;
    }

    String *key = nullptr;
    char c = 0;
    char is_waiting_key = 1;
    // We start at 1 because if we entered this function, it means that we
    // already read a '{'
    uint_fast64_t initial_i = i;
    while (1)
    {
        c = b[i];
        if (c == 0 || nb_elts_parsed >= nb_elts)
        {
            break;
        }

        if (c == '"')
        {
            if (is_waiting_key)
            {
                key = parse_string_buff(b, &i);
                is_waiting_key = 0;
            }
            else
            {
                jd->addItem(new StringItem(key, parse_string_buff(b, &i)));
                ++nb_elts_parsed;
            }
        }
        else if (IS_NUMBER_START(c))
        {
            StrAndLenTuple sl = parse_number_buff(b, &i);
            if (sl.str == nullptr)
            {
                continue;
            }

            if (sl.is_float)
            {
                jd->addItem(new DoubleItem(key, str_to_double(&sl)));
            }
            else
            {
                jd->addItem(new IntItem(key, str_to_long(&sl)));
            }
            ++nb_elts_parsed;
        }
        else if (IS_BOOL_START(c))
        {
            uint_fast64_t len = parse_boolean_buff(b, &i);
            if (IS_NOT_BOOLEAN(c, len))
            {
                continue;
            }
            jd->addItem(new BoolItem(key, len == 4 ? true : false));
            ++nb_elts_parsed;
        }
        else if (c == 'n')
        {
            jd->addItem(new NullItem(key));
            i += 3;
            ++nb_elts_parsed;
        }
        else if (c == '[')
        {
            JSONArray *tmp_ja = parse_array_buff(b, &i, err);
            if (tmp_ja == nullptr)
            {
                *err = 1;
                break;
            }
            jd->addItem(new ArrayItem(key, tmp_ja));
            ++nb_elts_parsed;
        }
        else if (c == '{')
        {
            JSONDict *tmp_jd = parse_dict_buff(b, &i, err);
            if (tmp_jd == nullptr)
            {
                *err = 1;
                break;
            }
            jd->addItem(new DictItem(key, tmp_jd));
            ++nb_elts_parsed;
        }
        else if (c == ',')
        {
            is_waiting_key = 1;
        }
        ++i;
    }
    if (*err)
    {
        delete jd;
        return nullptr;
    }
    if (idx != nullptr)
    {
        *idx += i - initial_i;
    }
    return jd;
}

/**
** \brief Parses the string starting at 'pos + 1' (first char after the '"')
** \param f The file stream
** \param pos A pointer to the uint_fast64_t containing the position of the '"'
**            that started the string we want to parse
** \returns An empty string in case of error, the parsed string otherwise
*/
String *parse_string(FILE *f, uint_fast64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return nullptr;
    }

    uint_fast64_t i = *pos;
    char c = 0;
    char prev_c = 0;
    while (SEEK_AND_GET_CHAR(i) && !IS_STRING_END(c))
    {
        prev_c = c;
    }

    uint_strlen_t len = i - *pos - 1 > MAX_STR_LEN ? 0 : i - *pos - 1;
    if (len == 0)
    {
        return nullptr;
    }

    char *str = new char[len + 1]();
    if (str == nullptr)
    {
        return nullptr;
    }

    if (fseek(f, *pos, SEEK_SET) != 0)
    {
        delete[] str;
        return nullptr;
    }
    fread(str, sizeof(char), len, f);

    // +1 to not read the last '"' when returning in the calling function
    *pos += len + 1;
    return new String(str, len);
}

/**
** \brief Reads the file from the given pos and returns an instance of the
**        StrAndLenTuple class containing the number as a char array, the length
**        of the char array and whether the number is a float and has an
**        exponent
** \param f The file stream
** \param pos The pos of the start of the number of which we are currently
**            acquiring the length
*/
StrAndLenTuple parse_number(FILE *f, uint_fast64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return StrAndLenTuple();
    }

    // Obtains the length of the value
    // -1 because we already read the first digit (or sign)
    uint_fast64_t end_pos = *pos - 1;

    char c = 0;
    // end_pos is incremented for each character found to be part of a number
    while (SEEK_AND_GET_CHAR(end_pos) && !IS_END_CHAR(c))
    {
    }

    uint_fast64_t len = end_pos - *pos;
    if (len == 0)
    {
        return StrAndLenTuple();
    }

    // Puts the value in the form of a char array
    char *str = new char[len + 1]();
    // If the allocation failed
    if (str == nullptr)
    {
        return StrAndLenTuple();
    }

    // If we couldn't set the pos in the file
    if (fseek(f, *pos - 1, SEEK_SET) != 0)
    {
        delete[] str;
        return StrAndLenTuple();
    }
    fread(str, sizeof(char), len, f);

    *pos += len - 1;
    return StrAndLenTuple(str, len, is_float(str, len), has_exponent(str, len));
}

/**
** \returns 5 if false, 4 if true, 0 otherwise
**/
uint_fast64_t parse_boolean(FILE *f, uint_fast64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return 0;
    }

    // -1 because we already read the first character
    uint_fast64_t end_pos = *pos - 1;

    char c = 0;
    // end_pos is incremented for each character found to be part of a boolean
    while (SEEK_AND_GET_CHAR(end_pos) && !IS_END_CHAR(c))
    {
    }
    uint_fast64_t len = end_pos - *pos;
    *pos += len - 1;
    return len;
}

/**
** \param f The file stream
** \param pos The position in the file of the character after the '[' that
**            begins the current array
** \param err A pointer to a char that will be updated if an error occurs
**            (we return the number of chars, so we can't use a number to
**            indicate an error)
** \returns The total number of characters in the current array - 1 (the first
**          '[' is not counted)
*/
uint_fast64_t get_nb_chars_in_array(FILE *f, uint_fast64_t pos,
                                    uint_fast16_t *err)
{
    if (f == nullptr || err == nullptr)
    {
        return 0;
    }

    uint_fast64_t nb_chars = 0;

    uint_nested_arrays_t is_in_array = 1;
    uint_nested_dicts_t is_in_dict = 0;
    char is_in_string = 0;
    char is_backslashing = 0;

    char c = 0;
    while (SEEK_AND_GET_CHAR(pos))
    {
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
                if (max_nested_arrays_reached(is_in_array, err))
                {
                    return 0;
                }
                ++is_in_array;
            }
            else if (c == ']')
            {
                --is_in_array;
            }
            else if (c == '{')
            {
                if (max_nested_dicts_reached(is_in_dict, err))
                {
                    return 0;
                }
                ++is_in_dict;
            }
            else if (c == '}')
            {
                --is_in_dict;
            }
        }
        ++nb_chars;

        if (!is_in_array)
        {
            break;
        }
    }
    return nb_chars;
}

/**
** \param f The file stream
** \param pos The position in the file of the character just after the '[' that
**            begins the current array
** \returns The number of elements of the current array
*/
uint_fast64_t get_nb_elts_array(FILE *f, uint_fast64_t pos, uint_fast16_t *err)
{
    if (f == nullptr || err == nullptr)
    {
        return 0;
    }

    uint_fast64_t nb_elts = 0;

    uint_nested_arrays_t is_in_array = 1;
    uint_nested_dicts_t is_in_dict = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    char comma_encountered = 0;

    char c = 0;
    char prev_c = 0;
    while (SEEK_AND_GET_CHAR(pos))
    {
        if (!is_in_array)
        {
            break;
        }

        if (c == '\\')
        {
            is_backslashing = !is_backslashing;
        }
        else if (!comma_encountered && c == ',' && is_in_array == 1)
        {
            comma_encountered = 1;
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
                if (max_nested_arrays_reached(is_in_array, err))
                {
                    return 0;
                }
                ++is_in_array;
            }
            else if (c == ']')
            {
                // Empty array
                // TODO: Remove this and put it at the top like in the buffered
                // version
                if (is_in_array == 1 && prev_c == 0)
                {
                    return 0;
                }
                --is_in_array;
            }
            else if (c == '{')
            {
                if (max_nested_dicts_reached(is_in_dict, err))
                {
                    return 0;
                }
                ++is_in_dict;
            }
            else if (c == '}')
            {
                --is_in_dict;
            }
            else if (!is_in_dict && is_in_array == 1 && c == ',')
            {
                ++nb_elts;
            }
        }

        if (c != ' ' && c != '\t' && c != '\n')
        {
            prev_c = c;
        }
    }

    // If there was only one value, there was no ',', so the element wasn't
    // detected or if at least one element was found, it means that a ',' was
    // found
    if ((nb_elts == 0 && prev_c != 0) || (nb_elts >= 1 && comma_encountered))
    {
        ++nb_elts;
    }
    return nb_elts;
}

/**
** \param f The file stream
** \param pos The position in the file of the character after the '{' that
**            begins the current dict
** \returns The total number of characters in the current dict - 1 (the first
**          '{' is not counted)
*/
uint_fast64_t get_nb_chars_in_dict(FILE *f, uint_fast64_t pos,
                                   uint_fast16_t *err)
{
    if (f == nullptr || err == nullptr)
    {
        return 0;
    }

    uint_fast64_t nb_chars = 0;

    uint_nested_dicts_t is_in_dict = 1;
    uint_nested_arrays_t is_in_array = 0;
    char is_in_string = 0;
    char is_backslashing = 0;

    char c = 0;
    while (SEEK_AND_GET_CHAR(pos))
    {
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
                if (max_nested_arrays_reached(is_in_array, err))
                {
                    return 0;
                }
                ++is_in_array;
            }
            else if (c == ']')
            {
                --is_in_array;
            }
            else if (c == '{')
            {
                if (max_nested_dicts_reached(is_in_dict, err))
                {
                    return 0;
                }
                ++is_in_dict;
            }
            else if (c == '}')
            {
                --is_in_dict;
            }
        }
        ++nb_chars;

        if (!is_in_dict)
        {
            break;
        }
    }
    return nb_chars;
}

/**
** \param f The file stream
** \param pos The position in the file of the character just after the '{' that
**            begins the current dict
** \returns The number of elements of the current dict
*/
uint_fast64_t get_nb_elts_dict(FILE *f, uint_fast64_t pos, uint_fast16_t *err)
{
    if (f == nullptr || err == nullptr)
    {
        return 0;
    }

    uint_fast64_t nb_elts = 0;

    uint_nested_dicts_t is_in_dict = 1;
    uint_nested_arrays_t is_in_array = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    // Used for the case where the dict contains only one element, and so does
    // not contain a ','
    char single_elt_found = 0;

    char c = 0;
    while (SEEK_AND_GET_CHAR(pos))
    {
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
                single_elt_found = 1;
            }
            else if (c == '[')
            {
                if (max_nested_arrays_reached(is_in_array, err))
                {
                    return 0;
                }
                ++is_in_array;
            }
            else if (c == ']')
            {
                --is_in_array;
            }
            else if (c == '{')
            {
                if (max_nested_dicts_reached(is_in_dict, err))
                {
                    return 0;
                }
                ++is_in_dict;
            }
            else if (c == '}')
            {
                --is_in_dict;
            }
            else if (!is_in_array && is_in_dict == 1 && c == ',')
            {
                ++nb_elts;
            }
        }

        if (!is_in_dict)
        {
            break;
        }
    }
    return nb_elts == 0 ? single_elt_found : nb_elts + 1;
}

/**
** \param f The file stream
** \param pos The postion in the file of the character after the '[' that begins
**            the current array
** \returns The json array parsed from the position
*/
JSONArray *parse_array(FILE *f, uint_fast64_t *pos, uint_fast16_t *err)
{
    if (f == nullptr || pos == nullptr || err == nullptr)
    {
        return nullptr;
    }

    uint_fast64_t i = *pos;

    uint_fast64_t nb_elts_parsed = 0;
    uint_fast64_t nb_elts = get_nb_elts_array(f, i, err);
    if (*err)
    {
        return nullptr;
    }

    JSONArray *ja = new JSONArray();
    if (nb_elts == 0)
    {
        ++(*pos);
        return ja;
    }

    char c = 0;
    while (SEEK_AND_GET_CHAR(i) && nb_elts_parsed < nb_elts && c != 0)
    {
        // If we are not in a string or if the string just ended
        if (c == '"')
        {
            *err |= ja->addValue(new StringValue(parse_string(f, &i)));
            ++nb_elts_parsed;
        }
        else if (IS_NUMBER_START(c))
        {
            StrAndLenTuple sl = parse_number(f, &i);
            if (sl.str == nullptr)
            {
                continue;
            }

            if (sl.is_float)
            {
                *err |= ja->addValue(new DoubleValue(str_to_double(&sl)));
            }
            else
            {
                *err |= ja->addValue(new IntValue(str_to_long(&sl)));
            }
            ++nb_elts_parsed;
        }
        else if (IS_BOOL_START(c))
        {
            uint_fast64_t len = parse_boolean(f, &i);
            if (IS_NOT_BOOLEAN(c, len))
            {
                continue;
            }
            *err |= ja->addValue(new BoolValue(len == 4 ? true : false));
            ++nb_elts_parsed;
        }
        else if (c == 'n')
        {
            *err |= ja->addValue(new NullValue());
            i += 3;
            ++nb_elts_parsed;
        }
        else if (c == '[')
        {
            uint_fast64_t nb_chars = get_nb_chars_in_array(f, i, err);
            if (*err)
            {
                break;
            }

            JSONArray *tmp_ja = nullptr;
            // If there is enough space, we fill a buffer to read from it
            if (nb_chars < MAX_READ_BUFF_SIZE)
            {
                char *b = new char[nb_chars + 1]();
                if (fseek(f, i, SEEK_SET) != 0)
                {
                    *err = 1;
                    delete[] b;
                    break;
                }
                fread(b, sizeof(char), nb_chars, f);
                tmp_ja = parse_array_buff(b, nullptr, err);
                delete[] b;
                i += nb_chars;
            }
            else
            {
                tmp_ja = parse_array(f, &i, err);
            }

            *err |= ja->addValue(new ArrayValue(tmp_ja));
            ++nb_elts_parsed;
        }
        else if (c == '{')
        {
            uint_fast64_t nb_chars = get_nb_chars_in_dict(f, i, err);
            if (*err)
            {
                break;
            }

            JSONDict *tmp_jd = nullptr;
            // If there is enough space, we fill a buffer to read from it
            if (nb_chars < MAX_READ_BUFF_SIZE)
            {
                char *b = new char[nb_chars + 1]();
                if (fseek(f, i, SEEK_SET) != 0)
                {
                    *err = 1;
                    delete[] b;
                    break;
                }
                fread(b, sizeof(char), nb_chars, f);
                tmp_jd = parse_dict_buff(b, nullptr, err);
                delete[] b;
                i += nb_chars;
            }
            else
            {
                tmp_jd = parse_dict(f, &i, err);
            }

            *err |= ja->addValue(new DictValue(tmp_jd));
            ++nb_elts_parsed;
        }

        if (*err)
        {
            break;
        }
    }
    if (*err)
    {
        print_err_bits(*err);
        delete ja;
        return nullptr;
    }
    *pos = i - 1;
    return ja;
}

/**
** \param f The file stream
** \param pos A pointer to the position in the file of the character after the
**            '{' that begins the current dict
** \returns The json dict parsed from the position
*/
JSONDict *parse_dict(FILE *f, uint_fast64_t *pos, uint_fast16_t *err)
{
    if (f == nullptr || pos == nullptr || err == nullptr)
    {
        return nullptr;
    }

    uint_fast64_t i = *pos;

    uint_fast64_t nb_elts_parsed = 0;
    uint_fast64_t nb_elts = get_nb_elts_dict(f, i, err);
    if (*err)
    {
        return nullptr;
    }

    JSONDict *jd = new JSONDict();
    if (nb_elts == 0)
    {
        ++(*pos);
        return jd;
    }

    String *key = nullptr;
    char c = 0;
    char is_waiting_key = 1;
    // We start at 1 because if we entered this function, it means that we
    // already read a '['
    while (SEEK_AND_GET_CHAR(i) && nb_elts_parsed < nb_elts && c != 0)
    {
        if (c == '"')
        {
            if (is_waiting_key)
            {
                key = parse_string(f, &i);
                is_waiting_key = 0;
            }
            else
            {
                *err |= jd->addItem(new StringItem(key, parse_string(f, &i)));
                ++nb_elts_parsed;
            }
        }
        else if (IS_NUMBER_START(c))
        {
            StrAndLenTuple sl = parse_number(f, &i);
            if (sl.str == nullptr)
            {
                continue;
            }

            if (sl.is_float)
            {
                *err |= jd->addItem(new DoubleItem(key, str_to_double(&sl)));
            }
            else
            {
                *err |= jd->addItem(new IntItem(key, str_to_long(&sl)));
            }
            ++nb_elts_parsed;
        }
        else if (IS_BOOL_START(c))
        {
            uint_fast64_t len = parse_boolean(f, &i);
            if (IS_NOT_BOOLEAN(c, len))
            {
                continue;
            }
            *err |= jd->addItem(new BoolItem(key, len == 4 ? true : false));
            ++nb_elts_parsed;
        }
        else if (c == 'n')
        {
            *err |= jd->addItem(new NullItem(key));
            i += 3;
            ++nb_elts_parsed;
        }
        else if (c == '[')
        {
            uint_fast64_t nb_chars = get_nb_chars_in_array(f, i, err);
            if (*err)
            {
                break;
            }

            JSONArray *tmp_ja = nullptr;
            // If there is enough space, we fill a buffer to read from it
            if (nb_chars < MAX_READ_BUFF_SIZE)
            {
                char *b = new char[nb_chars + 1]();
                if (fseek(f, i, SEEK_SET) != 0)
                {
                    *err = ERR_FSEEK;
                    delete[] b;
                    break;
                }
                fread(b, sizeof(char), nb_chars, f);
                tmp_ja = parse_array_buff(b, nullptr, err);
                delete[] b;
                i += nb_chars;
            }
            else
            {
                tmp_ja = parse_array(f, &i, err);
            }

            *err |= jd->addItem(new ArrayItem(key, tmp_ja));
            ++nb_elts_parsed;
        }
        else if (c == '{')
        {
            uint_fast64_t nb_chars = get_nb_chars_in_dict(f, i, err);
            if (*err)
            {
                break;
            }

            JSONDict *tmp_jd = nullptr;
            // If there is enough space, we fill a buffer to read from it
            if (nb_chars < MAX_READ_BUFF_SIZE)
            {
                char *b = new char[nb_chars + 1]();
                if (fseek(f, i, SEEK_SET) != 0)
                {
                    *err = ERR_FSEEK;
                    delete[] b;
                    break;
                }
                fread(b, sizeof(char), nb_chars, f);
                tmp_jd = parse_dict_buff(b, nullptr, err);
                delete[] b;
                i += nb_chars;
            }
            else
            {
                tmp_jd = parse_dict(f, &i, err);
            }

            *err |= jd->addItem(new DictItem(key, tmp_jd));
            ++nb_elts_parsed;
        }
        else if (c == ',')
        {
            is_waiting_key = 1;
        }

        if (*err)
        {
            break;
        }
    }
    if (*err)
    {
        print_err_bits(*err);
        if (*err != ERR_NULL_KEY)
        {
            delete key;
        }
        delete jd;
        return nullptr;
    }
    *pos = i - 1;
    return jd;
}

/*******************************************************************************
**                                 FUNCTIONS                                  **
*******************************************************************************/
JSON *parse(char *file)
{
    FILE *f = fopen(file, "r");
    if (f == nullptr)
    {
        return nullptr;
    }

    uint_fast64_t offset = 0;
    if (fseek(f, offset++, SEEK_SET) != 0)
    {
        fclose(f);
        return nullptr;
    }

    // Obtains the number of characters in the file
    struct stat st;
    stat(file, &st);
    uint_fast64_t nb_chars = st.st_size;

    uint_fast16_t err = 0;
    char c = fgetc(f);
    if (c == '{')
    {
        JSONDict *jd = nullptr;
        if (nb_chars < MAX_READ_BUFF_SIZE)
        {
            char *b = new char[nb_chars + 1]();
            if (fseek(f, offset, SEEK_SET) != 0)
            {
                fclose(f);
                delete[] b;
                return nullptr;
            }
            fread(b, sizeof(char), nb_chars, f);
            jd = parse_dict_buff(b, nullptr, &err);
            delete[] b;
        }
        else
        {
            jd = parse_dict(f, &offset, &err);
        }
        fclose(f);
        return jd;
    }
    else if (c == '[')
    {
        JSONArray *ja = nullptr;
        if (nb_chars < MAX_READ_BUFF_SIZE)
        {
            char *b = new char[nb_chars + 1]();
            if (fseek(f, offset, SEEK_SET) != 0)
            {
                fclose(f);
                delete[] b;
                return nullptr;
            }
            fread(b, sizeof(char), nb_chars, f);
            ja = parse_array_buff(b, nullptr, &err);
            delete[] b;
        }
        else
        {
            ja = parse_array(f, &offset, &err);
        }
        fclose(f);
        return ja;
    }
    fclose(f);
    return nullptr;
}
