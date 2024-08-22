#include <iostream>

#include "json.hpp"
#include "parser.hpp"
#include "types.h"

using namespace std;

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
        JSONDict *jd = (JSONDict *)j;
        jd->printItems();
    }
    return 0;
}
