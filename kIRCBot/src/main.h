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

#ifndef KMAIN_H_INCLUDED
#define KMAIN_H_INCLUDED

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include <Squirrel/squirrel.h> 
#include <Squirrel/sqstdio.h> 
#include <Squirrel/sqstdaux.h> 
#include <Squirrel/sqstdmath.h>
#include <Squirrel/sqstdstring.h>
#include <Squirrel/sqstdsystem.h>

#define K_VERSION "1.0"

/*#ifdef _MSC_VER
#pragma comment (lib ,"squirrel.lib")
#pragma comment (lib ,"sqstdlib.lib")
#endif*/

#include "kIRC.h"

#define VMIDTOREAL(id)	(id-1)
#define REALIDTOVM(id)	(id+1)

extern HSQUIRRELVM vm;
extern std::vector<kIRC*> IRCs;
//extern kIRC* IRCSock;

#ifndef _WIN32
typedef int SOCKET;
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#define Sleep(x)        sleep(x/1000);
#define SD_BOTH         SHUT_RDWR
#define SOCKET_ERROR    (-1)
#define INVALID_SOCKET  (-1)
#endif

#ifdef _WIN32
extern HANDLE hConsole;
enum Colors // 0x0 -> 0xF
{
	Black = 0,	Blue,	Green,	Aqua,	Red,	Purple,	Yellow,	White,
	Gray,		LBlue,	LGreen,	LAqua,	LRed,	LPurple,LYellow,BWhite
};
#define SetColour(color)	SetConsoleTextAttribute(hConsole, color)
#else
#define LRed	"1;31"
#define LAqua	"0;36"
#define Aqua	LAqua
#define LYellow	"1;33"
#define Yellow	LYellow
#define White	"0;0"
#define BWhite	White
#define SetColour(color)	printf("\x1b[%sm", color)
typedef char* Colors;
#endif
namespace kConsole
{

	void PrintError(const char* text, ...);
	void PrintNotice(const char* text, ...);
	void PrintWarning(const char* text, ...);
	void PrintEx(Colors color, const char* text, ...);

}

#endif