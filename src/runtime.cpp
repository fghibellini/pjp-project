
#include <iostream>

using namespace std;

extern "C" int pis(int c)
{
    cout << "Cislo: <" << c << ">" << endl;

    return 42;
}
