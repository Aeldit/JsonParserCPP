#include <iostream>

#include "json.hpp"
#include "parser.hpp"

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
        JSONArray *ja = (JSONArray *)j;
        ja->printValues();
    }
    else
    {
        JSONDict *jd = (JSONDict *)j;
        jd->printItems();

        Item *i = jd->getItem("arr");
        if (i->isArray())
        {
            TypedValue *ai = ((ArrayItem *)i)->getValue()->getValueAt(0);
            if (ai->isString())
            {
                ai->print();
            }
        }
    }
    delete j;
    return 0;
}
