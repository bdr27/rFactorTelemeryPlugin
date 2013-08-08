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
	void tcpSend(string str, float number);
	void tcpSend(string str, string value);
	void tcpSend(string str, long number);

    void open();
    void close();
	const char* getHost();
private:
	string addFloatString(string str, float value);
	string addStringtoString(string str, string value);
	string addLongToString(string str, long value);
    const char* mHost;
    int mPort;

	int iResult;
	int error;
	WSADATA wsaData;

	SOCKET ConnectSocket;
	struct sockaddr_in clientService; 

	std::ostringstream os;
};