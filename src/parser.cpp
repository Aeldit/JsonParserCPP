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

#ifndef READ_BUFF_MAX_SIZE_OVERRIDE
#    define READ_BUFF_MAX_SIZE 1024
#endif

/*******************************************************************************
**                           FUNCTIONS DECLARATIONS                           **
*******************************************************************************/
JSONDict *parse_json_dict(FILE *f, uint64_t *pos);

/*******************************************************************************
**                              LOCAL FUNCTIONS                               **
*******************************************************************************/
/**
** \brief Reads the string at position 'pos' in the given file, and adds it to
**        the given dict
** \param pos The pos of the '"' that starts the string of which we are
**            currently acquiring the length
** \returns An empty string in case of error, the parsed string otherwise
*/
string parse_string_buff(char buff[READ_BUFF_MAX_SIZE], uint64_t *pos)
{
    if (pos == nullptr)
    {
        return string();
    }

    uint64_t i = *pos + 1;
    uint64_t initial_i = i;
    char c = 0;
    char prev_c = 0;
    for (; i < READ_BUFF_MAX_SIZE; ++i)
    {
        c = buff[i];
        if (c == 0 || c == '"' && prev_c != '\\')
        {
            break;
        }
        prev_c = c;
    }

    uint64_t len = i - initial_i;
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
    (*pos) += len + 1;
    string fstr(str);
    delete[] str;
    return fstr;
}

/**
** \brief Reads the string at position 'pos' in the given file, and adds it to
**        the given dict
** \param f The file stream
** \param pos The pos of the '"' that starts the string of which we are
**            currently acquiring the length
** \returns An empty string in case of error, the parsed string otherwise
*/
string parse_string(FILE *f, uint64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return string();
    }

    uint64_t size = (*pos);
    char c = '\0';
    char prev_c = '\0';
    while ((c = fgetc(f)) != EOF)
    {
        if (c == '"' && prev_c != '\\')
        {
            break;
        }
        if (fseek(f, size++, SEEK_SET) != 0)
        {
            break;
        }
        prev_c = c;
    }

    uint64_t len = size - (*pos) - 1;
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
    uint64_t size = (*pos);
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
    if (str == nullptr || fseek(f, (*pos), SEEK_SET) != 0)
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

    (*pos) += len;
    return len;
}

/**
** \param f The file stream
** \param pos The pos of the character just after the '[' that begins the
**            current array
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

    char c = '\0';
    char prev_c = '\0';
    char is_in_array = 1;
    char is_in_dict = 0;
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
                if (is_in_array == 1 && prev_c == '\0')
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
    if (size >= 1 && comma_encountered)
    {
        ++size;
    }
    // If there was only one value, there was no ',', so the element wasn't
    // detected
    if (size == 0 && prev_c != '\0')
    {
        ++size;
    }
    return size;
}

/**
** \param f The file stream
** \param pos The pos of the character just after the '[' that begins the
**            current array
** \returns The json array parsed at the pos
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

    char c = '\0';
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
            ja->addValue(new DictTypedValue(parse_json_dict(f, pos)));
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
    char c = '\0';
    char is_in_dict = 1;
    char is_in_array = 0;
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
            else if (!is_in_array && is_in_dict == 1 && c == ',')
            {
                ++nb_chars;
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
** \param buff The buffer containing the current dict
** \returns The number of elements of the current dict
*/
uint64_t get_nb_elts_dict_buff(char buff[READ_BUFF_MAX_SIZE])
{
    uint64_t size = 0;
    // Used for the case where the dict contains only one element, and so does
    // not contain a ','
    uint64_t single_elt_found = 0;

    char c = '\0';
    char is_in_dict = 1;
    char is_in_array = 0;
    char is_in_string = 0;
    char is_backslashing = 0;
    uint64_t i = 0;
    while (i < READ_BUFF_MAX_SIZE && buff[i] != 0)
    {
        c = buff[i];
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
        ++i;
    }
    return size == 0 ? single_elt_found : size + 1;
}

/**
** \param f The file stream
** \param pos The pos of the character just after the '{' that begins the
**            current dict
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
    char is_in_dict = 1;
    char is_in_array = 0;
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
** \param f The file stream
** \param pos The pos of the character just after the '[' that begins the
**            current array
** \returns The json dict parsed at the pos
*/
JSONDict *parse_json_dict(FILE *f, uint64_t *pos)
{
    if (f == nullptr || pos == nullptr)
    {
        return nullptr;
    }

    JSONDict *jd = new JSONDict();

    uint64_t nb_chars_in_dict = get_nb_chars_in_dict(f, *pos);
    printf("nb chars = %lu\n", nb_chars_in_dict);

    if (fseek(f, (*pos), SEEK_SET) != 0)
    {
        return nullptr;
    }

    // Read from the buffer if the size is not too big, improving performances
    // Otherwise, read using fseek and fgetc (slower)
    if (nb_chars_in_dict <= READ_BUFF_MAX_SIZE)
    {
        // + 1 to ensure that the string is null-terminated
        char buff[READ_BUFF_MAX_SIZE + 1] = {};
        fread(buff, sizeof(char), nb_chars_in_dict, f);
        printf("buff = [%s]\n", buff);

        string key = string();
        uint64_t nb_elts = get_nb_elts_dict_buff(buff);
        uint64_t nb_elts_parsed = 0;
        printf("nb elts = %lu\n", nb_elts);

        char c = 0;
        char is_waiting_key = 1;
        for (uint64_t i = 0; i < nb_chars_in_dict; ++i)
        {
            if (nb_elts_parsed >= nb_elts)
            {
                break;
            }

            c = buff[i];
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
                if (len == 0 || (c == 'f' && len != 5)
                    || (c == 't' && len != 4))
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
                jd->addItem(new DictItem(key, parse_json_dict(f, pos)));
                ++nb_elts_parsed;
            }
            else if (c == ',')
            {
                is_waiting_key = 1;
            }
        }
    }
    else
    {
        string key = string();
        uint64_t nb_elts = get_nb_elts_dict(f, *pos);
        uint64_t nb_elts_parsed = 0;

        char c = '\0';
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
                if (len == 0 || (c == 'f' && len != 5)
                    || (c == 't' && len != 4))
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
                jd->addItem(new DictItem(key, parse_json_dict(f, pos)));
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
    }
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
        JSONDict *jd = parse_json_dict(f, &offset);
        fclose(f);
        if (jd == nullptr)
        {
            return nullptr;
        }
        return jd;
    }
    else if (c == '[')
    {
        // TODO: Fix parsing when dicts without keys are inside an array
        JSONArray *ja = parse_array(f, &offset);
        fclose(f);
        if (ja == nullptr)
        {
            return nullptr;
        }
        return ja;
    }

    fclose(f);
    return nullptr;
}
