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
    /*if (j->isArray())
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
                cout << ((StringTypedValue *)ai)->getValue() << endl;
            }
        }
    }*/
    delete j;

    ItemT<int> it("typed", T_NUM, 5);
    cout << it.getValue() << endl;
    it.print();
    return 0;
}
