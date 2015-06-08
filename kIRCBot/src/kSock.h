/*
Copyright 2015 Kirollos

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef KSOCK_H_INCLUDED
#define KSOCK_H_INCLUDED

#include "main.h"

#ifdef _WIN32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define DO_WSA(ref) \
	do { \
	if(WSAStartup(MAKEWORD(2,0), &ref) != 0) { \
	WSACleanup(); \
	kConsole::PrintError("WSAStartup() has failed.\r\n"); \
	exit(1); \
				}} while(false)
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
typedef int SOCKET;
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#define Sleep(x)        sleep(x/1000);
#define SD_BOTH         SHUT_RDWR
#define SOCKET_ERROR    (-1)
#define INVALID_SOCKET  (-1)
#endif
//#include <boost/thread.hpp>
#include <thread>
//#include <future>

class kSock
{
public:
#ifdef _WIN32
	WSAData _WSADATA;
#endif
	//boost::thread_group sockthread;
	//boost::thread* sockthread;
	std::thread* sockthread;
	//std::future<void> async_ptr;
	std::string storedrecv;
	SOCKET sockid = -1;
	bool isConnected;
	bool socket_create(int protocol = IPPROTO_TCP);
	bool socket_connect(char* host, int port);
	bool socket_disconnect();
	//bool socket_send(const char* raw);
	bool socket_send(std::string raw);
	void socket_destroy();
	void _OnSocketRaw(std::string raw);
	void socket_sendex(const char* format, ...);
};

void SocketThread(kSock* Socket);
void OnSocketRaw(kSock* Socket, std::string raw);
//-----------------------------------------------------------

#endif