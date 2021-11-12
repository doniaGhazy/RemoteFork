#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include"myfork.h"
using namespace std;
int main()
{
    string x;
    int i= 0;

    if(myfork("192.168.1.20")==getpid())
        x = "Parent process";
    else 
        x = "Child process";
            
    while (i<=5)
    {
        sleep(15);
        printf("PID of %s, is: %d \n", x, getpid());
        i++;
    }

    return 0;
}
