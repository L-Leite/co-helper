#include "stdafx.h"
#include "WindowsHooks.h"
#include "IFileSystem.h"

PLH::Detour* loadLibraryHook = new PLH::Detour;
PLH::Detour* getCommandLineHook = new PLH::Detour;

HMODULE WINAPI hkLoadLibraryExA( LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags )
{
	using fn_t = HMODULE( WINAPI* )(LPCSTR, HANDLE, DWORD);

	// Let's keep these incase the user has something like metamod source
	static bool bClientCalled = false;
	static bool bServerCalled = false;
	static bool bFileSystemCalled = false;

	HMODULE res = loadLibraryHook->GetOriginal< fn_t >()(lpLibFileName, hFile, dwFlags);

	if ( !bServerCalled && strstr( lpLibFileName, "bin\\server.dll" ) )
	{
		using HookServer_t = bool(*)();

		HookServer_t fnHookServer = (HookServer_t) GetProcAddress( g_hServerDll, "HookServer" );
		fnHookServer();

		bServerCalled = true;
	}

	else if ( !bClientCalled && strstr( lpLibFileName, "bin\\client.dll" ) )
	{
		using HookClient_t = bool(*)();

		HookClient_t fnHookClient = (HookClient_t) GetProcAddress( g_hClientDll, "HookClient" );
		fnHookClient();

		bClientCalled = true;
	}

	else if ( !bFileSystemCalled && strstr( lpLibFileName, "bin\\filesystem_stdio.dll" ) )
	{
		OnFileSystemLoad( res );
		bFileSystemCalled = true;
	}

	return res;
}

LPSTR WINAPI hkGetCommandLineA()
{
	using fn_t = LPSTR( WINAPI* )();

	static char cmdLine[ 512 ];
	static bool bReady = false;

	printf( "GetCommandLineA called!\n" );		 	

	if ( !bReady )
	{
#ifdef _DEBUG
		sprintf_s( cmdLine, "%s -insecure -nobreakpad -nominidumps -dev -game \"%S\\csco\"", getCommandLineHook->GetOriginal< fn_t >()(), g_pMainDirectory );
#else
		sprintf_s( cmdLine, "%s -insecure -nobreakpad -game \"%S\\csco\"", getCommandLineHook->GetOriginal< fn_t >()(), g_pMainDirectory );
#endif
		printf( "New cmdLine: %s\n", cmdLine );
		bReady = true;
	}

	return cmdLine;
}

void HookWindows()
{
	loadLibraryHook->SetupHook( (BYTE*) &LoadLibraryExA, (BYTE*) &hkLoadLibraryExA );
	loadLibraryHook->Hook();

	getCommandLineHook->SetupHook( (BYTE*) &GetCommandLineA, (BYTE*) &hkGetCommandLineA );
	getCommandLineHook->Hook();
}