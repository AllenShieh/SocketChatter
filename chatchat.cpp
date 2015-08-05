//don't worry about these headers
#ifndef WIN32_LEAN_AND_MEAN  
#define WIN32_LEAN_AND_MEAN  
#endif  
#include <Windows.h>  
#include <winsock2.h>  
#include <ws2tcpip.h>  
#include <iphlpapi.h>
#include <cstdio>  
#include <iostream>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")  
#pragma comment (lib, "Mswsock.lib")  
#pragma comment (lib, "AdvApi32.lib")  
#define DEFAULT_PORT "27015"  
#define DEFAULT_BUFLEN 512  
SOCKET ConnectSocket = INVALID_SOCKET;  

//enter the ip you are going to chat with
char IP[DEFAULT_BUFLEN] = "192.168.1.104";
char temp[DEFAULT_BUFLEN];
char temp2[DEFAULT_BUFLEN];

//sender thread
DWORD WINAPI sender(LPVOID lpParameter){
	while (1){
		scanf_s("%s",temp);
		send(ConnectSocket, temp, 256, 0);
		if(strcmp(temp,"exit")==0) exit(0);
	}
}

//receiver thread
DWORD WINAPI receiver(LPVOID lpParameter){
	while (1){
		int num = 0;  
		while (1)  
		{  
			num = recv(ConnectSocket, temp2, DEFAULT_BUFLEN, 0);  
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

int main()  
{  
	//whatever, i barely understand these
    int iResult = 0;  
    WSADATA wsaData;  
    struct addrinfo *result = NULL, *ptr = NULL, hints;   
    int recvbuflen = DEFAULT_BUFLEN; 

    if(WSAStartup(MAKEWORD(2,2), &wsaData))  
    {  
        printf("client WSAStartup failed: %d\n", iResult);  
        return 1;  
    }  
  
    ZeroMemory(&hints, sizeof(hints));  
    hints.ai_family = AF_UNSPEC;  
    hints.ai_socktype = SOCK_STREAM;  
    hints.ai_protocol = IPPROTO_TCP;  
  
    //this is important
    iResult = getaddrinfo(IP, DEFAULT_PORT, &hints, &result);  
    if(iResult != 0)  
    {  
        printf("client get addrinfor fail: %d\n", iResult);  
        WSACleanup(); // terminate use of WS2_32.dll  
        return 1;  
    }  
  
    for(ptr = result; ptr != NULL; ptr = ptr->ai_next)  
    {  
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);  
        if(ConnectSocket == INVALID_SOCKET)  
        {  
            printf("error %ld\n", WSAGetLastError());  
            WSACleanup();  
            return 1;  
        }  
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);  
        if(iResult == SOCKET_ERROR)  
        {  
            closesocket(ConnectSocket);  
            ConnectSocket = INVALID_SOCKET;// if fail try next address returned by getaddrinfo  
            continue;  
        }  
        break;  
    }  
  
    freeaddrinfo(result);  
    if(ConnectSocket == INVALID_SOCKET)  
    {  
        printf("client unable to connect to server\n");  
        WSACleanup();  
        return 1;  
    }  
    
	//create threads
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

    closesocket(ConnectSocket);  
    WSACleanup();  
	
    return 0;  
}  

