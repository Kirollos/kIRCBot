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
#include "kIRC.h"
#include "kSock.h"
#include "kIRCChan.h"
#include "kSquirrel.h"

#include <stdio.h>
#include <iostream>
#include <algorithm>
//#include <regex>
#include <boost/regex.hpp>
#include <sstream>
#include <map>

kIRC::kIRC(std::string host, int port, std::string nick, std::string user, std::string realname)
{
	IRC_Sock = new kSock();
	IRC_Sock->socket_create();
	this->_host = host;
	this->_port = port;
	this->_nick = nick;
	this->_user = user;
	this->_realname = realname;
	this->cmd_prefix = '!';
	/*IRC_Sock->socket_connect((char*) host.c_str(), port);
	IRC_Sock->socket_sendex("NICK %s", nick.c_str());
	IRC_Sock->socket_sendex("USER %s - - :%s", user.c_str(), realname.c_str());
	//tmp
	Sleep(5000);
	IRC_Sock->socket_send("JOIN #xcnr");*/
}
kIRC::~kIRC()
{
	this->Registered = false;
	IRC_Sock->isConnected = false;
	IRC_Sock->socket_disconnect();
	IRC_Sock->socket_destroy();
}

bool kIRC::Connect()
{
	IRC_Sock->socket_connect((char*)this->_host.c_str(), this->_port);
	IRC_Sock->socket_sendex("NICK %s", this->_nick.c_str());
	IRC_Sock->socket_sendex("USER %s - - :%s", this->_user.c_str(), this->_realname.c_str());
	return true;
}

void kIRC::Disconnect(std::string quitmsg)
{
	if (!this->IRC_Sock->isConnected) return;
	IRC_Sock->socket_sendex("QUIT :%s", quitmsg.c_str());
	this->~kIRC();
}

bool kIRC::SendRaw(std::string raw)
{
	//static std::vector<std::string> storedRaws; // It is now in the class definitions
	if (this->Registered == false)
	{
		storedRaws.push_back(raw);
		return false;
	}
	if (this->Registered == true)
	{
		while (!storedRaws.empty())
		{
			//MessageBox(0, storedRaws.begin()->c_str(), "Sent", 0);
			IRC_Sock->socket_send(std::string(storedRaws.begin()->c_str()));
			storedRaws.erase(storedRaws.begin());
		}
	}
	return IRC_Sock->socket_send(raw);
}

bool kIRC::SendRawEx(std::string raw, ...)
{
	va_list args;
	char* format = (char*) raw.c_str();
	va_start(args, raw);

	char* formatted = new char[1024];

	vsprintf(formatted, format, args);

	bool retval = this->SendRaw(std::string(formatted));

	va_end(args);
	//delete format;
	delete formatted;
	return retval;
}

void OnSocketRaw(kSock* Socket, std::string raw)
{
	kIRC* IRCSock = GetIRCfromSock(Socket);
	if (IRCSock == nullptr) return;
	if (raw.empty()) return;
	raw.erase(raw.find_last_of("\r\n", raw.length()-2));
	if (IRCSock->Logging == true)
		std::cout << "<< " << raw << std::endl;
	if (raw.substr(0, 6) == "ERROR ")
	{
		//IRCSock->Disconnect("");
		IRCSock->~kIRC();
		//IRCs[GetIteratorFromIRC(IRCSock)] = nullptr;
		return;
	}
	if (raw.substr(0, 6) == "PING :")
	{
		std::string pong = raw.substr(6, raw.length() - 6);
		PingPong_Handler(Socket, pong);

		for (int i = 0; i < (int) IRCSock->channels.size(); i++)
		{
			if (IRCSock->channels[i] == nullptr) continue;

			IRCSock->SendRawEx("WHO %s", IRCSock->channels[i]->_name.c_str());
		}
	}
	
	//IRCSock->Registered = true; // Temp

	// ^(:(\S+) )?(\S+)( (?!:)(.+?))?( :(.+))?$ (http://calebdelnay.com/blog/2010/11/parsing-the-irc-message-format-as-a-client)

	boost::regex regex("^(:(\\S+) )?(\\S+)( (?!:)(.+?))?( :(.+))?$");
	boost::smatch results;
	boost::regex_match(raw, results, regex);

	std::string prefix, command, params, trail;
	std::stringstream MAHRESULTS;

	prefix = results[2];
	command = results[3];
	params = results[5];
	trail = results[7];

#define ISNUMERIC(id)		if(boost::regex_match(id, boost::regex("^\\d\\d\\d$")))
#define ISCMD(id)			if(command == id)
	//MessageBox(0, command.c_str(), "lel", 0);
	ISNUMERIC(command)
	{
		ISCMD("001")
		{
			IRCSock->Registered = true;
			// IRC_OnConnect(id, host, port)
			kSquirrel::CallFunction(vm, "IRC_OnConnect", "isi", REALIDTOVM(GetIteratorFromIRC(IRCSock)), IRCSock->_host.c_str(), IRCSock->_port);
			IRCSock->SendRaw(""); // To push the stored raw messages sent before registering (if any)
		}
		ISCMD("002")
		{
		}
		ISCMD("003")
		{
		}
		ISCMD("004")
		{
		}
		ISCMD("005")
		{
		}
		ISCMD("375") // MOTD Start
		{
			// IRC_OnMOTDStart(id)
			kSquirrel::CallFunction(vm, "IRC_OnMOTDStart", "i", REALIDTOVM(GetIteratorFromIRC(IRCSock)));
		}
		ISCMD("372") // MOTD
		{
			IRCSock->_motd += "\n";
			IRCSock->_motd += trail;
			// IRC_OnMOTDMessage(id, message)
			kSquirrel::CallFunction(vm, "IRC_OnMOTDMessage", "is", REALIDTOVM(GetIteratorFromIRC(IRCSock)), trail.c_str());
		}
		ISCMD("376") // MOTD End
		{
			// IRC_OnMOTDEnds(id, fullmotd)
			kSquirrel::CallFunction(vm, "IRC_OnMOTDEnds", "is", REALIDTOVM(GetIteratorFromIRC(IRCSock)), IRCSock->_motd.c_str());
		}
		ISCMD("353") // Names list // Not using this anymore, using /WHO instead.
		{/*
			std::istringstream param(trail);
			std::istream_iterator<std::string> linebeg(param), lineend;
			std::vector<std::string> names(linebeg, lineend);

			boost::smatch sm;
			boost::regex_match(params, sm, boost::regex("^(\\S+) \\S (\\S+)$"));
			std::string
				nick = sm[1],
				chan = sm[2]
				;

			kIRCChan* theChan = GetChanByName(IRCSock, chan);

			for (std::string pnick : names)
			{
				char prank = NULL;
				switch (pnick[0])				// not working wtf
				{
				case '~':
				case '&':
				case '@':
				case '%':
				case '+':
					prank = pnick[0];
				default:
					prank = NULL;
				}//----------------------------------------------------
				if (
					pnick[0] == '~' ||
					pnick[0] == '&' ||
					pnick[0] == '@' ||
					pnick[0] == '%' ||
					pnick[0] == '+'
					)
				{
					prank = pnick[0];
					pnick.replace(0, 1, ""); // Tries this. Worked.
				}
				else
				{
					prank = NULL;
				}
				//pnick.erase(0); // Apparently erases whole string
				//pnick.replace(0, 1, ""); // Tries this. Worked. // WRONG PLACE LOL
				theChan->addNick(pnick, prank);
			}
			*/
		}
		ISCMD("352") // WHO
		{
			boost::smatch sm;
			boost::regex_match(params, sm, boost::regex("\\S+ (#\\S+) \\S+ \\S+ \\S+ (\\S+) (\\S+)"));
			std::string chan = sm[1];
			std::string pnick = sm[2];
			std::string flags = sm[3];

			kIRCChan* theChan = GetChanByName(IRCSock, chan);

			if (!theChan->__WHO)
			{
				theChan->__WHO = true;
				theChan->userlist.clear();
			}			
			theChan->addNick(pnick, flags);

		}
		ISCMD("315") // End of WHO
		{
			boost::smatch sm;
			boost::regex_match(params, sm, boost::regex("^\\S+ (#\\S+)$"));
			std::string chan = sm[1];

			GetChanByName(IRCSock, chan)->__WHO = false;
		}
		ISCMD("324") // Channel Modes
		{
			std::istringstream param(params);
			std::istream_iterator<std::string> linebeg(param), lineend;
			std::vector<std::string> paramm(linebeg, lineend);

			kIRCChan* theChan = GetChanByName(IRCSock, paramm[1]);

			theChan->_modes = paramm[2];

		}
		ISCMD("332")
		{
			boost::smatch sm;
			boost::regex_match(params, sm, boost::regex("^\\S+ (#\\S+)$"));
			std::string chan = sm[1];
			std::string topic = trail;

			GetChanByName(IRCSock, chan)->_topic = trail;
		}
	}
	else
	{
		ISCMD("PRIVMSG")
		{
			boost::smatch sm;
			boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
			std::string nick = sm[1];
			std::string nhost = sm[3];
			// IRC_OnPrivMsg(id, recipient, usernick, message)
			kSquirrel::CallFunction(vm, "IRC_OnPrivMsg", "isss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), params.c_str(), nick.c_str(), trail.c_str());

			if (trail[0] == IRCSock->cmd_prefix)
			{
				try
				{
					//trail.erase(0);
					std::string command = trail.substr(1, trail.find_first_of(' ')-1);
					std::string parameters = trail.substr(trail.find_first_of(' ') + 1, trail.length() - command.length() - 1);
					
					std::string command_func = "IRCCMD_" + command;
					// IRCCMD_{command}(id, recipient, usernick, userhost, params)
					kSquirrel::CallFunction(vm, command_func.c_str(), "issss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), params.c_str(), nick.c_str(), nhost.c_str(), parameters.c_str());
				}
				catch (std::exception e)
				{

				}
			}
		}
		ISCMD("NOTICE")
		{
			boost::smatch sm;
			boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
			std::string nick = sm[1];
			// IRC_OnNotice(id, recipient, usernick, message)
			kSquirrel::CallFunction(vm, "IRC_OnNotice", "isss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), params.c_str(), nick.c_str(), trail.c_str());
		}
		ISCMD("JOIN")
		{
			boost::smatch sm;
			boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
			std::string nick = sm[1];
			if (nick == IRCSock->_nick)
			{
				kIRCChan* newchan = new kIRCChan(trail);
				IRCSock->channels.push_back(newchan);
				IRCSock->SendRawEx("WHO %s", trail.c_str());
				IRCSock->SendRawEx("MODE %s", trail.c_str());
			}
			else {
				//kIRCChan* chan = GetChanByName(IRCSock, trail);
				//chan->addNick(nick, NULL);
				IRCSock->SendRawEx("WHO %s", trail.c_str());
			}
			// IRC_OnJoin(id, usernick, channel)
			kSquirrel::CallFunction(vm, "IRC_OnJoin", "iss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), nick.c_str(), trail.c_str());
		}
		ISCMD("PART")
		{
			boost::smatch sm;
			boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
			std::string nick = sm[1];
			kIRCChan* chan = GetChanByName(IRCSock, params);
			if (nick == IRCSock->_nick)
			{
				IRCSock->channels.erase(std::find(IRCSock->channels.begin(), IRCSock->channels.end(), chan));
				chan->~kIRCChan();
			}
			else {
				chan->remNick(nick);
			}
			// IRC_OnPart(id, usernick, channel, reason)
			kSquirrel::CallFunction(vm, "IRC_OnPart", "isss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), nick.c_str(), params.c_str(), trail.c_str());
		}
		ISCMD("TOPIC")
		{
			boost::smatch sm;
			boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
			std::string nick = sm[1];
			kIRCChan* chan = GetChanByName(IRCSock, params);
			chan->_topic = trail;
			// IRC_OnTopicChange(id, channel, usernick, newtopic)
			kSquirrel::CallFunction(vm, "IRC_OnTopicChange", "isss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), params.c_str(), nick.c_str(), trail.c_str());
		}
		ISCMD("MODE")
		{
			if (!trail.empty()) // It's a user mode
			{
				/* // wtf not needed lol
				boost::smatch sm;
				std::string nick;
				if (boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)")))
					nick = sm[1];
				else
					nick = prefix; // UnrealIRCd only shows nick in usermode :(
				*/
				// modes are set as trails
				IRCSock->_usermode = trail;
				// IRC_OnUserMode(id, modes)
				kSquirrel::CallFunction(vm, "IRC_OnUserMode", "is", REALIDTOVM(GetIteratorFromIRC(IRCSock)), trail.c_str());

			}
			else
			{
				boost::smatch sm;
				boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
				std::string nick = sm[1];

				std::istringstream param(params);
				std::istream_iterator<std::string> linebeg(param), lineend;
				std::vector<std::string> lesparams(linebeg, lineend);

				std::string chan = lesparams[0];
				// calling MODE #chan to refill the kIRCChan chanmodes
				IRCSock->SendRawEx("MODE %s", chan.c_str());
				// ----
				// calling WHO #chan to refill user list with their new modes
				IRCSock->SendRawEx("WHO %s", chan.c_str());
				
				// Calling callback
				std::string modestr = params;
				modestr.replace(0, chan.length()+1, "");
				// IRC_OnChannelMode(id, usernick, channel, modes)
				kSquirrel::CallFunction(vm, "IRC_OnChannelMode", "isss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), nick.c_str(), chan.c_str(), modestr.c_str());
				// -----
			}
		}
		ISCMD("NICK")
		{
			boost::smatch sm;
			boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
			std::string nick = sm[1];
			if (nick == IRCSock->_nick)
				IRCSock->_nick = trail;
			// IRC_OnUserNickChange(id, oldnick, newnick)
			kSquirrel::CallFunction(vm, "IRC_OnUserNickChange", "iss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), nick.c_str(), trail.c_str());
		}
		ISCMD("KICK")
		{
			boost::smatch sm;
			boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
			std::string nick = sm[1];
			std::string channel = params.substr(0, params.find(' '));
			std::string kicked = params.substr(params.find(' ')+1);
			// IRC_OnUserKickedFromChannel(id, usernick, channel, kickednick, reason)
			kSquirrel::CallFunction(vm, "IRC_OnUserKickedFromChannel", "issss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), nick.c_str(), channel.c_str(), kicked.c_str(), trail.c_str());
		}
		ISCMD("INVITE")
		{
			boost::smatch sm;
			boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
			std::string nick = sm[1];
			// IRC_OnUserInvite(id, usernick, invitednick, channel)
			kSquirrel::CallFunction(vm, "IRC_OnUserInvite", "isss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), nick.c_str(), params.c_str(), trail.c_str());
		}
		ISCMD("QUIT")
		{
			boost::smatch sm;
			boost::regex_match(prefix, sm, boost::regex("^(.+)!(.+)@(.+)"));
			std::string nick = sm[1];
			if (nick != IRCSock->_nick)
			// IRC_OnQuit(id, usernick, quitmsg)
			kSquirrel::CallFunction(vm, "IRC_OnQuit", "iss", REALIDTOVM(GetIteratorFromIRC(IRCSock)), nick.c_str(), trail.c_str());
		}
	}
	// IRC_OnRaw(raw)
	kSquirrel::CallFunction(vm, "IRC_OnRaw", "s", raw.c_str());
}

void PingPong_Handler(kSock* Socket, std::string pong)
{
	if (Socket->isConnected == true)
	{
		Socket->socket_sendex("PONG :%s", pong.c_str());
	}
}

int DisconnectAllIRCs(std::string reason)
{
	int num = 0;
	for (size_t i = 0; i < IRCs.size(); i++)
	{
		if (IRCs[i] == nullptr)continue;
		IRCs[i]->Disconnect(reason);
		num++;
	}
	IRCs.clear();
	return num;
}

kIRC* GetIRCfromSock(kSock* Socket)
{
	kIRC* retval = nullptr;
	for (size_t i = 0; i < IRCs.size(); i++)
	{
		if (IRCs[i] != NULL && IRCs[i]->IRC_Sock == Socket)
		{
			retval = IRCs[i];
			break;
		}
	}
	return retval;
}

int GetIteratorFromIRC(kIRC* IRCS)
{
	int ret = -1;
	for (int i = 0; i < (int) IRCs.size(); i++)
	{
		if (IRCs[i] == IRCS)
		{
			ret = i;
			break;
		}
	}
	return ret;
}