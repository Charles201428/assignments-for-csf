#include <iostream>
#include <string>

using namespace std;


int main() {
    int a = stol("0x0000AA40", nullptr, 0);
    cout << a <<endl;
    return 0;
}