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

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#endif
//#include <regex>
#include <boost/regex.hpp>

//#include "main.h"
#include "kSock.h"
#include "kSquirrel.h"
#include "kIRC.h"
#include "kIRCChan.h"
//#include "Natives.h"

//#include "myregex.h"

#include <boost/regex.hpp>

#ifdef _DEBUG
#ifdef _WIN32
#define PAUSE std::system("PAUSE")
#else
#define PAUSE printf("\nPress any key to continue..."); getchar()
#endif
#else
#define PAUSE
#endif

std::vector<kIRC*> IRCs;
HSQUIRRELVM vm = NULL;
#ifdef _WIN32
HANDLE hConsole = NULL;
#endif

int main(int argc, char** argv)
{
#ifdef _WIN32
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	SetColour(BWhite);
	
	//std::cout << "Welcome to kIRCBot\r\n\r\nUse loadscript [script name] to load your squirrel script!\r\n\r\n";
	std::cout
		<< "Welcome to kIRCBot" << std::endl
		<< "Currently running version " << K_VERSION << std::endl
		<< "Use loadscript [script name] to load your squirrel script." << std::endl
		<< "Use unloadscript [script name] to unload your squirrel script." << std::endl
		<< "Use raw [bot id] [raw message] to send direct RAW message to the IRC server." << std::endl
		<< "Use eval [functions] to send an eval to the squirrel virtual machine."
		<< std::endl << std::endl
	;
	std::string line;

	do{

		if (argv[1] != nullptr && !std::string(argv[1]).empty())
		{
			kConsole::PrintNotice("Detected argument #1 \"%s\", using that as the Squirrel script.\r\n\r\n", argv[1]);

			std::string arg(argv[1]);

			if (arg.find(".nut") == std::string::npos)
				arg.append(".nut");

			if (fopen(arg.c_str(), "r") == NULL)
			{
				kConsole::PrintError("%s does not exist!\r\n", arg.c_str());
				break;
			}

			kConsole::PrintNotice("Using %s - %s\r\n\r\n", SQUIRREL_VERSION, SQUIRREL_COPYRIGHT);

			kSquirrel::Init(&vm);

			if (!kSquirrel::RegisterStds(vm))
			{
				kConsole::PrintError("Failed to register std functions.\r\n");
				kSquirrel::Close(&vm);
				break;
			}

			kSquirrel::RegisterNatives(vm);
			kConsole::PrintNotice("Registered Squirrel Native functions.\r\n");

			//if (params[1].find(".nut") == std::string::npos)
			//	params[1].append(".nut");

			kConsole::PrintNotice("Loading script (%s)....\r\n", arg.c_str());
			if (!kSquirrel::LoadScript(vm, (const SQChar*)arg.c_str()))
			{
				kConsole::PrintError("Failed to load script. (%s)\r\n\r\n", kSquirrel::GetLastError(vm));
				kSquirrel::Close(&vm);
				break;
			}

			kSquirrel::CallFunction(vm, "OnScriptInit");
			break;
		}
	} while (false);

	while (true)
	{
		std::cout << "> ";

		std::getline(std::cin, line);
		if (line.empty())
			continue;

		std::istringstream command(line);
		std::istream_iterator<std::string> linebeg(command), lineend;

		std::vector<std::string> params(linebeg, lineend);

		if (params[0] == "exit")
		{
			if (vm != NULL)
			//	kSquirrel::CallFunction(vm, "OnScriptExit");
				kSquirrel::GracefulClose(&vm);
			kConsole::PrintWarning("Force closed %i connections.",
			DisconnectAllIRCs("Application Exited.")
			);

			kConsole::PrintWarning("Exitting application in 5 seconds!\r\n");
			Sleep(5000);
			//if (vm != NULL)
			//	kSquirrel::Close(&vm);
			break;
		}
		else if (params[0] == "loadscript")
		{
			if (params.size() != 2 || params[1].empty())
			{
				kConsole::PrintWarning("Syntax: loadscript [filename]\r\n");
				continue;
			}

			if (vm != NULL)
			{
				kConsole::PrintError("A script is already loaded.\r\n");
				continue;
			}

			if (params[1].find(".nut") == std::string::npos)
				params[1].append(".nut");

			if (fopen(params[1].c_str(), "r") == NULL)
			{
				kConsole::PrintError("%s does not exist!\r\n", params[1].c_str());
				continue;
			}

			kConsole::PrintNotice("Using %s - %s\r\n\r\n", SQUIRREL_VERSION, SQUIRREL_COPYRIGHT);
			
			kSquirrel::Init(&vm);

			if (!kSquirrel::RegisterStds(vm))
			{
				kConsole::PrintError("Failed to register std functions.\r\n");
				kSquirrel::Close(&vm);
				continue;
			}

			kSquirrel::RegisterNatives(vm);
			kConsole::PrintNotice("Registered Squirrel Native functions.\r\n");

			//if (params[1].find(".nut") == std::string::npos)
			//	params[1].append(".nut");

			kConsole::PrintNotice("Loading script (%s)....\r\n", params[1].c_str());
			if (!kSquirrel::LoadScript(vm, (const SQChar*) params[1].c_str()))
			{
				kConsole::PrintError("Failed to load script. (%s)\r\n\r\n", kSquirrel::GetLastError(vm));
				kSquirrel::Close(&vm);
				break;
			}

			kSquirrel::CallFunction(vm, "OnScriptInit");
		}
		else if (params[0] == "unloadscript")
		{
			if (params.size() != 2 || params[1].empty())
			{
				kConsole::PrintWarning("Syntax: unloadscript [filename]\r\n");
				continue;
			}

			if (params[1].find(".nut") == std::string::npos)
				params[1].append(".nut");

			if (fopen(params[1].c_str(), "r") == NULL)
			{
				kConsole::PrintError("%s does not exist!\r\n", params[1].c_str());
				continue;
			}

			if (vm == NULL)
			{
				kConsole::PrintError("No script was loaded.\r\n");
				continue;
			}

			//kSquirrel::CallFunction(vm, "OnScriptExit");	// GracefulClose does the job
			//Sleep(500);									// GracefulClose does the job
			kSquirrel::GracefulClose(&vm);
			
			continue;

		}
		else if (params[0] == "raw")
		{
			if (params.size() < 3)
			{
				kConsole::PrintWarning("Syntax: raw [bot id] [raw message]\r\n");
				continue;
			}

			int botid = atoi(params[1].c_str());
			std::string raw;
			for (int i = 2; i < (int)params.size(); i ++)
			{
				if (!params[i].empty())
				{
					raw += " ";
					raw += params[i];
				}
			}
			kIRC* raw_irc;
			try{
				 raw_irc = IRCs.at(VMIDTOREAL(botid));
			}
			catch (...)
			{
				kConsole::PrintError("botid does not exist.\r\n");
				continue;
			}

			if (raw_irc == nullptr)
			{
				kConsole::PrintError("botid does not exist.\r\n");
				continue;
			}
			raw_irc->SendRaw(raw);
		}
		else if (params[0] == "eval")
		{
			if (params.size() < 2)
			{
				kConsole::PrintWarning("Syntax: eval [function]\r\n");
				continue;
			}

			std::string function;
			for (int i = 1; i < (int)params.size(); i++)
			{
				if (!params[i].empty())
				{
					function += " ";
					function += params[i];
				}
			}

			if (vm == NULL)
			{
				kConsole::PrintError("No script was loaded.\r\n");
				continue;
			}

			sq_reseterror(vm);

			sq_pushroottable(vm);

			if (SQ_FAILED(sq_compilebuffer(vm, function.c_str(), function.length(), "program", SQFalse)))
			{
				kConsole::PrintError("Error while compiling: %s.\n", kSquirrel::GetLastError(vm));
				continue;
			}

			sq_pushroottable(vm);
			if (SQ_FAILED(sq_call(vm, 1, SQTrue, SQFalse)))
			{
				kConsole::PrintError("Error while executing: %s.\n", kSquirrel::GetLastError(vm));
				continue;
			}
			else
				kConsole::PrintNotice("\"%s\" has executed.\r\n", function.c_str());
			sq_reseterror(vm);
			continue;
		}
		else
		{
			if (vm == NULL) continue;
			std::string command = params[0];
			std::string parameters = line.substr(line.find_first_of(' ') + 1, line.length() - command.length() - 1);

			// OnConsoleCommand(command, params)
			kSquirrel::CallFunction(vm, "OnConsoleCommand", "ss", command.c_str(), parameters.c_str());
		}

	}

	PAUSE;
	return 0;
}

namespace kConsole
{

	void PrintError(const char* text, ...)
	{
		va_list arglist;
		char* dest = new char[1024];
		va_start(arglist, text);
		vsprintf(dest, text, arglist);

		SetColour(LRed);
		printf("[ERROR]: ");
		SetColour(BWhite);
		printf(dest);

		if (std::string(text).find("\r\n") == std::string::npos || std::string(text).find("\n") == std::string::npos)
			printf("\r\n");
		
		delete dest;
	}

	void PrintNotice(const char* text, ...)
	{
		va_list arglist;
		char* dest = new char[1024];
		va_start(arglist, text);
		vsprintf(dest, text, arglist);

		SetColour(LAqua);
		printf("[NOTICE]: ");
		SetColour(BWhite);
		printf(dest);

		if (std::string(text).find("\r\n") == std::string::npos || std::string(text).find("\n") == std::string::npos)
			printf("\r\n");

		delete dest;
	}

	void PrintWarning(const char* text, ...)
	{
		va_list arglist;
		char* dest = new char[1024];
		va_start(arglist, text);
		vsprintf(dest, text, arglist);

		SetColour(LYellow);
		printf("[WARNING]: ");
		SetColour(BWhite);
		printf(dest);

		if (std::string(text).find("\r\n") == std::string::npos || std::string(text).find("\n") == std::string::npos)
			printf("\r\n");

		delete dest;
	}

	void PrintEx(Colors color, const char* text, ...)
	{
		va_list arglist;
		char* dest = new char[1024];
		va_start(arglist, text);
		vsprintf(dest, text, arglist);

		SetColour(color);
		printf(dest);
		SetColour(BWhite);

		if (std::string(text).find("\r\n") == std::string::npos || std::string(text).find("\n") == std::string::npos)
			printf("\r\n");

		delete dest;
	}


}