#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <string>
using namespace std;

int main()
{
    int a = 1;
    string toSend = "CREATEROOM@" + to_string(a);
    cout<<toSend <<endl;
    return 0; 
}
