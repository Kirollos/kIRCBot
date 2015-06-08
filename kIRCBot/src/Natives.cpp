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
#include "Natives.h"
#include "kSquirrel.h"
#include "kIRC.h"

SQInteger kSquirrelNatives::printerror(HSQUIRRELVM v) // printerror(str);
{
	const SQChar* str;

	sq_getstring(v, 2, &str);

	kConsole::PrintError(str);
	
	sq_pushbool(v, SQTrue);
	return 1;
}

SQInteger kSquirrelNatives::printwarning(HSQUIRRELVM v) // printwarning(str);
{
	const SQChar* str;

	sq_getstring(v, 2, &str);

	kConsole::PrintWarning(str);

	sq_pushbool(v, SQTrue);
	return 1;
}

SQInteger kSquirrelNatives::printnotice(HSQUIRRELVM v) // printnotice(str);
{
	const SQChar* str;

	sq_getstring(v, 2, &str);

	kConsole::PrintNotice(str);

	sq_pushbool(v, SQTrue);
	return 1;
}

SQInteger kSquirrelNatives::printex(HSQUIRRELVM v) // printex(str, colourid);
{
#ifdef _WIN32
	const SQChar* str;
	SQInteger colourid;

	sq_getstring(v, 2, &str);
	sq_getinteger(v, 3, &colourid);

	if (colourid < 0 || colourid > 15)
	{
		kConsole::PrintError("{printex(%s, %i)}: colourid is not valid!", str, colourid);
		sq_pushbool(v, SQFalse);
		return 1;
	}

	kConsole::PrintEx((Colors)colourid, str);

	sq_pushbool(v, SQTrue);
#else
	kConsole::PrintError("{printex()}: is currently not supported on Linux.");
	sq_pushbool(v, SQFalse);
#endif
	return 1;
}

SQInteger kSquirrelNatives::exit(HSQUIRRELVM v) // exit(void);
{
	kSquirrel::GracefulClose(&v);
	return 1;
}

SQInteger kSquirrelNatives:: do_eval(HSQUIRRELVM v) /// do_eval(str);
{
	const SQChar* str;
	sq_getstring(v, 2, &str);

	sq_reseterror(v);
	
	sq_pushroottable(v);
	
	if (SQ_FAILED(sq_compilebuffer(v, str, sizeof(SQChar)*strlen(str), "program", SQFalse)))
	{
		const SQChar* retstr = kSquirrel::GetLastError(v);
		sq_pushstring(v, retstr, -1);
		return 1;
	}
	sq_pushroottable(v);
	if (SQ_FAILED(sq_call(v, 1, SQTrue, SQFalse)))
	{
		const SQChar* retstr = kSquirrel::GetLastError(v);
		sq_pushstring(v, retstr, -1);
		return 1;
	}
	else
		sq_pushbool(v, SQTrue);
	return 1;
}

SQInteger kSquirrelNatives::IRC_Connect(HSQUIRRELVM v) // IRC_Connect(host, port, nick, user, realname);
{
	const SQChar* host, *nick, *user, *realname;
	SQInteger port;

	sq_getstring(v, 2, &host);
	sq_getinteger(v, 3, &port);
	sq_getstring(v, 4, &nick);
	sq_getstring(v, 5, &user);
	sq_getstring(v, 6, &realname);

	kIRC* IRCSocket = nullptr;

	IRCSocket = new kIRC(
		std::string(host), 
		(int) port, 
		std::string(nick), 
		std::string(user), 
		std::string(realname)
		);
	IRCs.push_back(IRCSocket);
	IRCSocket->Logging = true;
	IRCSocket->Connect();

	SQInteger newid = IRCs.size();

	sq_pushinteger(v, newid);

	//printf("\n\nLA NEW ID(%i)\n\n", newid);

	// IRC_OnConnect is called when message 001 is received.

	return 1;
}

SQInteger kSquirrelNatives::IRC_Quit(HSQUIRRELVM v) // IRC_Disconnect(id, quitmsg);
{
	GETID(v, botid);
	if (IRCSock == nullptr) return 1;
	const SQChar* quitmsg;
	sq_getstring(v, 3, &quitmsg);
	IRCSock->Disconnect(std::string(quitmsg));
	kSquirrel::CallFunction(vm, "IRC_OnDisconnect", "is", botid, quitmsg); // IRC_OnDisconnect(id, quitmsg)
	//IRCs.erase(IRCs.begin()+VMIDTOREAL(botid));
	IRCs[VMIDTOREAL(botid)] = nullptr;
	//IRCs[VMIDTOREAL(botid)]->Disconnect(quitmsg);
	return 1;
}

SQInteger kSquirrelNatives::IRC_SendRaw(HSQUIRRELVM v) // IRC_SendRaw(id, raw);
{
	GETID(v, botid);
	const SQChar* raw;
	sq_getstring(v, 3, &raw);
	IRCSock->SendRaw(std::string(raw));
	return 1;
}

SQInteger kSquirrelNatives::IRC_JoinChannel(HSQUIRRELVM v) // IRC_JoinChannel(id, channel);
{
	GETID(v, botid);
	const SQChar* channel;
	sq_getstring(v, 3, &channel);
	IRCSock->SendRawEx("JOIN %s", channel);
	//kSquirrel::CallFunction(v, "IRC_OnJoin", "iss", botid, IRCSock->_nick.c_str(), channel); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_PartChannel(HSQUIRRELVM v) // IRC_PartChannel(id, channel, partmsg);
{
	GETID(v, botid);
	const SQChar* channel, *reason;
	sq_getstring(v, 3, &channel);
	sq_getstring(v, 4, &reason);
	IRCSock->SendRawEx("PART %s :%s", channel, reason);
	//kSquirrel::CallFunction(v, "IRC_OnPart", "sss", IRCSock->_nick.c_str(), channel, reason); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_ChangeNick(HSQUIRRELVM v) // IRC_ChangeNick(id, newnick);
{
	GETID(v, botid);
	const SQChar* newnick;
	sq_getstring(v, 3, &newnick);
	IRCSock->SendRawEx("NICK %s", newnick);
	//kSquirrel::CallFunction(v, "IRC_OnNickChange", "ss", IRCSock->_nick.c_str(), newnick); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_SetMode(HSQUIRRELVM v) // IRC_SetMode(id, target, modes);
{
	GETID(v, botid);
	const SQChar* target, *modes;
	sq_getstring(v, 3, &target);
	sq_getstring(v, 4, &modes);
	IRCSock->SendRawEx("MODE %s %s", target, modes);
	//kSquirrel::CallFunction(v, "IRC_OnModeChange", "sss", IRCSock->_nick.c_str(), target, modes); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_Say(HSQUIRRELVM v) // IRC_Say(id, target, message);
{
	GETID(v, botid);
	const SQChar* target, *message;
	sq_getstring(v, 3, &target);
	sq_getstring(v, 4, &message);
	IRCSock->SendRawEx("PRIVMSG %s :%s", target, message);
	//kSquirrel::CallFunction(v, "IRC_OnSay", "sss", IRCSock->_nick.c_str(), target, message); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_Notice(HSQUIRRELVM v) // IRC_Notice(id, target, message);
{
	GETID(v, botid);
	const SQChar* target, *message;
	sq_getstring(v, 3, &target);
	sq_getstring(v, 4, &message);
	IRCSock->SendRawEx("NOTICE %s :%s", target, message);
	//kSquirrel::CallFunction(v, "IRC_OnNotice", "sss", IRCSock->_nick.c_str(), target, message); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_InviteUser(HSQUIRRELVM v) // IRC_InviteUser(id, user, channel);
{
	GETID(v, botid);
	const SQChar* user, *channel;
	sq_getstring(v, 3, &user);
	sq_getstring(v, 4, &channel);
	IRCSock->SendRawEx("INVITE %s :%s", user, channel);
	//kSquirrel::CallFunction(v, "IRC_OnInvite", "sss", IRCSock->_nick.c_str(), user, channel); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_KickUser(HSQUIRRELVM v) // IRC_KickUser(id, channel, nick, reason);
{
	GETID(v, botid);
	const SQChar* channel, *nick, *reason;
	sq_getstring(v, 3, &channel);
	sq_getstring(v, 4, &nick);
	sq_getstring(v, 5, &reason);
	IRCSock->SendRawEx("KICK %s %s :%s", channel, nick, reason);
	//kSquirrel::CallFunction(v, "IRC_OnKick", "ssss", IRCSock->_nick.c_str(), channel, nick, reason); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_SetChannelTopic(HSQUIRRELVM v) // IRC_SetChannelTopic(id, channel, topic);
{
	GETID(v, botid);
	const SQChar* channel, *topic;
	sq_getstring(v, 3, &channel);
	sq_getstring(v, 4, &topic);
	IRCSock->SendRawEx("TOPIC %s :%s", channel, topic);
	//kSquirrel::CallFunction(v, "IRC_OnKick", "sss", IRCSock->_nick.c_str(), channel, topic); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_SendCTCP(HSQUIRRELVM v) // IRC_SendCTCP(id, target, message);
{
	GETID(v, botid);
	const SQChar* target, *message;
	sq_getstring(v, 3, &target);
	sq_getstring(v, 4, &message);
	IRCSock->SendRawEx("PRIVMSG %s :\x01%s\x01", target, message);
	//kSquirrel::CallFunction(v, "IRC_OnCTCPSend", "sss", IRCSock->_nick.c_str(), target, message); // Handled by Parser
	return 1;
}

SQInteger kSquirrelNatives::IRC_GetUMode(HSQUIRRELVM v) // IRC_GetUMode(id);
{
	GETID(v, botid);
	const SQChar* umodes = (const SQChar*) IRCSock->_usermode.c_str();
	sq_pushstring(v, umodes, -1);
	return 1;
}

SQInteger kSquirrelNatives::IRC_GetChMode(HSQUIRRELVM v) // IRC_GetChMode(id, channel);
{
	GETID(v, botid);
	const SQChar* channel;
	sq_getstring(v, 3, &channel);
	const SQChar* umodes = (const SQChar*)GetChanByName(IRCSock, channel)->_modes.c_str();
	sq_pushstring(v, umodes, -1);
	return 1;
}

SQInteger kSquirrelNatives::IRC_SetCommandPrefix(HSQUIRRELVM v) // IRC_SetCommandPrefix(id, prefix);
{
	GETID(v, botid);
	const SQChar* prefix;
	//char tmpprefix = NULL;
	sq_getstring(v, 3, &prefix);
	char* tmpprefix = (char*)prefix;
	//tmpprefix = (int)prefix;
	IRCSock->cmd_prefix = *tmpprefix;
	//printf("CMD PREFIX IS %c ITS ID IS %i LOLOL XD\n\n\n", IRCSock->cmd_prefix, IRCSock->cmd_prefix);
	sq_pushbool(v, SQTrue);
	return 1;
}

SQInteger kSquirrelNatives::IRC_GetChannelTopic(HSQUIRRELVM v) // IRC_GetChannelTopic(id, channel);
{
	GETID(v, botid);
	const SQChar* channel;
	sq_getstring(v, 3, &channel);
	const SQChar* rettopic = GetChanByName(IRCSock, std::string(channel))->_topic.c_str();
	sq_pushstring(v, rettopic, -1);
	return 1;
}

SQInteger kSquirrelNatives::IRC_GetUserChannelModes(HSQUIRRELVM v) // IRC_GetUserChannelModes(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	const SQChar* flags = GetChanByName(IRCSock, std::string(channel))->userlist.at(std::string(usernick)).c_str();

	sq_pushstring(v, flags, -1);
	return 1;
}

SQInteger kSquirrelNatives::IRC_IsUserOwner(HSQUIRRELVM v) // IRC_IsUserOwner(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	bool retval = GetChanByName(IRCSock, std::string(channel))->User_IsOwner(std::string(usernick));
	//sq_pushinteger(v, (SQInteger)retval);
	sq_pushbool(v, (SQBool)retval);
	return 1;
}

SQInteger kSquirrelNatives::IRC_IsUserAdmin(HSQUIRRELVM v) // IRC_IsUserAdmin(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	bool retval = GetChanByName(IRCSock, std::string(channel))->User_IsAdmin(std::string(usernick));
	//sq_pushinteger(v, (SQInteger)retval);
	sq_pushbool(v, (SQBool)retval);
	return 1;
}

SQInteger kSquirrelNatives::IRC_IsUserOp(HSQUIRRELVM v) // IRC_IsUserOp(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	bool retval = GetChanByName(IRCSock, std::string(channel))->User_IsOp(std::string(usernick));
	//sq_pushinteger(v, (SQInteger)retval);
	sq_pushbool(v, (SQBool)retval);
	return 1;
}

SQInteger kSquirrelNatives::IRC_IsUserHalfop(HSQUIRRELVM v) // IRC_IsUserHalfop(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	bool retval = GetChanByName(IRCSock, std::string(channel))->User_IsHalfop(std::string(usernick));
	//sq_pushinteger(v, (SQInteger)retval);
	sq_pushbool(v, (SQBool)retval);
	return 1;
}

SQInteger kSquirrelNatives::IRC_IsUserVoice(HSQUIRRELVM v) // IRC_IsUserVoice(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	bool retval = GetChanByName(IRCSock, std::string(channel))->User_IsVoice(std::string(usernick));
	//sq_pushinteger(v, (SQInteger)retval);
	sq_pushbool(v, (SQBool)retval);
	return 1;
}

SQInteger kSquirrelNatives::IRC_IsUserAway(HSQUIRRELVM v) // IRC_IsUserAway(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	bool retval = GetChanByName(IRCSock, std::string(channel))->User_IsAway(std::string(usernick));
	sq_pushbool(v, (SQBool)retval);
	return 1;
}

SQInteger kSquirrelNatives::IRC_IsUserOper(HSQUIRRELVM v) // IRC_IsUserOper(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	bool retval = GetChanByName(IRCSock, std::string(channel))->User_IsOper(std::string(usernick));
	sq_pushbool(v, (SQBool)retval);
	return 1;
}

SQInteger kSquirrelNatives::IRC_IsUserBot(HSQUIRRELVM v) // IRC_IsUserBot(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	bool retval = GetChanByName(IRCSock, std::string(channel))->User_IsBot(std::string(usernick));
	sq_pushbool(v, (SQBool)retval);
	return 1;
}

SQInteger kSquirrelNatives::IRC_IsUserRegistered(HSQUIRRELVM v) // IRC_IsUserRegistered(id, usernick, channel);
{
	GETID(v, botid);
	const SQChar* usernick, *channel;
	sq_getstring(v, 3, &usernick);
	sq_getstring(v, 4, &channel);

	bool retval = GetChanByName(IRCSock, std::string(channel))->User_IsRegistered(std::string(usernick));
	sq_pushbool(v, (SQBool)retval);
	return 1;
}

SQInteger kSquirrelNatives::IRC_GetChannelUserList(HSQUIRRELVM v) // userlist[] = IRC_GetChannelUserList(id, channel);
{
	GETID(v, botid);
	const SQChar* channel;
	sq_getstring(v, 3, &channel);

	////////////////////
	kIRCChan* chan = GetChanByName(IRCSock, std::string(channel));

	sq_newarray(v, 0);

	for (std::map<std::string, std::string>::iterator it = chan->userlist.begin(); it != chan->userlist.end(); it++)
	{
		sq_pushstring(v, std::string(it->second + "||" + it->first).c_str(), -1); // Had to :/
		sq_arrayappend(v, -2);
	}

	return 1;
}