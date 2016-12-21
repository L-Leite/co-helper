#pragma once			

#ifdef _DEBUG
#define ConsoleDebugA printf_s
#define ConsoleDebugW wprintf_s
#else
#define ConsoleDebugA
#define ConsoleDebugW
#endif

void Main_UnprotectModule( HMODULE hModule );