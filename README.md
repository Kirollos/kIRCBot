kIRCBot
========

This is a project I've written in C++ in my free time.
Basically, It's an IRC Bot giving the user the ability to load [Squirrel-lang] (squirrel-lang.org) scripts to use it.

**Note: This project is still under development. I'm working on this in my free time as well.
**Documentation will be added soon.

Features
=========

+ Control your bot with squirrel-lang with a lot of standard and IRC functions
+ Supports both Windows and Linux (not tested on *BSD or other *NIX systems)
+ Create more than one bot through one script and one runtime

Installation
=============

Windows
========

Open the solution file of Visual Studio 2013 and compile **Squirrel** and **sqstdlib** and copy the .lib files to kIRCBot\lib\.
Then compile project **kIRCBot**.

Linux
======

Run ``make`` in **Squirrel/** and **sqstdlib/** then ``make Debug`` or ``make Release`` in **kIRCBot/** to compile the application (or ``make all`` for both Debug and Release).

**Note:** This project uses [Boost] (http://boost.org) for regex.

Usage
=====

After compiling, run **kIRCBot.exe** (Windows) or **bin/kIRCBot** (Linux)
then use ``loadscript <path/to/script.nut>`` to load your script
**OR**: run ``kIRCBot.exe <path/to/script.nut>`` (Windows) or ``kIRCBot <path/to/script.nut>`` (Linux) to directly load your script

Functions
==========
```squirrel
printerror(str);
printwarning(str);
printnotice(str);
printex(str, colourid);
exit(void);
do_eval(str);
IRC_Connect(host, port, nick, user, realname); // Returns an unique bot ID
IRC_Quit(id, quitmsg);
IRC_SendRaw(id, raw);
IRC_JoinChannel(id, channel);
IRC_PartChannel(id, channel, partmsg);
IRC_ChangeNick(id, newnick);
IRC_SetMode(id, target, modes);
IRC_Say(id, target, message);
IRC_Notice(id, target, message);
IRC_InviteUser(id, user, channel);
IRC_KickUser(id, channel, nick, reason);
IRC_SetChannelTopic(id, channel, topic);
IRC_SendCTCP(id, target, message);
IRC_GetUMode(id); // Returns a string of bot's mode
IRC_GetChMode(id, channel); // Returns a string of a channel modes
IRC_SetCommandPrefix(id, prefix);
IRC_GetChannelTopic(id, channel); // Retunrs a string of current channel topic
IRC_GetUserChannelModes(id, usernick, channel); // Returns a string of modes of a user in a channel
IRC_IsUserOwner(id, usernick, channel); // Returns a boolean
IRC_IsUserAdmin(id, usernick, channel); // Returns a boolean
IRC_IsUserOp(id, usernick, channel); // Returns a boolean
IRC_IsUserHalfop(id, usernick, channel); // Returns a boolean
IRC_IsUserVoice(id, usernick, channel); // Returns a boolean
IRC_IsUserAway(id, usernick, channel); // Returns a boolean
IRC_IsUserOper(id, usernick, channel); // Returns a boolean
IRC_IsUserBot(id, usernick, channel); // Returns a boolean
IRC_IsUserRegistered(id, usernick, channel); // Returns a boolean
IRC_GetChannelUserList(id, channel); // Returns an array of userlist
```

Callback Functions
==================

```squirrel
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
```