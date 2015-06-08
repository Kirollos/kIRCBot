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
#ifndef KIRCCHAN_H_INCLUDED
#define KIRCCHAN_H_INCLUDED

#include "main.h"
//#include "kIRC.h"
#include <string>
//#include <vector>
#include <map>


class kIRCChan
{
public:
	kIRCChan(std::string channame);
	~kIRCChan();

	std::string _name;
	std::string _modes;
	std::string _topic;
	std::map<std::string, std::string> userlist;

	bool __WHO;

	void addNick(std::string name, std::string rank);
	void remNick(std::string name);
	void modifyUserList(std::string name, std::string new_name = "", std::string new_rank = NULL);
	std::string GetUserFlags(std::string name);
	bool User_IsOwner(std::string name);
	bool User_IsAdmin(std::string name);
	bool User_IsOp(std::string name);
	bool User_IsHalfop(std::string name);
	bool User_IsVoice(std::string name);
	bool User_IsAway(std::string name);
	bool User_IsOper(std::string name);
	bool User_IsBot(std::string name);
	bool User_IsRegistered(std::string name);
};

//kIRCChan* GetChanByName(kIRC* IRC, std::string channame); // kIRC.h

#endif