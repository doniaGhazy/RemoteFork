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
using namespace std;
string dirname = "hello";
string actualname = "d.zip";
string rtext(int newSd)
{
    char data[100];
    memset(&data, 0, sizeof(data));
    recv(newSd, (char *)&data, sizeof(data), 0);
    return data;
}
void rfile(string filename, int newSd, string size2)
{
    ofstream file;
    int valread = 0;
    int size = atoi(size2.c_str());
    cout << "ORIGINAL SIZE    " << size << endl;
    file.open(filename, ios::out | ios::trunc | ios::binary);
    if (file.is_open())
    {
        cout << "[LOG] : File Creted.\n";
    }
    else
    {
        cout << "[ERROR] : File creation failed, Exititng.\n";
        exit(EXIT_FAILURE);
    }
    char buffer[1000000] = {};
    int index = 0;
    while (index < size - 1)
    {
        valread = recv(newSd, &buffer[index], 1024, 0);
        index += valread;
        cout << index << endl;
    }
    cout << valread << endl;
    cout << "[LOG] : Data received " << valread << " bytes\n";
    cout << "[LOG] : Saving data to file.\n";
    for (int i = 0; i < size; i++)
        file.put(buffer[i]);
    file.close();
}
int mysend(string filename, int socket, int size)
{
    fstream file;
    file.open(filename, ios::in | ios::binary);
    if (!file.is_open())
    {
        cout << "Can't load file.\n";
        return 1;
    }
    int index = 0;
    int sent = 0;
    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    while (index < contents.length())
    {
        sent = send(socket, contents.c_str(), contents.length(), 0);
        index += sent;
    }
    cout << "SENT " << index << " Bytes\n";
    return 0;
}
//byft7 port na 3ayz a recieve 3leh
int getready(int port)
{
    sockaddr_in servAddr;
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    int firstSD = socket(AF_INET, SOCK_STREAM, 0);
    if (firstSD < 0)
    {
        cerr << "Can't establish socket" << endl;
        exit(0);
    }
    int bindStatus = bind(firstSD, (struct sockaddr *)&servAddr,
                          sizeof(servAddr));
    if (bindStatus < 0)
    {
        cout << "Socket not available" << endl;
        exit(0);
    }
    cout << "Server active" << endl;
    listen(firstSD, 5);
    sockaddr_in secondSocket;
    socklen_t newSockAddrSize = sizeof(secondSocket);
    int secondSD = accept(firstSD, (sockaddr *)&secondSocket, &newSockAddrSize);
    close(firstSD);
    return secondSD;
}
int getfolderready(string pid)
{
    string removedzip = ("rm " + actualname + " -f > /dev/null");
    string removedfolder = ("rm -r " + dirname + " -f > /dev/null");
    string newfolder = ("mkdir " + dirname);
    system(removedzip.c_str());
    system(removedfolder.c_str());
    system(newfolder.c_str());
    string criumessage = "sudo criu dump -t " + string(pid) + " -D .//" + dirname + " -s --shell-job > /dev/null";
    system(criumessage.c_str());
    system(string("zip -r " + actualname + " myfork.cpp myfork.h c.cpp hello > /dev/null").c_str());
    ifstream ifile(actualname.c_str(), ios::binary);
    ifile.seekg(0, ios::end);
    int size = ifile.tellg();
    ifile.close();
    return size;
}
int getsendingready(int port, string IP)
{
    struct hostent *host = gethostbyname(IP.c_str());
    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =
        inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    int status = connect(clientSd,
                         (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));
    return clientSd;
}
int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        cout << "Invalid number of arguments\n";
        return 0;
    }
    int port = atoi(argv[1]);
    char msg[100];
    int secondSD = getready(port);
    if (secondSD < 0)
        return 1;
    cout << "Connected" << endl;
    recv(secondSD, (char *)&msg, 1, 0);
    if (strcmp(msg, string("a").c_str()) == 0)
    {
        memset(&msg, 0, sizeof(msg));
        recv(secondSD, (char *)&msg, sizeof(msg), 0);
        string pid = msg;
        int file_size = getfolderready(pid);
        memset(&msg, 0, sizeof(msg));
        recv(secondSD, (char *)&msg, sizeof(msg), 0);
        string serverIp = msg;
        close(secondSD);
        cout << "PID is: " << pid << "\nIP is: " << serverIp << endl;
        int clientSd = getsendingready(port, serverIp);
        string signaltoget = "1";
        memset(&msg, 0, sizeof(msg));
        strcpy(msg, signaltoget.c_str());
        send(clientSd, (char *)&msg, strlen(msg), 0);
        string filename = actualname + " " + to_string(file_size) + "-" + pid;
        memset(&msg, 0, sizeof(msg));
        strcpy(msg, filename.c_str());
        send(clientSd, (char *)&msg, strlen(msg), 0);
        mysend(actualname, clientSd, file_size);
        close(clientSd);
    }
    else if (strcmp(msg, string("1").c_str()) == 0)
    {
        system("rm -r hello -f > /dev/null");
        system("rm d.zip -f > /dev/null");
        string x = rtext(secondSD);
        string y = x.substr(x.find(' ') + 1);
        y = y.substr(0, y.find('-'));
        string pid = x.substr(x.find('-') + 1);
        x = x.substr(0, x.find(' '));
        cout << x << endl;
        //cout << y << endl;
        cout << "waiting for files\n"
             << endl;
        rfile(x, secondSD, y);
        cout << "DONE RECIEVING\n";
        string call = "gnome-terminal -- /bin/sh -c ' unzip -o d.zip > /dev/null; g++ -o a myfork.cpp c.cpp > /dev/null; kill -9 " + pid + " > /dev/null; sudo criu restore -D ./hello --shell-job > /dev/null; exec bash  '";
        system(call.c_str());
    }

    return 0;
}