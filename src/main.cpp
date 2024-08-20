#include "json.hpp"

int main(void)
{
    StringItem s("Test", "vali");
    s.print();

    IntItem i("number", 50);
    i.print();

    BoolItem b("boolean", true);
    b.print();
    return 0;
}
