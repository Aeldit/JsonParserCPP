
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

    if (j->isArray())
    {
        JSONArray *ja = (JSONArray *)j;
        ja->printValues();
    }
    else
    {
        JSONDict *jd = (JSONDict *)j;
        jd->printItems();

        /*Item *i = jd->getItem("arr");
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
        }*/
    }
    delete j;
    return 0;
}
