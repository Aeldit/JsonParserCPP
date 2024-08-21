#include "json.hpp"
#include "parser.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 1;
    }

    JSONDict *jd = parse(argv[1]);

    jd->printItems();
    return 0;
}
