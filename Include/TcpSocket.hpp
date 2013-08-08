#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

/**
 * A basic wrapper for an asyncronous TCP socket.
 */
class TcpSocket
{
public:
    TcpSocket(const char* host, int port);
    ~TcpSocket(void);

    void tcpSend(char *str);
	void tcpSend(string str, float value);
	void tcpSend(string str, string value);
	void tcpSend(string str, long value);
	void tcpSend(string str, unsigned char value);
	//void tcpSend(string str, const bool value);

    void open();
    void close();
	const char* getHost();
private:
    const char* mHost;
    int mPort;

	int iResult;
	int error;
	WSADATA wsaData;

	SOCKET ConnectSocket;
	struct sockaddr_in clientService; 

	std::ostringstream os;
};