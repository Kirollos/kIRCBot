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

#pragma once
#ifndef KIRC_H_INCLUDED
#define KIRC_H_INCLUDED

#include "main.h"
#include "kSock.h"
#include "kIRCChan.h"
#include <vector>
#include <map>

void PingPong_Handler(kSock* Socket, std::string pong);
int DisconnectAllIRCs(std::string reason = "");

class kIRC
{
public:
	kSock* IRC_Sock;
	std::string _host;
	int _port;
	std::string _nick;
	std::string _user;
	std::string _realname;
	std::string _motd;
	std::string _usermode;
	char cmd_prefix;
	bool Logging = false;
	bool Registered = false;

	kIRC(std::string host, int port, std::string nick, std::string user, std::string realname);
	~kIRC();
	bool Connect();
	void Disconnect(std::string quitmsg);
	bool SendRaw(std::string raw);
	bool SendRawEx(std::string raw, ...);
	std::vector<std::string> storedRaws;
	std::vector<kIRCChan*> channels;

};

kIRC* GetIRCfromSock(kSock* Socket);
int GetIteratorFromIRC(kIRC* IRCS);
kIRCChan* GetChanByName(kIRC* IRC, std::string channame);

#define IS_NUMERIC(rawcmd)		if(boost::regex_match(rawcmd, boost::regex("^\d\d\d$")) == true)

#endif