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

#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
//#include <boost/thread.hpp>
#include <thread>
#include "main.h"
#include "kSock.h"

void SocketThread(kSock* Socket)
{
	if (!Socket->isConnected)
		return;
	
	std::string _data;
	while (true)
	{
		if (Socket->isConnected == false)
			break;
		int recvsize;
		while (true)
		{
			recvsize = 0;
			char r = NULL;
			if ((recvsize = recv(Socket->sockid, &r, 1, 0)) > 0)
			{
				_data += r;
			}
			else break;

			if (r == '\n')
				break;
			else
				continue;
		}
		Socket->_OnSocketRaw(_data);
		_data.clear();
		continue;
	}
	return;
}


bool kSock::socket_create(int protocol)
{
	this->isConnected = false;
#ifdef _WIN32
	DO_WSA(this->_WSADATA);
#endif
	this->sockid = socket(AF_INET, SOCK_STREAM, protocol);
	if (this->sockid == INVALID_SOCKET)
	{
#ifdef _WIN32
		WSACleanup();
#endif
		return false;
	}
	return true;
}

bool kSock::socket_connect(char* host, int port)
{
	sockaddr_in __sockaddr;
	host = inet_ntoa(*(in_addr*)gethostbyname(host)->h_addr);

	__sockaddr.sin_family = AF_INET;
	__sockaddr.sin_addr.s_addr = inet_addr(host);
	__sockaddr.sin_port = htons(port);

	if (connect(this->sockid, (sockaddr*)&__sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
#ifdef _WIN32
		WSACleanup();
#endif
		return false;
	}
	this->isConnected = true;
	//this->sockthread.add_thread(new boost::thread(this->SocketThread, this));
	//this->sockthread = new boost::thread(this->SocketThread);
	//this->sockthread = new boost::thread(SocketThread, this);
	this->sockthread = new std::thread(SocketThread, this);
	this->sockthread->detach();
	//this->async_ptr = std::async(SocketThread, this);
	return true;
}

bool kSock::socket_disconnect()
{
	this->isConnected = false;
	//char* buf = new char[3];
	int r = 0;
	//try {
	r = send(this->sockid, "quitz", strlen("quitz"), 0);
	if (r != -1)
	{
		int ret = shutdown(this->sockid, SD_BOTH/*SD_SEND*/);
		if (ret == 0) {
			this->isConnected = false;
			int isit = this->sockthread->joinable();
			if (isit)
				this->sockthread->join();
			//this->sockthread->~thread();
			delete this->sockthread;
			//this->async_ptr.wait();
			return true;
		}
		else return false;
	}
	else
	{
		this->isConnected = false;
		this->sockthread->join();
		this->sockthread->~thread();
		//this->async_ptr.wait();
		//this->async_ptr.~future();
		return true;
	}
}

//bool kSock::socket_send(const char* raw)
bool kSock::socket_send(std::string raw)
{
	if (!raw.empty()) {
		if (raw.find("\n") == std::string::npos && raw.find("\r\n") == std::string::npos)
		{
			raw.append("\n");
		}
		std::cout << ">> " << raw << std::endl;
	}
	return send(this->sockid, raw.c_str(), raw.length(), 0) != -1;
}

void kSock::socket_destroy()
{
	//closesocket(this->sockid);
	this->sockid = NULL;
#ifdef _WIN32
	WSACleanup();
#endif
	this->isConnected = false;
	return;
}

void kSock::socket_sendex(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char* formatted = new char[1024];

	vsprintf(formatted, format, args);

	this->socket_send(std::string(formatted));

	va_end(args);
	delete formatted;
}

void kSock::_OnSocketRaw(std::string raw)
{
	OnSocketRaw(this, raw/*.erase(raw.find_last_of('\r\n'))*/);
}