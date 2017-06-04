
#include <iostream>

using namespace std;

extern "C" void write(int c)
{
    cout << c;
}

extern "C" void writeln(int c)
{
    cout << c << endl;
}

extern "C" int read()
{
    int n;
    cin >> n;
    return n;
}
