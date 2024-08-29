#include <iostream>

#include "json.hpp"
#include "linked_lists.hpp"
#include "parser.hpp"

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
                ai->print();
            }
        }
    }*/
    delete j;

    LinkedList<Item *> *ll = new LinkedList<Item *>();
    ll->add(new IntItem("key", 45));
    ll->add(new BoolItem("k", false));
    ll->add(new DoubleItem("d", 0.5));
    ll->add(new IntItem("k", 1));
    ll->add(new StringItem("e", "my_string"));
    ll->add(new IntItem("y", 3));
    ll->print();
    delete ll;
    return 0;
}
