#include "json.hpp"
#include "parser.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 1;
    }

    JSON *j = parse(argv[1]);
    if (j->isArray())
    {
        ((JSONArray *)j)->printValues();
    }
    else
    {
        ((JSONDict *)j)->printItems();
    }
    return 0;
}
