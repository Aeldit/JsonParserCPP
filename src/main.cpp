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

        Item *i = jd->getItem("dict");
        if (i == NULL)
        {
            return 1;
        }
        /*if (i->getType() != TYPE_DICT)
        {
            // printf("%d \n", i->getType());
            // cout << "AAA" << endl;
            return 1;
        }*/
        // i->print();
        cout << endl;

        /*JSONDict *j2 = (JSONDict *)i;
        Item *i2 = j2->getItem("empty");
        if (i2->getType() != TYPE_ARR)
        {
            return 3;
        }
        i2->print();
        cout << endl;

        JSONArray *ja = (JSONArray *)i2;
        TypedValue *t = ja->getValueAt(0);
        if (t->getType() != TYPE_ARR)
        {
            return 4;
        }
        t->print();
        cout << endl;*/
        //-> getValueAt(0)->print();
    }
    return 0;
}
