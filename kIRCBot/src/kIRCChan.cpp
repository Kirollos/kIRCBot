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

#include "main.h"
#include "kSock.h"
#include "kIRC.h"
#include "kIRCChan.h"
#include "kSquirrel.h"

#include <stdio.h>
#include <iostream>
//#include <regex>
#include <boost/regex.hpp>
#include <sstream>
#include <map>

kIRCChan::kIRCChan(std::string channame)
{
	this->_name = channame;
}
kIRCChan::~kIRCChan()
{
	this->_name = "";
	this->_modes = "";
	this->_topic = "";
	this->userlist.clear();
}

void kIRCChan::addNick(std::string name, std::string rank)
{
	this->userlist.insert(std::pair<std::string, std::string>(name, rank));
	return;
}

void kIRCChan::remNick(std::string name)
{
	std::map<std::string, std::string>::iterator user = this->userlist.find(name);
	if (user != this->userlist.end())
		this->userlist.erase(user);
	//this->userlist.erase(this->userlist.find(name));
	return;
}

void kIRCChan::modifyUserList(std::string name, std::string new_name, std::string new_rank)
{
	this->remNick(name);
	if (!new_name.empty())
		this->addNick(new_name, new_rank);
	else
		this->addNick(name, new_rank);
	return;
}

std::string kIRCChan::GetUserFlags(std::string name)
{
	return this->userlist.at(name);
}

bool kIRCChan::User_IsOwner(std::string name)
{
	return this->userlist.at(name).find_first_of('~') != std::string::npos;
}

bool kIRCChan::User_IsAdmin(std::string name)
{
	return this->userlist.at(name).find_first_of("&~") != std::string::npos;
}

bool kIRCChan::User_IsOp(std::string name)
{
	return this->userlist.at(name).find_first_of("@&~") != std::string::npos;
}

bool kIRCChan::User_IsHalfop(std::string name)
{
	return this->userlist.at(name).find_first_of("%@&~") != std::string::npos;
}

bool kIRCChan::User_IsVoice(std::string name)
{
	return this->userlist.at(name).find_first_of("+%@&~") != std::string::npos;
}

bool kIRCChan::User_IsAway(std::string name)
{																			/*  vvvvvvvvvvvvvvvv    not working :( vvvvvvvvvvvvvvv      */
	return (this->userlist.at(name).find_first_of('G') != std::string::npos)/* && (this->userlist.at(name).find_first_of('H') == std::string::npos)*/;
}

bool kIRCChan::User_IsOper(std::string name)
{
	return this->userlist.at(name).find_first_of('*') != std::string::npos;
}

bool kIRCChan::User_IsBot(std::string name)
{
	return this->userlist.at(name).find_first_of('B') != std::string::npos;
}

bool kIRCChan::User_IsRegistered(std::string name)
{
	return this->userlist.at(name).find_first_of('r') != std::string::npos;
}

kIRCChan* GetChanByName(kIRC* IRC, std::string channame)
{
	kIRCChan* retval = nullptr;
	for (int i = 0; i < (int) IRC->channels.size(); i++)
	{
		if (IRC->channels.at(i)->_name == channame)
		{
			retval = IRC->channels.at(i);
			break;
		}
	}
	return retval;
}