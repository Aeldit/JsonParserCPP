#include "json.hpp"
#include "parser.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 1;
    }

    printf("%s\n", argv[1]);

    JSON *j = parse(argv[1]);
    if (j == NULL)
    {
        return 1;
    }

    if (IS_JSON_ARRAY(j))
    {
        JSONArray *ja = (JSONArray *)j;
        ja->printValues();
    }
    else
    {
        JSONDict *jd = (JSONDict *)j;
        jd->printItems();
        jd->getItem("arr")->print();
    }
    delete j;

    /*LinkedList<TypedValue> *ll = new LinkedList<TypedValue>();
    for (int i = 0; i < 100; ++i)
    {
        ll->add(new IntTypedValue(i));
    }

    printf("size = %lu\n", ll->getSize());
    printf("nb links = %lu\n", ll->getNbLinks());

    ll->remove(5);
    for (int i = 0; i < 16; ++i)
    {
        ll->remove(1);
    }
    printf("size = %lu\n", ll->getSize());

    TypedValue *t = ll->get(1);
    if (t != NULL)
    {
        t->print();
    }
    printf("\n%lu\n", ll->getNbLinks());
    delete ll;*/

    // cout << boolalpha;
    // cout << (FastCompString("Testing", 7) == FastCompString("T", 1)) << endl;
    return 0;
}
