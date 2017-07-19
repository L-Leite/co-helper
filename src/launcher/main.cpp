#include "stdafx.h"

#include "Hooks.h"
#include "Updater.h"

char g_szBuffer[ 2048 ];
wchar_t g_wzBuffer[ 2048 ];
wchar_t* g_pMainDirectory = nullptr;
wchar_t* g_pGameDirectory = nullptr;
BOOL g_bIs64Bit = false;
HMODULE g_hLauncher = nullptr;
HMODULE g_hGameLauncher = nullptr;
HMODULE g_hClientDll = nullptr;
HMODULE g_hServerDll = nullptr;

bool CreateDebugConsole()
{
	BOOL result = AllocConsole();

	if ( !result )
		return false;

	freopen( "CONIN$", "r", stdin );
	freopen( "CONOUT$", "w", stdout );
	freopen( "CONOUT$", "w", stderr );

	SetConsoleTitleW( L"Debug Console" );

	return true;
}

wchar_t* GetMainDirectory()
{
	static wchar_t szDirectory[ MAX_PATH ];
	wchar_t szModule[ MAX_PATH ];

	if ( !GetModuleFileNameW( g_hLauncher, szModule, MAX_PATH ) )
	{
		printf( "GetModuleFileName failed!\n" );
		MessageBoxW( nullptr, L"GetModuleFileName failed!", L"Launcher Error", MB_OK | MB_ICONERROR );
		return false;
	}

	wchar_t* pBuffer = wcsrchr( szModule, L'\\' );
	if ( pBuffer )
	{
		*(pBuffer + 1) = L'\0';
	}

	wcscpy_s( szDirectory, szModule );

	size_t j = wcslen( szDirectory );
	if ( j > 0 )
	{
		if ( (szDirectory[ j -1 ] == L'\\') ||
			(szDirectory[ j - 1 ] == L'/') )
		{
			szDirectory[ j - 1 ] = 0;
		}
	}

	return szDirectory;
}

wchar_t* GetGameDirectory()
{
	HKEY steamRegKey = 0;
	LSTATUS res = 0;

	if ( g_bIs64Bit )
		res = RegOpenKeyExW( HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Valve\\Steam", 0, KEY_READ, &steamRegKey );
	else
		res = RegOpenKeyExW( HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_READ, &steamRegKey );

	if ( res )
	{
		printf( "RegOpenKeyEx failed with 0x%X\n", res );
		MessageBoxW( nullptr, L"RegOpenKey failed!", L"Launcher Error", MB_OK | MB_ICONERROR );
		return nullptr;
	}

	wchar_t gamePath[ MAX_PATH + 51 ];
	DWORD pathSize = sizeof( gamePath );

	res = RegQueryValueExW( steamRegKey, L"InstallPath", nullptr, nullptr, (BYTE*) &gamePath, &pathSize );

	if ( res )
	{
		printf( "RegQueryValueEx failed with 0x%X\n", res );
		MessageBoxW( nullptr, L"RegQueryValueEx failed!", L"Launcher Error", MB_OK | MB_ICONERROR );
		RegCloseKey( steamRegKey );
		return nullptr;
	}

	wcscat_s( gamePath, L"\\steamapps\\common\\Counter-Strike Global Offensive" );
	wprintf( L"CSGO's directory: %s\n", gamePath );

	DWORD attributes = GetFileAttributesW( gamePath );

	if ( attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		printf( "Directory is invalid! Attributes: 0x%X\n", attributes );
		MessageBoxW( nullptr, L"Directory is invalid!", L"Launcher Error", MB_OK | MB_ICONERROR );
		RegCloseKey( steamRegKey );
		return nullptr;
	}

	size_t newGamePathSize = wcslen( gamePath ) + 1;
	wchar_t* newGamePath = new wchar_t[ newGamePathSize ];
	wcscpy_s( newGamePath, newGamePathSize, gamePath );

	RegCloseKey( steamRegKey );

	return newGamePath;
}

LONG WINAPI HandleException( _EXCEPTION_POINTERS* ExceptionInfo )
{	   
	HANDLE dumpHandle = CreateFileW( L"ClassicDump.mdmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( dumpHandle != INVALID_HANDLE_VALUE )
	{
		_MINIDUMP_EXCEPTION_INFORMATION exceptInfo;
		exceptInfo.ThreadId = GetCurrentThreadId();
		exceptInfo.ExceptionPointers = ExceptionInfo;
		exceptInfo.ClientPointers = FALSE;

		MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), dumpHandle, MiniDumpWithIndirectlyReferencedMemory, &exceptInfo, nullptr, nullptr );
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

int StartLauncher( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	static char cmdLine[ 256 ];
	sprintf_s( cmdLine, "%S", lpCmdLine );		

	g_hGameLauncher = LoadLibraryExW( L"launcher.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

	if ( !g_hGameLauncher )
	{
		printf( "Failed to load Launcher.dll\n" );
		MessageBoxW( nullptr, L"Failed to load Launcher", L"Launcher Error", MB_OK | MB_ICONERROR );
		return 0;
	}  	

	using LauncherMain_t = int(*)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	LauncherMain_t pfnLauncherMain = (LauncherMain_t) GetProcAddress( g_hGameLauncher, "LauncherMain" );

	HookLauncher();

	return pfnLauncherMain( hInstance, hPrevInstance, cmdLine, nCmdShow );
}	  

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	CreateDebugConsole();

	printf( "Main function called!\n" );   	

	g_hLauncher = hInstance;

	// We probably don't need this, csgo already has an exception handler with a minidumper that works 'ok'
	//AddVectoredExceptionHandler( 0, HandleException );

	if ( !IsWow64Process( GetCurrentProcess(), &g_bIs64Bit ) )
	{
		printf( "IsWow64Process failed!\n" );
		MessageBoxW( NULL, L"IsWow64Process failed!", L"Launcher Error", MB_OK | MB_ICONERROR );
		return 0;
	}

	g_pMainDirectory = GetMainDirectory();
	g_pGameDirectory = GetGameDirectory();

	if ( !HandleRequiredFiles() )
	{
		printf( "HandleRequiredFiles failed!\n" );
		MessageBoxW( NULL, L"HandleRequiredFiles failed!", L"Launcher Error", MB_OK | MB_ICONERROR );
		return 0;
	}

	if ( !UpdateGameDlls() )
	{
		printf( "UpdateGameDlls failed!\n" );
		MessageBoxW( NULL, L"UpdateGameDlls failed!", L"Launcher Error", MB_OK | MB_ICONERROR );
		return 0;
	}

	wchar_t* oldEnv = _wgetenv( L"PATH" );
	swprintf_s( g_wzBuffer, L"PATH=%s\\bin\\;%s\\;%s", g_pGameDirectory, g_pGameDirectory, oldEnv );
	_wputenv( g_wzBuffer );
	wprintf( L"\n%s\n", g_wzBuffer );

	HookWindows();

	swprintf_s( g_wzBuffer, L"%s\\ClassicClient.dll", g_pMainDirectory );
	g_hClientDll = LoadLibraryW( g_wzBuffer );
	swprintf_s( g_wzBuffer, L"%s\\ClassicServer.dll", g_pMainDirectory );
	g_hServerDll = LoadLibraryW( g_wzBuffer );

	char varBuffer[16];
	sprintf( varBuffer, "%u", 243750 );
	SetEnvironmentVariableA( "SteamAppId", varBuffer );
	SetEnvironmentVariableA( "SteamGameId", varBuffer );

	return StartLauncher( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
}