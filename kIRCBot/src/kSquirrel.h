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

#ifndef KSQUIRREL_H_INCLUDED
#define KSQUIRREL_H_INCLUDED

#include "main.h"
#include "kSock.h"
#include "kIRC.h"

namespace kSquirrel
{
	void Init(HSQUIRRELVM *v, int initial_stack = 1024);

	void GracefulClose(HSQUIRRELVM *v);

	void Close(HSQUIRRELVM *v);

	SQInteger RegisterStds(HSQUIRRELVM v);

	void RegisterNatives(HSQUIRRELVM v);

	SQInteger LoadScript(HSQUIRRELVM v, const SQChar* scriptname);

	const SQChar* GetLastError(HSQUIRRELVM v);

	SQInteger CallFunction(HSQUIRRELVM v, const SQChar* funcname, const char* format = "", ...);
}

namespace kSquirrelInternalFuncs
{
	void PrintFunction(HSQUIRRELVM v, const SQChar* s, ...);

	static void PrintLastErr(HSQUIRRELVM sqvm);

	const SQChar* ReturnLastErr(HSQUIRRELVM sqvm);

	void register_global_func(HSQUIRRELVM vm, const char *name, SQFUNCTION function);
}

#endif