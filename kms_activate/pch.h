#ifndef PCH_H
#define PCH_H

#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <map>

#ifdef _UNICODE
#define tstring wstring
#define tcout   wcout
#else
#define tstring string
#define tcout   cout
#endif // _UNICODE


#endif //PCH_H
