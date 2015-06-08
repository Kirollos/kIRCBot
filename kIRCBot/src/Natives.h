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

#ifndef NATIVES_H_INCLUDED
#define NATIVES_H_INCLUDED

#include "main.h"
#include "kSquirrel.h"

#define GETID(v,botid)	SQInteger botid; sq_getinteger(v, 2, &botid)
#define IRCSock			IRCs[VMIDTOREAL(botid)]

namespace kSquirrelNatives
{
	SQInteger printerror(HSQUIRRELVM v);			// printerror(str);
	SQInteger printwarning(HSQUIRRELVM v);			// printwarning(str);
	SQInteger printnotice(HSQUIRRELVM v);			// printnotice(str);
	SQInteger printex(HSQUIRRELVM v);				// printex(str, colourid);
	SQInteger exit(HSQUIRRELVM v);					// exit(void);
	SQInteger do_eval(HSQUIRRELVM v);				// do_eval(str);
	SQInteger IRC_Connect(HSQUIRRELVM v);			// IRC_Connect(host, port, nick, user, realname); // Returns an unique bot ID
	SQInteger IRC_Quit(HSQUIRRELVM v);				// IRC_Quit(id, quitmsg);
	SQInteger IRC_SendRaw(HSQUIRRELVM v);			// IRC_SendRaw(id, raw);
	SQInteger IRC_JoinChannel(HSQUIRRELVM v);		// IRC_JoinChannel(id, channel);
	SQInteger IRC_PartChannel(HSQUIRRELVM v);		// IRC_PartChannel(id, channel, partmsg);
	SQInteger IRC_ChangeNick(HSQUIRRELVM v);		// IRC_ChangeNick(id, newnick);
	SQInteger IRC_SetMode(HSQUIRRELVM v);			// IRC_SetMode(id, target, modes);
	SQInteger IRC_Say(HSQUIRRELVM v);				// IRC_Say(id, target, message);
	SQInteger IRC_Notice(HSQUIRRELVM v);			// IRC_Notice(id, target, message);
	SQInteger IRC_InviteUser(HSQUIRRELVM v);		// IRC_InviteUser(id, user, channel);
	SQInteger IRC_KickUser(HSQUIRRELVM v);			// IRC_KickUser(id, channel, nick, reason);
	SQInteger IRC_SetChannelTopic(HSQUIRRELVM v);	// IRC_SetChannelTopic(id, channel, topic);
	SQInteger IRC_SendCTCP(HSQUIRRELVM v);			// IRC_SendCTCP(id, target, message);
	SQInteger IRC_GetUMode(HSQUIRRELVM v);			// IRC_GetUMode(id); // Returns a string of bot's mode
	SQInteger IRC_GetChMode(HSQUIRRELVM v);			// IRC_GetChMode(id, channel); // Returns a string of a channel modes
	SQInteger IRC_SetCommandPrefix(HSQUIRRELVM v);	// IRC_SetCommandPrefix(id, prefix);
	SQInteger IRC_GetChannelTopic(HSQUIRRELVM v);	// IRC_GetChannelTopic(id, channel); // Retunrs a string of current channel topic
	SQInteger IRC_GetUserChannelModes(HSQUIRRELVM v);// IRC_GetUserChannelModes(id, usernick, channel); // Returns a string of modes of a user in a channel
	SQInteger IRC_IsUserOwner(HSQUIRRELVM v);		// IRC_IsUserOwner(id, usernick, channel); // Returns a boolean
	SQInteger IRC_IsUserAdmin(HSQUIRRELVM v);		// IRC_IsUserAdmin(id, usernick, channel); // Returns a boolean
	SQInteger IRC_IsUserOp(HSQUIRRELVM v);			// IRC_IsUserOp(id, usernick, channel); // Returns a boolean
	SQInteger IRC_IsUserHalfop(HSQUIRRELVM v);		// IRC_IsUserHalfop(id, usernick, channel); // Returns a boolean
	SQInteger IRC_IsUserVoice(HSQUIRRELVM v);		// IRC_IsUserVoice(id, usernick, channel); // Returns a boolean
	SQInteger IRC_IsUserAway(HSQUIRRELVM v);		// IRC_IsUserAway(id, usernick, channel); // Returns a boolean
	SQInteger IRC_IsUserOper(HSQUIRRELVM v);		// IRC_IsUserOper(id, usernick, channel); // Returns a boolean
	SQInteger IRC_IsUserBot(HSQUIRRELVM v);			// IRC_IsUserBot(id, usernick, channel); // Returns a boolean
	SQInteger IRC_IsUserRegistered(HSQUIRRELVM v);	// IRC_IsUserRegistered(id, usernick, channel); // Returns a boolean
	SQInteger IRC_GetChannelUserList(HSQUIRRELVM v);// IRC_GetChannelUserList(id, channel); // Returns an array of userlist
}

#endif