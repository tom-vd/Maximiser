// (c) 2018 by Tom van Dijkhuizen. All rights reserved.

// C/C++ headers
#include <iostream>
#include <string>
#include <cstring>
#include <cwchar>
using namespace std;

// Win32 headers
#include <Windows.h>

// Library headers
#include <typedefs>

// Other headers
#include "WindowActionA.h"
#include "WindowActionW.h"
#include "main.h"

i32 _tmain(i32 argc,TCHAR* argv[]) {
	// If the program is invoked through the command line and a window title is specified, the console window's title will contain the window title specified.
	// This would cause the console window to be maximised as well. In order to prevent this, the console window's title is set to something else before the
	// call to EnumWindows.
	SetConsoleTitle(TEXT("Maximiser"));
	BOOL res;
	WindowAction wa;
	wa.nCmdShow = SW_MAXIMIZE;
	switch(argc) {
		// TODO: Add "case 3" that can specify whether to maximise or minimise the windows
		case 2:
			wa.WindowText = argv[1];
			tcout << "Going to maximise " << argv[1] << " windows..." << endl;
			res = EnumWindows(&MaximiseWindowsWithTitle,(LPARAM) &wa);
		break;
		case 1:
		default:
			tcout << "Going to maximise all windows..." << endl;
			res = EnumWindows(&PerformWindowAction,(LPARAM) &wa);
		break;
	} // switch
	tcout << "EnumWindows result: " << res << endl;
	tcout << "Press [ENTER] to exit... " << flush;
	
	//getchar();
	return 0;
} // _tmain(...)

///	<summary>Maximises the current window as received through <c>EnumWindows</c> if its title contains the substring pointed to by <c>lParam</c>.
/// <param name="hwnd">Handle to the current window.</param>
/// <param name="lParam">String that is matched to the title of the current window pointed to by <c>hwnd</c>. If the current window's title contains this substring, the window is maximised.</param>
/// <returns>As specified by the <c>EnumWindows</c> documentation, this function returns <c>TRUE</c> if <c>EnumWindows</c> should continue enumerating windows and <c>FALSE</c> if it should stop.</returns>
/// </summary>
BOOL CALLBACK MaximiseWindowsWithTitle(HWND hwnd,LPARAM lParam) {
	DWORD_PTR msgres = 0;
	DWORD LastError = ERROR_SUCCESS;
	// Try to obtain the target window's title length.
	if(!SendMessageTimeout(hwnd,WM_GETTEXTLENGTH,0,0,SMTO_ABORTIFHUNG,DEFAULT_SMTO_TIMEOUT,&msgres)) {
		LastError = GetLastError();
		if(LastError == ERROR_TIMEOUT) tcout << "SendMessageTimeout failed: timeout" << endl;
		else tcout << "SendMessageTimeout failed: " << LastError << endl;
		return TRUE;
	} // if

	// Check whether the operation has succeeded.
	auto len = (i32) (msgres);
	if(!len) {
		LastError = GetLastError();
		if(!LastError) return TRUE;
		tcerr << "Error getting window text length for " << hwnd << ": " << LastError << endl;
		return TRUE;
	} // if

	// Try to allocate memory for the target window's title.
	u32 memsize = (len + 1) * sizeof(TCHAR);
	auto buffer = (TCHAR*) malloc(memsize);
	if(!buffer) {
		tcerr << "Error allocating memory" << endl;
		return FALSE;
	} else ZeroMemory(buffer,memsize);

	// Try to get the target window's title.
	if(!SendMessageTimeout(hwnd,WM_GETTEXT,len + 1,(LPARAM) buffer,SMTO_ABORTIFHUNG,10000,&msgres)) {
		LastError = GetLastError();
		if(LastError == ERROR_TIMEOUT) tcout << "SendMessageTimeout failed: timeout" << endl;
		else tcout << "SendMessageTimeout failed: " << LastError << endl;
		free(buffer);
		return TRUE;
	} // if

	auto wa = (WindowAction*) lParam;
	auto sTargetWindowTitle = wa->WindowText;
	auto res = (LRESULT) msgres;
	if(res) {
		// Check whether the target window's title matches the title specified.
		if(FindSubString(buffer,sTargetWindowTitle) > -1) {
			tcout << "Found a " << sTargetWindowTitle << " window: " << buffer << endl;
			PerformWindowAction(hwnd,lParam);
		} // if
	} else {
		LastError = GetLastError();
		tcerr << "Error getting window text for " << hwnd << ": " << LastError << endl;
	} // if

	free(buffer);
	return TRUE;
} // MaximiseWindowsWithTitle(...)

///	<summary>Maximises the current window as received (either directly or indirectly) through <c>EnumWindows</c>.
/// <param name="hwnd">Handle to the current window.</param>
/// <param name="lParam">Not used.</param>
/// <returns>Always returns <c>TRUE</c> as specified by EnumWindowsProc.</returns>
/// </summary>
BOOL CALLBACK PerformWindowAction(HWND hwnd,LPARAM lParam) {
	auto wa = (WindowAction*) lParam;
	DWORD LastError = ERROR_SUCCESS;
	ShowWindow(hwnd,wa->nCmdShow);
	if(wa->nCmdShow == SW_MAXIMIZE) {
		SetForegroundWindow(hwnd);
		SetFocus(hwnd);
	} // if
	return TRUE;
} // PerformWindowAction(...)

///	<summary>Finds a substring within a larger string.
/// <param name="str1">Haystack; used to find <c>str2</c> in.</param>
/// <param name="str2">Needle; this is the string that is looked for in <c>str1</c>.</param>
/// <returns>If <c>str1</c> contains <c>str2</c>, this function returns the starting index of <c>str2</c> within <c>str1</c>. Otherwise, -1 will be returned.</returns>
/// <remarks>This is the ANSI version of this function.</remarks>
/// <seealso cref="FindSubStringW">The Unicode version of this function.</seealso>
/// </summary>
i32 FindSubStringA(const char* str1,const char* str2) {
	const char* res = strstr(str1,str2);
	if(!res) return -1;
	return (i32) (res - str1);
} // FindSubStringA(...)

///	<summary>Finds a substring within a larger string.
/// <param name="str1">Haystack; used to find <c>str2</c> in.</param>
/// <param name="str2">Needle; this is the string that is looked for in <c>str1</c>.</param>
/// <returns>If <c>str1</c> contains <c>str2</c>, this function returns the starting index of <c>str2</c> within <c>str1</c>. Otherwise, -1 will be returned.</returns>
/// <remarks>This is the Unicode version of this function.</remarks>
/// <seealso cref="FindSubStringA">The ANSI version of this function.</seealso>
/// </summary>
i32 FindSubStringW(const wchar_t* str1,const wchar_t* str2) {
	const wchar_t* res = wcsstr(str1,str2);
	if(!res) return -1;
	return (i32) (res - str1);
} // FindSubStringW(...)
