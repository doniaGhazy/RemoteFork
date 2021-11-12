#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include"myfork.h"

using namespace std;
int  myconnect(string ip, int port)
{

    struct hostent *hostName = gethostbyname(ip.c_str());
    sockaddr_in mysend;
    bzero((char *)&mysend, sizeof(mysend));
    mysend.sin_family = AF_INET;
    mysend.sin_addr.s_addr =
        inet_addr(inet_ntoa(*(struct in_addr *)*hostName->h_addr_list));
    mysend.sin_port = htons(port);
    int mySd = socket(AF_INET, SOCK_STREAM, 0);
    int status = connect(mySd,
                        (sockaddr *)&mysend, sizeof(mysend));

    if (status < 0) return -1;
    return mySd;

}

int myfork(string IP)
{
    int pid;
    int mySd; 
    char target[15] = {};
    char mypid[15] = {};
    char flag = 'a';

    pid = fork();
    if(pid!=0)
        return getpid();


    pid = getpid();
    strcpy(mypid,to_string(pid).c_str());
    mySd= myconnect("172.0.0.1",8888);



    if (mySd < 0)
        return 1;

    send(mySd, (char *)&flag, 1, 0);
    strcpy(target, IP.c_str());
    send(mySd, (char *)&target, strlen(target), 0);
    sleep(3);
    send(mySd, (char *)&mypid, strlen(mypid), 0);
    close(mySd);

    return 1;
} 