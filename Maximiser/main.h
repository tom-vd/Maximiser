// (c) 2018 by Tom van Dijkhuizen. All rights reserved.

#ifndef main_h_
#define main_h_

#ifdef UNICODE
#define FindSubString FindSubStringW
#define tcout wcout
#define tcerr wcerr
#define tclog wclog
#define tcin wcin
#define _tmain wain
using WindowAction = WindowActionW;
#else
#define FindSubString FindSubStringA
#define tcout cout
#define tcerr cerr
#define tclog clog
#define tcin cin
#define _tmain main
using WindowAction = WindowActionA;
#endif

i32 _tmain(i32 argc,TCHAR* argv[]);
BOOL CALLBACK MaximiseWindowsWithTitle(HWND hwnd,LPARAM lParam);
BOOL CALLBACK PerformWindowAction(HWND hwnd,LPARAM lParam);

i32 FindSubStringA(const char* str1,const char* str2);
i32 FindSubStringW(const wchar_t* str1,const wchar_t* str2);

#endif
