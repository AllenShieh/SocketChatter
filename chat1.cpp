//don't worry about these headers
#ifndef WIN32_LEAN_AND_MEAN  
#define WIN32_LEAN_AND_MEAN  
#endif  
#include <iostream>
#include <Windows.h>  
#include <WinSock2.h>  
#include <WS2tcpip.h>  
#include <IPHlpApi.h>  
#include <cstdio>  
using namespace std;
#pragma comment (lib, "Ws2_32.lib")  
#define DEFAULT_PORT "27015"
#define DEFAULT_PORT_REC "27013"
#define DEFAULT_BUFLEN 512  
SOCKET ClientSocket = INVALID_SOCKET; 
char temp[DEFAULT_BUFLEN];
char temp2[DEFAULT_BUFLEN];

//sender thread
DWORD WINAPI sender(LPVOID lpParameter){
	while (1){
		scanf_s("%s",temp);
		send(ClientSocket, temp, 256, 0);
		if(strcmp(temp,"exit")==0) exit(0);
	}
}

//receiver thread
DWORD WINAPI receiver(LPVOID lpParameter){
	while (1){
		int num = 0;  
		while (1)  
		{  
			num = recv(ClientSocket, temp2, DEFAULT_BUFLEN, 0);  
			if(strcmp(temp2,"exit")==0){
				printf("lost connection\n");
				system("pause");
				exit(0);
			}
			if(num == 0) break;  
			printf("%s\n",temp2); 
		}  
		printf("\n");
	}
}

int main ()  
{  
	//you don't need to know these and i barely understand either
    int iResult = 0;
    WSADATA wsaData;  
    struct addrinfo *result = NULL, *ptr = NULL, hints;  
	
    if(WSAStartup(MAKEWORD(2, 2), &wsaData))  
    {  
        printf("server WSAStartup failed: %d\n", iResult);  
        return 1;  
    }  

    ZeroMemory(&hints, sizeof(hints));  
    hints.ai_family = AF_INET;  
    hints.ai_socktype = SOCK_STREAM;  
    hints.ai_protocol = IPPROTO_TCP;  
    hints.ai_flags = AI_PASSIVE; 

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);  
    if(iResult != 0)  
    {  
        printf("server getaddrinfo faild: %d\n", iResult);  
        WSACleanup();  
        return 1;  
    }  
    
    SOCKET ListenSocket = INVALID_SOCKET;  
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);  
    if(ListenSocket == INVALID_SOCKET)  
    {  
        printf("server failed at socket(): %ld\n", WSAGetLastError());  
        freeaddrinfo(result);  
        WSACleanup();  
        return 1;  
    }  
    
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);  
    if(iResult == SOCKET_ERROR)  
    {  
        printf("server bind faild: %ld\n", WSAGetLastError());  
        freeaddrinfo(result);  
        closesocket(ListenSocket);  
        WSACleanup();  
        return 1;  
    }  
    freeaddrinfo(result);  

    if(listen(ListenSocket, SOMAXCONN))  
    {  
        printf("server listen socket failed %ld\n", WSAGetLastError());  
        closesocket(ListenSocket);  
        WSACleanup();  
        return 1;  
    }  
    
    ClientSocket = accept(ListenSocket, NULL, NULL);  
    if(ClientSocket == INVALID_SOCKET)  
    {  
        printf("server accept failed: %ld\n",WSAGetLastError());  
        closesocket(ListenSocket);  
        WSACleanup();  
        return 1;  
    }
    closesocket(ListenSocket);  
    
	//create the two threads
	HANDLE handle1,handle2;
    handle1 = CreateThread(NULL,0,sender,NULL,0,NULL);
    handle2 = CreateThread(NULL,0,receiver,NULL,0,NULL);
	if(NULL == handle1)
    {
        cout<<"Create Thread failed !"<<endl;
        return -1;
    }
    if(NULL == handle2)
    {
        cout<<"Create Thread failed !"<<endl;
        return -1;
    }

	printf("entering chatting\n");
	while(1);

    iResult = shutdown(ClientSocket, SD_SEND);  
    if(iResult == SOCKET_ERROR)  
    {  
        printf("server shutdown failed %ld\n", WSAGetLastError());  
        closesocket(ClientSocket);  
        WSACleanup();  
        return 1;  
    }  
    closesocket(ClientSocket);  
    WSACleanup(); 

    return 0;  
}  