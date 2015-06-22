/*
This script is an example, It tests many functions of this application
*/

local irc = 0; // Freenode
local irc2 = 0; // Rizon

local freenode = 
[
	"irc.freenode.net",
	6667,
	"kIRCBot",
	"squirrel",
	"https://github.com/Kirollos/kIRCBot"
];
local rizon =
[
	"irc.rizon.net",
	6667,
	"kIRCBot",
	"squirrel",
	"https://github.com/Kirollos/kIRCBot"
];

function OnScriptInit()
{
	print("OnScriptInit() is called");
	irc = IRC_Connect(freenode[0], freenode[1], freenode[2], freenode[3], freenode[4]);
	irc2 = IRC_Connect(rizon[0], rizon[1], rizon[2], rizon[3], rizon[4]);
	IRC_SetCommandPrefix(irc, "!");
	IRC_SetCommandPrefix(irc2, "@");
	return 1;
}

function OnScriptExit()
{
	IRC_Quit(irc, "obai");
	IRC_Quit(irc2, "obai");
	return 1;
}

function IRC_OnConnect(id, host, port)
{
	switch(id) // Same thing I know, but trying out switch
	{
		case irc:
			IRC_JoinChannel(id, "#Kiro");
			IRC_Say(id, "#Kiro", "Hi! I am connected to " + host);
			break;
		case irc2:
			IRC_JoinChannel(id, "#Kiro");
			IRC_Say(id, "#Kiro", "Hi!! I am connected to " + host);
			break;
	}
	return 1;
}

function IRC_OnPrivMsg(id, recipient, usernick, message)
{
	if(usernick != freenode[2] && usernick != rizon[2])
		IRC_Say(id, recipient, usernick+" said: "+message);
	return 1;
}

function IRC_OnNotice(id, recipient, usernick, message)
{
	if(usernick != freenode[2] && usernick != rizon[2])
		IRC_Say(id, recipient, usernick+" noticed: "+message);
	return 1;
}

function IRC_OnJoin(id, usernick, channel)
{
	if(usernick != freenode[2] && usernick != rizon[2])
		IRC_Say(id, channel, usernick+" joined: "+channel);
	return 1;
}

function IRC_OnPart(id, usernick, channel, reason)
{
	if(usernick != freenode[2] && usernick != rizon[2])
		IRC_Say(id, channel, usernick+" parted: "+channel);
	return 1;
}

function IRC_OnTopicChange(id, channel, usernick, newtopic)
{
	IRC_Say(id, channel, usernick+" changed topic of channel "+channel+" to:"+newtopic);
	return 1;
}

function IRC_OnChannelMode(id, usernick, channel, modes)
{
	IRC_Say(id, channel, usernick+" has altered channel mode to :"+modes);
	IRC_Say(id, channel, "Current channel mode for "+channel+" is :"+IRC_GetChMode(id, channel));
	return 1;
}

function IRC_OnUserMode(id, modes)
{
	IRC_Say(id, "#Kiro", "HY I CHANGED MY MODE :"+modes);
	return 1;
}

function IRCCMD_hi(id, recipient, usernick, userhost, params)
{
	IRC_Say(id, recipient, "Someone has just said hi! usernick:"+usernick+"|userhost:"+userhost+"|params:"+params+"|");
	return 1;
}

function IRCCMD_testcolours(id, recipient, usernick, userhost, params)
{
	printerror("test error");
	printwarning("test warning");
	printnotice("test notice");
	for(local i = -5; i < 20; i++)
	{
		printex("testing colours #"+i, i); // Does not work on linux for now
	}
	return 1;
}

function IRCCMD_disconnect(id, recipient, usernick, userhost, params)
{
	IRC_Say(id, recipient, "I'm gone, bye :(");
	IRC_Quit(id, "Quit by "+usernick+" :(");
	return 1;
}

function IRCCMD_eval(id, recipient, usernick, userhost, params)
{
	/*if(usernick != "Kirollos")
	{
		IRC_Say(id, recipient, "No.");
		return 1;
	}*/
	
	local ret = do_eval(params);
	if(ret != true)
	{
		IRC_Say(id, recipient, "Error: "+ret);
	}
	return 1;
}

function IRCCMD_userlist(id, recipient, usernick, userhost, params)
{
	local ret = IRC_GetChannelUserList(id, params);
	
	for(local i = 0; i < ret.len(); i++)
	{
		IRC_Say(id, params, ret[i]);
	}
	return 1;
}

/*

// Native Functions:
// ==================

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

// Callback functions:
// ====================

function OnScriptInit()
function OnScriptExit()
function IRC_OnConnect(id, host, port)
function IRC_OnDisconnect(id, quitmsg)
function IRC_OnMOTDStart(id)
function IRC_OnMOTDMessage(id, message)
function IRC_OnMOTDEnds(id, fullmotd)
function IRC_OnPrivMsg(id, recipient, usernick, message)
function IRC_OnNotice(id, recipient, usernick, message)
function IRC_OnJoin(id, usernick, channel)
function IRC_OnPart(id, usernick, channel, reason)
function IRC_OnTopicChange(id, channel, usernick, newtopic)
function IRC_OnUserNickChange(id, oldnick, newnick)
function IRC_OnUserKickedFromChannel(id, usernick, channel, kickednick, reason)
function IRC_OnUserInvite(id, usernick, invitednick, channel)
function IRC_OnChannelMode(id, usernick, channel, modes)
function IRC_OnUserMode(id, modes)
function IRC_OnQuit(id, usernick, quitmsg)
function IRC_OnRaw(raw)
function IRCCMD_{command}(id, recipient, usernick, userhost, params)
function OnConsoleCommand(command, params) // Doesn't get called when the default commands are executed.

*/