#include "stdafx.h"
#include "IFileSystem.h"

PLH::Detour* exeDirHook = new PLH::Detour;
PLH::VTableSwap* fileSystemHook = new PLH::VTableSwap;

void* fnGetSearchpath = nullptr;
void* fnFileExists = nullptr;
void* fnGetFileTime = nullptr;
void* fnOpenEx = nullptr;
void* fnFindFirst = nullptr;

int __fastcall hkGetSearchPath( IFileSystem* thisptr, void*, const char* pathID, bool bGetPackFiles, char* pPath, int nMaxLen )
{
	using fn_t = int( __thiscall* )(IFileSystem*, const char*, bool, char*, int);

	if ( !strcmp( pathID, "USRLOCAL" ) )
	{
		printf( "GetSearchPath: changing USRLOCAL to MOD\n" );
		pathID = "MOD";
	}

	return ((fn_t) fnGetSearchpath)(thisptr, pathID, bGetPackFiles, pPath, nMaxLen);
}

bool __fastcall hkFileExists( IFileSystem* thisptr, void*, const char *pFileName, const char *pPathID )
{
	using fn_t = bool( __thiscall* )(IFileSystem*, const char*, const char*);

	if ( !stricmp( pPathID, "usrlocal" ) )
		pPathID = "MOD";

	const char* res = strstr( pFileName, "usrlocal" );

	if ( res )
	{
		printf_s( "hkFileExists: original %s\n", pFileName );

		pFileName += 5;
		strcpy_s( (char*) pFileName, 3, "mod" );

		printf_s( "hkFileExists: new %s\n", pFileName );
	}

	return ((fn_t) fnFileExists)(thisptr, pFileName, pPathID);
}

long __fastcall hkGetFileTime( IFileSystem* thisptr, void*, const char *pFileName, const char *pPathID )
{
	using fn_t = long( __thiscall* )(IFileSystem*, const char*, const char*);

	if ( !stricmp( pPathID, "usrlocal" ) )
		pPathID = "MOD";

	const char* res = strstr( pFileName, "usrlocal" );

	if ( res )
	{
		printf_s( "hkGetFileTime: original %s\n", pFileName );

		pFileName += 5;
		strcpy_s( (char*) pFileName, 3, "mod" );

		printf_s( "hkGetFileTime: new %s\n", pFileName );
	}

	return ((fn_t) fnGetFileTime)(thisptr, pFileName, pPathID);
}

void* __fastcall hkOpenEx( IFileSystem* thisptr, void*, const char* pFileName, const char* pOptions, unsigned flags, const char* pathID, char** ppszResolvedFilename )
{
	using fn_t = void*(__thiscall*)(IFileSystem*, const char*, const char*, unsigned, const char*, char**);

	void* res = ((fn_t) fnOpenEx)(thisptr, pFileName, pOptions, flags, pathID, ppszResolvedFilename);

	if ( res == (void*) -1 && (strstr( pFileName, "maps\\" ) || strstr( pFileName, "maps/" )) && strstr( pFileName, ".bsp" ) )
	{
		sprintf_s( g_szBuffer, "%S\\csgo\\%s", g_pGameDirectory, pFileName );

		printf( "Couldn't find map %s in our files, looking up %s in csgo...\n", pFileName, g_szBuffer );
		res = ((fn_t) fnOpenEx)(thisptr, g_szBuffer, pOptions, flags, pathID, ppszResolvedFilename);
	}

	return res;
}

const char* __fastcall hkFindFirst( IFileSystem* thisptr, void*, const char* pWildCard, int* pHandle )
{
	using fn_t = const char*(__thiscall*)(IFileSystem*, const char*, int*);

	if ( !strcmp( pWildCard, "maps/*.bsp" ) )
	{
		printf( "Changing wildcard...\n" );
		sprintf_s( g_szBuffer, "%S\\csgo\\%s", g_pGameDirectory, pWildCard );
	}

	return ((fn_t) fnFindFirst)(thisptr, g_szBuffer, pHandle);
}

void __fastcall hkAddSearchPath( IFileSystem* thisptr, void* edx, const char* pPath, const char* pathID, int addType )
{
	using fn_t = void( __thiscall* )(IFileSystem*, const char*, const char*, int);
	static char szNewPath[ MAX_PATH ];

	printf( "AddSearchpath called! pPath: %s pathID: %s\n", pPath, pathID );

	/*if ( strstr( pPath, "maps/" ) && strstr( pPath, ".bsp" ) && !strstr( pPath, "" ) )
	{
	sprintf_s( szNewPath, "%S\\csgo\\%s", g_pGameDirectory, pPath );
	}

	else*/ if ( strstr( pPath, "content\\csgo\\pak01_dir.vpk" ) )
	{
		sprintf_s( szNewPath, "%S\\csgo\\pak01_dir.vpk", g_pGameDirectory );
	}

	else if ( strstr( pPath, "content\\csgo" ) )
	{
		sprintf_s( szNewPath, "%S\\csgo", g_pGameDirectory );
	}

	else if ( strstr( pPath, "content\\platform" ) )
	{
		sprintf_s( szNewPath, "%S\\platform", g_pGameDirectory );
	}

	else if ( strstr( pPath, "platform\\config" ) )
	{
		sprintf_s( szNewPath, "%S\\platform\\config", g_pGameDirectory );
	}

	else if ( !strcmp( pPath, "platform" ) )
	{
		sprintf_s( szNewPath, "%S\\platform", g_pGameDirectory );
	}

	else
		strcpy( szNewPath, pPath );

	printf( "New pPath: %s\n", szNewPath );
	fileSystemHook->GetOriginal< fn_t >()(thisptr, szNewPath, pathID, addType);
}

bool __fastcall hkFileSystem_GetExecutableDir( char* exedir ) // exeDirLen got optimized since it's always MAX_PATH
{
	printf( "GetExecutableDir called! exedir: %p %s\n", exedir, exedir );
	sprintf_s( exedir, MAX_PATH, "%S\\bin", g_pGameDirectory );
	printf( "New exedir: %s\n", exedir );
	return true;
}

// This code is small so why not let it stay here
void HookLauncher()
{
	printf( "Hooking Launcher...\n" );

#ifdef DEDICATED_LAUNCHER
	BYTE* getExeDirAddress = (BYTE*) SearchPattern( L"dedicated.dll", "\x55\x8B\xEC\x81\xEC\xCC\xCC\xCC\xCC\x80\x3D\xCC\xCC\xCC\xCC\xCC\x56" );
#else
	BYTE* getExeDirAddress = (BYTE*) SearchPattern( L"launcher.dll", "\x55\x8B\xEC\x81\xEC\xCC\xCC\xCC\xCC\x80\x3D\xCC\xCC\xCC\xCC\xCC\x56" );
#endif
	printf( "FileSystem_GetExecutableDir: %p", getExeDirAddress );

	exeDirHook->SetupHook( getExeDirAddress, (BYTE*) hkFileSystem_GetExecutableDir );
	exeDirHook->Hook();
}

void OnFileSystemLoad( HMODULE hFileSystem )
{
	using CreateInterface_t = void*(*)(const char* pName, int* pReturnCode);

	printf( "Hooking FileSystem...\n" );

	void* pFileSystem = nullptr;
	CreateInterface_t pSys = (CreateInterface_t) GetProcAddress( hFileSystem, "CreateInterface" );

	if ( pSys )
	{
		pFileSystem = pSys( "VFileSystem017", nullptr );
		fileSystemHook->SetupHook( (BYTE*) pFileSystem, 11, (BYTE*) &hkAddSearchPath );
		fileSystemHook->Hook();
		//fnGetSearchpath = fileSystemHook->HookAdditional< GetSearchPath_t >( 17, (BYTE*) &hkGetSearchPath );
		//fnOpenEx = fileSystemHook->HookAdditional< void* >( 72, (BYTE*) &hkOpenEx );

		/*g_pBaseFileSystemHook->SetupHook( (BYTE*) ( (DWORD) pFileSystem + 4 ), 10, (BYTE*) &hkFileExists );
		g_pBaseFileSystemHook->Hook();
		fnGetFileTime = g_pBaseFileSystemHook->HookAdditional< void* >( 13, (BYTE*) &hkGetFileTime );*/		 		
	}
}