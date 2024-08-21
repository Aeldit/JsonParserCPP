#include "json.hpp"

int main(void)
{
    JSONDict jd;

    StringItem s("Test", "vali");

    IntItem i("number", 50);

    BoolItem b("boolean", true);

    jd.addItem(&s);
    jd.addItem(&i);
    jd.addItem(&b);

    jd.printItems();

    size_t nb_elts = jd.getNbElts();
    Item **items = jd.getItems();
    for (size_t i = 0; i < nb_elts; ++i)
    {
        if (items[i]->getType() == TYPE_STR)
        {
            cout << ((StringItem *)items[i])->getValue() << endl;
        }
    }
    return 0;
}
