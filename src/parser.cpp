#include "parser.hpp"

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <cmath>
#include <string>

#include "json.hpp"

using namespace std;

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
    uint64_t len;
    bool is_float;
    bool has_exponent;

    StrAndLenTuple(char *str, uint64_t len, bool is_float, bool has_exponent)
        : str(str)
        , len(len)
        , is_float(is_float)
        , has_exponent(has_exponent)
    {}

    ~StrAndLenTuple()
    {
        delete[] str;
    }
};

/*******************************************************************************
**                                   MACROS                                   **
*******************************************************************************/
#define IS_NUMBER_START(c) (('0' <= (c) && (c) <= '9') || (c) == '-')
#define IS_BOOL_START(c) ((c) == 't' || (c) == 'f')

#define IS_END_CHAR(c) ((c) == ',' || (c) == '\n' || (c) == ']' || (c) == '}')
#define IS_STRING_END(c) ((c) == 0 || ((c) == '"' && prev_c != '\\'))

#define IS_NOT_BOOLEAN(c, l)                                                   \
    ((l) == 0 || ((c) == 'f' && (l) != 5) || ((c) == 't' && (l) != 4))

#ifndef READ_BUFF_MAX_SIZE_OVERRIDE
#    define READ_BUFF_MAX_SIZE 1024
#endif

#ifndef MAX_NESTED_ARRAYS
#    define MAX_NESTED_ARRAYS 255
#endif

#if MAX_NESTED_ARRAYS <= 255
typedef uint8_t nested_arrays_t;
#elif MAX_NESTED_ARRAYS <= 65536
typedef uint16_t nested_arrays_t;
#elif MAX_NESTED_ARRAYS <= 4294967296
typedef uint32_t nested_arrays_t;
#else
typedef uint64_t nested_arrays_t;
#endif

#if MAX_NESTED_DICTS <= 256
typedef uint8_t nested_dicts_t;
#elif MAX_NESTED_DICTS <= 65536
typedef uint16_t nested_dicts_t;
#elif MAX_NESTED_DICTS <= 4294967296
typedef uint32_t nested_dicts_t;
#else
typedef uint64_t nested_dicts_t;
#endif

/*******************************************************************************
**                           FUNCTIONS DECLARATIONS                           **
*******************************************************************************/
uint64_t get_nb_chars_in_dict(FILE *f, uint64_t pos);
JSONDict *parse_dict_buff(char b[READ_BUFF_MAX_SIZE], uint64_t *pos);
JSONDict *parse_dict(FILE *f, uint64_t *pos);

/*******************************************************************************
**                              LOCAL FUNCTIONS                               **
*******************************************************************************/
/**
** \brief Parses the string starting at 'pos + 1' (first char after the '"')
** \param buff The buffer containing the current json file or object
** \param idx A pointer to the uint64_t containing the index of the '"' that
**            started the string we want to parse
** \returns An empty string in case of error, the parsed string otherwise
*/
string parse_string_buff(char buff[READ_BUFF_MAX_SIZE], uint64_t *idx)
{
    if (idx == nullptr)
    {
        return string();
    }

    // Counts the number of characters until the first one that is an 'end char'
    uint64_t tmp_idx = *idx + 1;
    uint64_t initial_i = tmp_idx;
    char c = 0;
    char prev_c = 0;
    for (; tmp_idx < READ_BUFF_MAX_SIZE; ++tmp_idx)
    {
        c = buff[tmp_idx];
        if (IS_STRING_END(c))
        {
            break;
        }
        prev_c = c;
    }

    // Number of chars
    uint64_t len = tmp_idx - initial_i;
    if (len == 0)
    {
        return string();
    }

    char *str = new char[len + 1]();
    if (str == nullptr)
    {
        return string();
    }

    for (uint64_t i = 0; i < len; ++i)
    {
        str[i] = buff[i + initial_i];
    }

    // + 1 to not read the last '"' when returning in the calling function
    (*idx) += len + 1;
    string fstr(str);
    delete[] str;
    return fstr;
}

/**
** \brief Parses the string starting at 'pos + 1' (first char after the '"')
** \param f The file stream
** \param pos A pointer to the uint64_t containing the position of the '"' that
**            started the string we want to parse
** \returns An empty string in case of error, the parsed string otherwise
*/
string parse_string(FILE *f, uint64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return string();
    }

    // TODO: Test if + 1 is needed
    uint64_t i = *pos;
    char c = 0;
    char prev_c = 0;
    while ((c = fgetc(f)) != EOF)
    {
        if (IS_STRING_END(c))
        {
            break;
        }
        if (fseek(f, i++, SEEK_SET) != 0)
        {
            break;
        }
        prev_c = c;
    }

    uint64_t len = i - *pos - 1;
    if (len == 0)
    {
        return string();
    }

    char *str = new char[len + 1]();
    if (str == nullptr)
    {
        return string();
    }

    if (fseek(f, (*pos)++, SEEK_SET) != 0)
    {
        delete[] str;
        return string();
    }
    fread(str, sizeof(char), len, f);

    // ++(*pos); // Because otherwise, we end up reading the last '"' of the str
    (*pos) += len; // (len - 1) + 1, (the last '+1' replaces the '++pos')
    string fstr(str);
    delete[] str;
    return fstr;
}

/**
** \brief Takes sl's char array and transforms it into an int64_t.
**        If the number has an exponent, the exponent is parsed as well and the
**        number is elevated to that exponent
** \param sl A pointer to an StrAndLenTuple object
** \returns The 0 in case of error (or if the number was 0), the number
**          otherwise
*/
int64_t str_to_long(StrAndLenTuple *sl)
{
    char *str = sl->str;
    uint64_t len = sl->len;
    if (str == nullptr || len == 0)
    {
        return 0;
    }

    int64_t res = 0;
    uint64_t exponent = 0;
    char is_negative = str[0] == '-' ? -1 : 1;
    char is_in_exponent = 0;
    for (uint64_t i = 0; i < len; ++i)
    {
        if (sl->has_exponent && (str[i] == 'e' || str[i] == 'E'))
        {
            is_in_exponent = 1;
        }
        else if ('0' <= str[i] && str[i] <= '9')
        {
            if (is_in_exponent)
            {
                exponent = exponent * 10 + str[i] - '0';
            }
            else
            {
                res = res * 10 + str[i] - '0';
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
    char *str = sl->str;
    uint64_t len = sl->len;
    if (str == nullptr || len == 0)
    {
        return 0;
    }

    double res = 0; // Integer part
    double dot_res = 0; // Decimal part
    uint64_t exponent = 0; // Only used if sl->has_exponent() is true
    uint64_t nb_digits_dot = 1;
    // If the number is negative, this is set to -1 and the final res is
    // multiplied by it
    char is_negative = str[0] == '-' ? -1 : 1;
    char dot_reached = 0;
    char is_in_exponent = 0;
    for (uint64_t i = 0; i < len; ++i)
    {
        if (str[i] == '.')
        {
            dot_reached = 1;
        }
        else if (sl->has_exponent && (str[i] == 'e' || str[i] == 'E'))
        {
            is_in_exponent = 1;
        }
        else if ('0' <= str[i] && str[i] <= '9')
        {
            if (is_in_exponent)
            {
                exponent = exponent * 10 + str[i] - '0';
            }
            else if (dot_reached)
            {
                dot_res = dot_res * 10 + str[i] - '0';
                nb_digits_dot *= 10;
            }
            else
            {
                res = res * 10 + str[i] - '0';
            }
        }
    }
    return sl->has_exponent
        ? pow(is_negative * (res + (dot_res / nb_digits_dot)), exponent)
        : is_negative * (res + (dot_res / nb_digits_dot));
}

bool is_float(char *str, uint64_t len)
{
    if (str == nullptr)
    {
        return false;
    }

    for (uint64_t i = 0; i < len; ++i)
    {
        if (str[i] == '.')
        {
            return true;
        }
    }
    return false;
}

bool has_exponent(char *str, uint64_t len)
{
    if (str == nullptr)
    {
        return false;
    }

    for (uint64_t i = 0; i < len; ++i)
    {
        if (str[i] == 'e' || str[i] == 'E')
        {
            return true;
        }
    }
    return false;
}

/**
** \brief Reads the buffer from the given pos - 1
** \param buff The buffer containing the current json file or object
** \param pos The position of the second character of the number (the first one
**            was already read)
** \returns An instance of the StrAndLenTuple class containing the number as a
**          char array, the length of the char array and whether the number is a
**          float and has an exponent
*/
StrAndLenTuple parse_number_buff(char buff[READ_BUFF_MAX_SIZE], uint64_t *pos)
{
    if (pos == nullptr)
    {
        return StrAndLenTuple(nullptr, 0, false, false);
    }

    // Counts the number of characters until the first one that is an 'end char'
    uint64_t idx = *pos;
    uint64_t initial_i = idx;
    char c = 0;
    for (; idx < READ_BUFF_MAX_SIZE; ++idx)
    {
        c = buff[idx];
        if (IS_END_CHAR(c))
        {
            break;
        }
    }

    // Number of chars
    uint64_t len = idx - initial_i;
    if (len == 0)
    {
        return StrAndLenTuple(nullptr, 0, false, false);
    }

    // Puts the value in the form of a char array
    char *str = new char[len + 1]();
    if (str == nullptr)
    {
        return StrAndLenTuple(nullptr, 0, false, false);
    }

    for (uint64_t i = 0; i < len; ++i)
    {
        str[i] = buff[i + initial_i];
    }

    (*pos) += len - 1;
    return StrAndLenTuple(str, len, is_float(str, len), has_exponent(str, len));
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
StrAndLenTuple parse_number(FILE *f, uint64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return StrAndLenTuple(nullptr, 0, false, false);
    }

    // Because we already read the first digit (or sign)
    --(*pos);

    // Obtains the length of the value
    uint64_t size = *pos;
    if (fseek(f, size++, SEEK_SET) != 0)
    {
        return StrAndLenTuple(nullptr, 0, false, false);
    }

    char c = '\0';
    while ((c = fgetc(f)) != EOF)
    {
        if (IS_END_CHAR(c))
        {
            break;
        }
        if (fseek(f, size++, SEEK_SET) != 0)
        {
            break;
        }
    }

    uint64_t len = size - (*pos) - 1;
    if (len == 0)
    {
        return StrAndLenTuple(nullptr, 0, false, false);
    }

    // Puts the value in the form of a char array
    char *str = new char[len + 1]();
    if (str == nullptr || fseek(f, *pos, SEEK_SET) != 0)
    {
        return StrAndLenTuple(nullptr, 0, false, false);
    }

    fread(str, sizeof(char), len, f);

    (*pos) += len;
    return StrAndLenTuple(str, len, is_float(str, len), has_exponent(str, len));
}

/**
** \returns 5 if false, 4 if true, 0 otherwise
**/
uint64_t parse_boolean_buff(char buff[READ_BUFF_MAX_SIZE], uint64_t *pos)
{
    if (pos == nullptr)
    {
        return 0;
    }

    uint64_t idx = *pos;
    char c = 0;
    for (; idx < READ_BUFF_MAX_SIZE; ++idx)
    {
        c = buff[idx];
        if (IS_END_CHAR(c))
        {
            break;
        }
    }
    uint64_t len = idx - *pos;
    (*pos) += len - 1;
    return len;
}

/**
** \returns 5 if false, 4 if true, 0 otherwise
**/
uint64_t parse_boolean(FILE *f, uint64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return 0;
    }

    // Because we already read the first character
    --(*pos);

    uint64_t size = (*pos);
    if (fseek(f, size++, SEEK_SET) != 0)
    {
        return 0;
    }

    char c = 0;
    while ((c = fgetc(f)) != EOF)
    {
        if (IS_END_CHAR(c))
        {
            break;
        }
        if (fseek(f, size++, SEEK_SET) != 0)
        {
            break;
        }
    }
    uint64_t len = size - (*pos) - 1;
    (*pos) += len;
    return len;
}

/**
** \param f The file stream
** \param pos The position in the file of the character '[' that begins the
**            current array
** \returns The total number of characters in the current array
*/
uint64_t get_nb_chars_in_array(FILE *f, uint64_t pos)
{
    if (f == nullptr)
    {
        return 0;
    }

    uint64_t offset = pos;
    if (fseek(f, offset++, SEEK_SET) != 0)
    {
        return 0;
    }

    uint64_t nb_chars = 0;
    char c = 0;
    nested_arrays_t is_in_array = 1;
    nested_dicts_t is_in_dict = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    while ((c = fgetc(f)) != EOF)
    {
        if (!is_in_array)
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
        }
        ++nb_chars;

        if (fseek(f, offset++, SEEK_SET) != 0)
        {
            break;
        }
    }
    return nb_chars + 1;
}

/**
** \param buff The buffer containing the object currently being parsed
** \param idx The index of the character just after the '[' that begins the
**            current array
** \returns The number of elements of the current array
*/
uint64_t get_nb_elts_array_buff(char buff[READ_BUFF_MAX_SIZE], uint64_t idx)
{
    if (buff[idx] == ']')
    {
        return 0;
    }

    uint64_t size = 0;
    char c = 0;
    char prev_c = 0;
    nested_arrays_t is_in_array = 1;
    nested_dicts_t is_in_dict = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    char comma_encountered = 0;
    while (idx < READ_BUFF_MAX_SIZE)
    {
        if (!is_in_array)
        {
            break;
        }

        c = buff[idx];
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
            else if (!is_in_dict && is_in_array == 1 && c == ',')
            {
                ++size;
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
    if ((size == 0 && prev_c != 0) || (size >= 1 && comma_encountered))
    {
        ++size;
    }
    return size;
}

/**
** \param f The file stream
** \param pos The position in the file of the character just after the '[' that
**            begins the current array
** \returns The number of elements of the current array
*/
uint64_t get_nb_elts_array(FILE *f, uint64_t pos)
{
    if (f == nullptr)
    {
        return 0;
    }

    uint64_t offset = pos;
    if (fseek(f, offset++, SEEK_SET) != 0)
    {
        return 0;
    }

    uint64_t size = 0;

    char c = 0;
    char prev_c = 0;
    nested_arrays_t is_in_array = 1;
    nested_dicts_t is_in_dict = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    char comma_encountered = 0;
    while ((c = fgetc(f)) != EOF)
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
                ++is_in_dict;
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

        if (c != ' ' && c != '\t' && c != '\n')
        {
            prev_c = c;
        }
    }

    // If there was only one value, there was no ',', so the element wasn't
    // detected or if at least one element was found, it means that a ',' was
    // found
    if ((size == 0 && prev_c != 0) || (size >= 1 && comma_encountered))
    {
        ++size;
    }
    return size;
}

/**
** \param f The file stream
** \param pos The position in the file of the character '{' that begins the
**            current dict
** \returns The total number of characters in the current dict
*/
uint64_t get_nb_chars_in_dict(FILE *f, uint64_t pos)
{
    if (f == nullptr)
    {
        return 0;
    }

    uint64_t offset = pos;
    if (fseek(f, offset++, SEEK_SET) != 0)
    {
        return 0;
    }

    uint64_t nb_chars = 0;
    char c = 0;
    nested_dicts_t is_in_dict = 1;
    nested_arrays_t is_in_array = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    while ((c = fgetc(f)) != EOF)
    {
        if (!is_in_dict)
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
        }
        ++nb_chars;

        if (fseek(f, offset++, SEEK_SET) != 0)
        {
            break;
        }
    }
    return nb_chars + 1;
}

/**
** \param buff The buffer containing the object currently being parsed
** \param idx The index of the character just after the '{' that begins the
**            current dict
** \returns The number of elements of the current dict
*/
uint64_t get_nb_elts_dict_buff(char buff[READ_BUFF_MAX_SIZE], uint64_t pos)
{
    if (buff[pos] == '}')
    {
        return 0;
    }

    uint64_t size = 0;
    // Used for the case where the dict contains only one element, and so does
    // not contain a ','
    uint64_t single_elt_found = 0;

    char c = 0;
    nested_dicts_t is_in_dict = 1;
    nested_arrays_t is_in_array = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    while (pos < READ_BUFF_MAX_SIZE)
    {
        c = buff[pos];
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
        ++pos;
    }
    return size == 0 ? single_elt_found : size + 1;
}

/**
** \param f The file stream
** \param pos The position in the file of the character just after the '{' that
**            begins the current dict
** \returns The number of elements of the current dict
*/
uint64_t get_nb_elts_dict(FILE *f, uint64_t pos)
{
    if (f == nullptr)
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
    nested_dicts_t is_in_dict = 1;
    nested_arrays_t is_in_array = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    while ((c = fgetc(f)) != EOF)
    {
        if (!is_in_dict)
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
    }
    return size == 0 ? single_elt_found : size + 1;
}

/**
** \param buff The buffer containing the object currently being parsed
** \param idx The index of the character '[' that begins the current array
** \returns The json array parsed from the position
*/
JSONArray *parse_array_buff(char b[READ_BUFF_MAX_SIZE], uint64_t *pos)
{
    if (pos == nullptr)
    {
        return nullptr;
    }

    JSONArray *ja = new JSONArray();

    uint64_t nb_elts_parsed = 0;
    uint64_t nb_elts = get_nb_elts_array_buff(b, *pos + 1);
    if (nb_elts == 0)
    {
        return ja;
    }

    char c = 0;
    // We start at 1 because if we entered this function, it means that we
    // already read a '['
    uint64_t i = *pos + 1;
    uint64_t initial_i = i;
    for (; i < READ_BUFF_MAX_SIZE; ++i)
    {
        c = b[i];
        if (c == 0 || nb_elts_parsed >= nb_elts)
        {
            break;
        }

        if (c == '"')
        {
            ja->addValue(new StringTypedValue(parse_string_buff(b, &i)));
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
                ja->addValue(new DoubleTypedValue(str_to_double(&sl)));
            }
            else
            {
                ja->addValue(new IntTypedValue(str_to_long(&sl)));
            }
            ++nb_elts_parsed;
        }
        else if (IS_BOOL_START(c))
        {
            uint64_t len = parse_boolean_buff(b, &i);
            if (IS_NOT_BOOLEAN(c, len))
            {
                continue;
            }
            ja->addValue(new BoolTypedValue(len == 4 ? true : false));
            ++nb_elts_parsed;
        }
        else if (c == 'n')
        {
            ja->addValue(new NullTypedValue());
            i += 3;
            ++nb_elts_parsed;
        }
        else if (c == '[')
        {
            ja->addValue(new ArrayTypedValue(parse_array_buff(b, &i)));
            ++nb_elts_parsed;
        }
        else if (c == '{')
        {
            ja->addValue(new DictTypedValue(parse_dict_buff(b, &i)));
            ++nb_elts_parsed;
        }
    }
    (*pos) += i - initial_i;
    return ja;
}

/**
** \param f The file stream
** \param pos The postion in the file of the character just after the '[' that
**            begins the current array
** \returns The json array parsed from the position
*/
JSONArray *parse_array(FILE *f, uint64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return nullptr;
    }

    uint64_t nb_elts = get_nb_elts_array(f, *pos);
    uint64_t nb_elts_parsed = 0;

    JSONArray *ja = new JSONArray();
    if (nb_elts == 0)
    {
        return ja;
    }

    if (fseek(f, (*pos)++, SEEK_SET) != 0)
    {
        return nullptr;
    }

    char c = 0;
    while ((c = fgetc(f)) != EOF && nb_elts_parsed < nb_elts)
    {
        // If we are not in a string or if the string just ended
        if (c == '"')
        {
            ja->addValue(new StringTypedValue(parse_string(f, pos)));
            ++nb_elts_parsed;
        }
        else if (IS_NUMBER_START(c))
        {
            StrAndLenTuple sl = parse_number(f, pos);
            if (sl.str == nullptr)
            {
                continue;
            }

            if (sl.is_float)
            {
                ja->addValue(new DoubleTypedValue(str_to_double(&sl)));
            }
            else
            {
                ja->addValue(new IntTypedValue(str_to_long(&sl)));
            }
            ++nb_elts_parsed;
        }
        else if (IS_BOOL_START(c))
        {
            uint64_t len = parse_boolean(f, pos);
            if (len == 0 || (c == 'f' && len != 5) || (c == 't' && len != 4))
            {
                continue;
            }
            ja->addValue(new BoolTypedValue(len == 4 ? true : false));
            ++nb_elts_parsed;
        }
        else if (c == 'n')
        {
            ja->addValue(new NullTypedValue());
            (*pos) += 3;
            ++nb_elts_parsed;
        }
        else if (c == '[')
        {
            ja->addValue(new ArrayTypedValue(parse_array(f, pos)));
            ++nb_elts_parsed;
        }
        else if (c == '{')
        {
            uint64_t nb_chars = get_nb_chars_in_dict(f, *pos);
            JSONDict *jd = nullptr;
            if (nb_chars <= READ_BUFF_MAX_SIZE)
            {
                char b[READ_BUFF_MAX_SIZE] = {};
                fread(b, sizeof(char), nb_chars, f);
                jd = parse_dict_buff(b, pos);
            }
            else
            {
                jd = parse_dict(f, pos);
            }
            ja->addValue(new DictTypedValue(jd));
            ++nb_elts_parsed;
        }

        if (fseek(f, (*pos)++, SEEK_SET) != 0)
        {
            break;
        }
    }
    --(*pos);
    return ja;
}

/**
** \param b The buffer containing the object currently being parsed
** \param idx A pointer to the index of the character just after the '{' that
**            begins the current dict
** \returns The json dict parsed from the index
*/
JSONDict *parse_dict_buff(char b[READ_BUFF_MAX_SIZE], uint64_t *idx)
{
    if (idx == nullptr)
    {
        return nullptr;
    }

    JSONDict *jd = new JSONDict();

    string key = string();
    uint64_t nb_elts_parsed = 0;
    uint64_t nb_elts = get_nb_elts_dict_buff(b, *idx + 1);
    if (nb_elts == 0)
    {
        return jd;
    }

    char c = 0;
    char is_waiting_key = 1;
    // We start at 1 because if we entered this function, it means that we
    // already read a '{'
    uint64_t i = *idx + 1;
    uint64_t initial_i = i;
    for (; i < READ_BUFF_MAX_SIZE; ++i)
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
            uint64_t len = parse_boolean_buff(b, &i);
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
            jd->addItem(new ArrayItem(key, parse_array_buff(b, &i)));
            ++nb_elts_parsed;
        }
        else if (c == '{')
        {
            jd->addItem(new DictItem(key, parse_dict_buff(b, &i)));
            ++nb_elts_parsed;
        }
        else if (c == ',')
        {
            is_waiting_key = 1;
        }
    }
    (*idx) += i - initial_i;
    return jd;
}

/**
** \param f The file stream
** \param pos A pointer to the position in the file of the character just after
**            the '{' that begins the current dict
** \returns The json dict parsed from the position
*/
JSONDict *parse_dict(FILE *f, uint64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return nullptr;
    }

    JSONDict *jd = new JSONDict();
    // uint64_t i = *pos;

    if (fseek(f, *pos, SEEK_SET) != 0)
    {
        return nullptr;
    }

    string key = string();
    uint64_t nb_elts_parsed = 0;
    uint64_t nb_elts = get_nb_elts_dict(f, *pos);

    char c = 0;
    char is_waiting_key = 1;
    while ((c = fgetc(f)) != EOF && nb_elts_parsed < nb_elts)
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
                ++nb_elts_parsed;
            }
        }
        else if (IS_NUMBER_START(c))
        {
            StrAndLenTuple sl = parse_number(f, pos);
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
            uint64_t len = parse_boolean(f, pos);
            if (len == 0 || (c == 'f' && len != 5) || (c == 't' && len != 4))
            {
                continue;
            }
            jd->addItem(new BoolItem(key, len == 4 ? true : false));
            ++nb_elts_parsed;
        }
        else if (c == 'n')
        {
            jd->addItem(new NullItem(key));
            (*pos) += 3;
            ++nb_elts_parsed;
        }
        else if (c == '[')
        {
            jd->addItem(new ArrayItem(key, parse_array(f, pos)));
            ++nb_elts_parsed;
        }
        else if (c == '{')
        {
            jd->addItem(new DictItem(key, parse_dict(f, pos)));
            ++nb_elts_parsed;
        }
        else if (c == ',')
        {
            is_waiting_key = 1;
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
JSON *parse(char *file)
{
    FILE *f = fopen(file, "r");
    if (f == nullptr)
    {
        return nullptr;
    }

    uint64_t offset = 0;
    if (fseek(f, offset++, SEEK_SET) != 0)
    {
        fclose(f);
        return nullptr;
    }

    char c = fgetc(f);
    if (c == '{')
    {
        JSONDict *jd = nullptr;
        uint64_t nb_chars = get_nb_chars_in_dict(f, offset);
        if (nb_chars <= READ_BUFF_MAX_SIZE)
        {
            char b[READ_BUFF_MAX_SIZE] = {};
            if (fseek(f, offset, SEEK_SET) != 0)
            {
                fclose(f);
                return nullptr;
            }
            fread(b, sizeof(char), nb_chars, f);
            jd = parse_dict_buff(b, &offset);
        }
        else
        {
            jd = parse_dict(f, &offset);
        }

        fclose(f);
        return jd == nullptr ? nullptr : jd;
    }
    else if (c == '[')
    {
        JSONArray *ja = nullptr;
        uint64_t nb_chars = get_nb_chars_in_array(f, offset);
        if (nb_chars <= READ_BUFF_MAX_SIZE)
        {
            char b[READ_BUFF_MAX_SIZE] = {};
            if (fseek(f, offset, SEEK_SET) != 0)
            {
                fclose(f);
                return nullptr;
            }
            fread(b, sizeof(char), nb_chars, f);
            ja = parse_array_buff(b, &offset);
        }
        else
        {
            ja = parse_array(f, &offset);
        }
        fclose(f);
        return ja == nullptr ? nullptr : ja;
    }

    fclose(f);
    return nullptr;
}
