#include "TcpSocket.hpp"

#include <iostream>

using namespace std;

TcpSocket::TcpSocket(const char* host, int port) 
    : mHost(host), mPort(port)
{
	ConnectSocket = INVALID_SOCKET; 
}

TcpSocket::~TcpSocket(void)
{
}

/*
* Handles the setup of the socket
*/
void TcpSocket::Open()
{
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup failed with error: %d\n", iResult);
        error = 1;
    }

    //----------------------
    // Create a SOCKET for connecting to server
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        error = 1;
    }

    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr( mHost );
    clientService.sin_port = htons( mPort );

	//----------------------
    // Connect to server.
    iResult = connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) );
    if (iResult == SOCKET_ERROR) {
        wprintf(L"connect failed with error: %d\n", WSAGetLastError() );
        closesocket(ConnectSocket);
        WSACleanup();
        error = 1;
  }    
}

void TcpSocket::Close()
{
	// shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        error = 1;
    }
    closesocket(mPort);  
    WSACleanup();
}

void TcpSocket::Send(char *str)
{
	iResult = send( ConnectSocket, str, (int)strlen(str), 0 );
	if (iResult == SOCKET_ERROR) {
        wprintf(L"send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        error = 1;
    }

    printf("Bytes Sent: %d\n", iResult);
}

const char* TcpSocket::GetHost()
{
	return mHost;
}
