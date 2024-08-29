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
    printf("%s\n", argv[0]);

    JSON *j = parse(argv[1]);
    if (j == NULL)
    {
        return 1;
    }

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
        if (i == NULL)
        {
            delete j;
            return 1;
        }

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

    LinkedList<TypedValue *> l = LinkedList<TypedValue *>();
    l.add(new StringTypedValue("test"));
    l.add(new IntTypedValue(1));
    l.add(new DoubleTypedValue(0.5));
    l.add(new BoolTypedValue(false));
    l.add(new StringTypedValue("t"));
    l.add(new StringTypedValue("te"));
    l.add(new StringTypedValue("tes"));
    l.add(new StringTypedValue("1est"));
    l.add(new StringTypedValue("2est"));
    l.add(new StringTypedValue("3est"));
    l.add(new StringTypedValue("4est"));
    l.add(new StringTypedValue("5est"));
    l.add(new StringTypedValue("6est"));
    l.add(new StringTypedValue("7est"));
    l.print();

    return 0;
}
