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
#include "kSquirrel.h"
#include "kSock.h"
#include "kIRC.h"
#include "Natives.h"

namespace kSquirrel
{
	void Init(HSQUIRRELVM *v, int initial_stack)
	{
		*v = sq_open(initial_stack);
		sq_setprintfunc(*v, kSquirrelInternalFuncs::PrintFunction, NULL);
	}

	void GracefulClose(HSQUIRRELVM *v)
	{
		CallFunction(*v, "OnScriptExit");
		Sleep(1500);
		Close(v);
	}

	void Close(HSQUIRRELVM *v)
	{
		sq_pop(*v, 1);
		sq_close(*v);
		if (v == &vm)
			vm = NULL;
		*v = NULL;
	}

	SQInteger RegisterStds(HSQUIRRELVM v)
	{
		/*return
			// Standard libraries
			SQ_SUCCEEDED(sqstd_register_iolib(v))		&&
			SQ_SUCCEEDED(sqstd_register_systemlib(v))	&&
			SQ_SUCCEEDED(sqstd_register_stringlib(v))	&&
			SQ_SUCCEEDED(sqstd_register_mathlib(v))
			// ------------------
			;*/
		sq_pushroottable(v);
		sqstd_register_iolib(v);
		sqstd_register_mathlib(v);
		sqstd_register_stringlib(v);
		sqstd_register_systemlib(v);
		sq_pushroottable(v);
		return true;
	}

	void RegisterNatives(HSQUIRRELVM v)
	{
#define REGISTERNATIVE(name) kSquirrelInternalFuncs::register_global_func(v, #name, kSquirrelNatives::name)
		// Register our natives here
		//kSquirrelInternalFuncs::register_global_func(v, "IRC_Connect", kSquirrelNatives::IRC_Connect);
		REGISTERNATIVE(printerror);
		REGISTERNATIVE(printwarning);
		REGISTERNATIVE(printnotice);
		REGISTERNATIVE(printex);
		REGISTERNATIVE(exit);
		REGISTERNATIVE(do_eval);
		REGISTERNATIVE(IRC_Connect);
		REGISTERNATIVE(IRC_Quit);
		REGISTERNATIVE(IRC_SendRaw);
		REGISTERNATIVE(IRC_JoinChannel);
		REGISTERNATIVE(IRC_PartChannel);
		REGISTERNATIVE(IRC_ChangeNick);
		REGISTERNATIVE(IRC_SetMode);
		REGISTERNATIVE(IRC_Say);
		REGISTERNATIVE(IRC_Notice);
		REGISTERNATIVE(IRC_InviteUser);
		REGISTERNATIVE(IRC_KickUser);
		REGISTERNATIVE(IRC_SetChannelTopic);
		REGISTERNATIVE(IRC_SendCTCP);
		REGISTERNATIVE(IRC_GetUMode);
		REGISTERNATIVE(IRC_GetChMode);
		REGISTERNATIVE(IRC_SetCommandPrefix);
		REGISTERNATIVE(IRC_GetChannelTopic);
		REGISTERNATIVE(IRC_GetUserChannelModes);
		REGISTERNATIVE(IRC_IsUserOwner);
		REGISTERNATIVE(IRC_IsUserAdmin);
		REGISTERNATIVE(IRC_IsUserOp);
		REGISTERNATIVE(IRC_IsUserHalfop);
		REGISTERNATIVE(IRC_IsUserVoice);
		REGISTERNATIVE(IRC_IsUserAway);
		REGISTERNATIVE(IRC_IsUserOper);
		REGISTERNATIVE(IRC_IsUserBot);
		REGISTERNATIVE(IRC_IsUserRegistered);
		REGISTERNATIVE(IRC_GetChannelUserList);
	}

	SQInteger LoadScript(HSQUIRRELVM v, const SQChar* scriptname)
	{
		sq_pushroottable(v);
		SQRESULT ret = sqstd_dofile(v, scriptname, SQTrue, SQTrue);

		return SQ_SUCCEEDED(ret);
	}

	const SQChar* GetLastError(HSQUIRRELVM v)
	{
		return kSquirrelInternalFuncs::ReturnLastErr(v);
	}

	SQInteger CallFunction(HSQUIRRELVM v, const SQChar* funcname, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		int numargs = 0;

		int top = sq_gettop(v);
		sq_pushroottable(v);
		sq_pushstring(v, _SC(funcname), -1);
		if (SQ_FAILED(sq_get(v, -2)))
			return -1;
		sq_pushroottable(v);

		
		/*
		Formats:
		OT_NULL,OT_INTEGER,OT_FLOAT,OT_STRING,OT_TABLE,OT_ARRAY,OT_USERDATA,
		OT_CLOSURE,OT_NATIVECLOSURE,OT_GENERATOR,OT_USERPOINTER,OT_BOOL,OT_INSTANCE,OT_CLASS,OT_WEAKREF
		*/

		while (*format != '\0')
		{
			//printf("lol while loop\n");
			numargs++;
			switch (*format)
			{
			case 'i': // OT_INTEGER
				sq_pushinteger(v, va_arg(args, SQInteger));
				//printf("i pushed\n");
				break;
			case 'f': // OT_FLOAT
				sq_pushfloat(v, (SQFloat)va_arg(args, double));
				break;
			case 's': // OT_STRING
				sq_pushstring(v, va_arg(args, SQChar*), -1);
				//printf("s pushed\n");
				break;
			case 't': // OT_TABLE
				kConsole::PrintError("Error (kSquirrel::CallFunction): Table(%c) is currently not supported.\r\n", format);
				return -1;
				//break;
			case 'a': // OT_ARRAY
				kConsole::PrintError("Error (kSquirrel::CallFunction): Array(%c) is currently not supported.\r\n", format);
				return -1;
				//break;
			case 'b': // OT_BOOL
				sq_pushbool(v, va_arg(args, SQBool));
				//break;
			default:
				kConsole::PrintError("Error (kSquirrel::CallFunction): Specifier (%c) is currently not supported.\r\n", format);
				return -1;
				//break;
			}
			format++;
		}
		if (SQ_FAILED(sq_call(v, ++numargs, SQTrue, SQFalse))) {
			//kSquirrelInternalFuncs::PrintLastErr(v); // Printing callback errors
			return -1;
		}
		SQInteger returnval;
		sq_getinteger(v, -1, &returnval);
		va_end(args);
		sq_settop(v, top);
		return returnval;
	}
}

namespace kSquirrelInternalFuncs
{
	void PrintFunction(HSQUIRRELVM v, const SQChar* s, ...)
	{
		va_list arglist;
		va_start(arglist, s);
		vprintf(s, arglist);
		if (std::string(s).find("\r\n") == std::string::npos || std::string(s).find("\n") == std::string::npos)
			printf("\r\n");
		va_end(arglist);
	}

	static void PrintLastErr(HSQUIRRELVM sqvm)
	{
		/*const SQChar *error;
		sq_getlasterror(sqvm);
		if (SQ_SUCCEEDED(sq_getstring(sqvm, -1, &error))) {
			fprintf(stderr, "sqvm error: %s\n", error);
		}*/

		kConsole::PrintError("Squirrel Error: %s\r\n", ReturnLastErr(sqvm));
	}

	const SQChar* ReturnLastErr(HSQUIRRELVM sqvm)
	{
		const SQChar* error;
		sq_getlasterror(sqvm);
		if (SQ_SUCCEEDED(sq_getstring(sqvm, -1, &error)))
			return error;
		else
			return "None";
	}

	void register_global_func(HSQUIRRELVM vm, const char *name, SQFUNCTION function)
	{
		sq_pushroottable(vm);
		sq_pushstring(vm, name, -1);
		sq_newclosure(vm, function, 0);
		sq_createslot(vm, -3);
		sq_pop(vm, 1);
	}
}