#include "stdafx.h"
#include "IFileSystem.h"

PLH::Detour* exeDirHook = new PLH::Detour;
PLH::VTableSwap* fileSystemHook = new PLH::VTableSwap;
PLH::VTableSwap* baseFileSystemHook = new PLH::VTableSwap;

void* fnGetSearchpath = nullptr;
void* fnGetFileTime = nullptr;
void* fnOpenEx = nullptr;
void* fnFindFirst = nullptr;

void* fnCreateDirHierarchy = nullptr;
void* fnFileExists = nullptr;
void* fnIsFileWritable = nullptr;
void* fnAsyncWrite = nullptr;
void* fnSize = nullptr;
void* fnOpen = nullptr;
void* fnClose = nullptr;
void* fnReadEx = nullptr;

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

bool __fastcall hkFileExists( IBaseFileSystem* thisptr, void*, const char* pFileName, const char* pPathID )
{
	using fn_t = bool( __thiscall* )(IBaseFileSystem*, const char*, const char*);

	if ( strstr( pFileName, "config.cfg" ) )
		printf( "hkFileExists: filename: %s pathid: %s\n", pFileName, pPathID );

	/*if ( !stricmp( pFileName, "//usrlocal/cfg/config.cfg" ) )
	{
		printf( "hkFileExists changed to //mod/cfg/config.cfg\n" );
		pFileName = "//mod/cfg/config.cfg";
	}*/

	/*if ( !stricmp( pPathID, "usrlocal" ) )
		pPathID = "MOD";

	const char* res = strstr( pFileName, "usrlocal" );

	if ( res )
	{
		printf_s( "hkFileExists: original %s\n", pFileName );

		pFileName += 5;
		strcpy_s( (char*) pFileName, 3, "mod" );

		printf_s( "hkFileExists: new %s\n", pFileName );
	}*/

	return baseFileSystemHook->GetOriginal<fn_t>()( thisptr, pFileName, pPathID );
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

	if ( strstr( pFileName, "video.txt" ) )
		printf( "yuh\n" );

	void* res = ((fn_t) fnOpenEx)(thisptr, pFileName, pOptions, flags, pathID, ppszResolvedFilename);

	/*if ( res == (void*) -1 && (strstr( pFileName, "maps\\" ) || strstr( pFileName, "maps/" )) && strstr( pFileName, ".bsp" ) )
	{
		sprintf_s( g_szBuffer, "%S\\csgo\\%s", g_pGameDirectory, pFileName );

		printf( "Couldn't find map %s in our files, looking up %s in csgo...\n", pFileName, g_szBuffer );
		res = ((fn_t) fnOpenEx)(thisptr, g_szBuffer, pOptions, flags, pathID, ppszResolvedFilename);
	}*/

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

	else if ( strstr( pPath, "csgo" ) )
	{
		sprintf_s( szNewPath, "%S\\csgo", g_pGameDirectory );
	}

	else if ( strstr( pPath, "csgo\\bin" ) )
	{
		sprintf_s( szNewPath, "%S\\csgo\\bin", g_pGameDirectory );
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

	if ( !strcmp( pathID, "USRLOCAL" ) || !strcmp( pathID, "game_write" ) )
	{
		sprintf_s( szNewPath, "%S\\csco", g_pMainDirectory );
	}

	printf( "New pPath: %s\n", szNewPath );
	fileSystemHook->GetOriginal< fn_t >()(thisptr, szNewPath, pathID, addType);
}

void* __fastcall hkOpen( IBaseFileSystem* thisptr, void*, const char* pFileName, const char* pOptions, const char* pathID )
{
	using fn_t = void*( __thiscall* )(IBaseFileSystem*, const char*, const char*, const char*);

	static char szNewFileName[ 200 ];

	if ( strstr( pFileName, "video.txt" ) )
	{
		//sprintf( szNewFileName, "%S\\ccso\\cfg\\video.txt", g_pMainDirectory );
		printf( "hkOpen: filename: %s pathid: %s options: %s\n", pFileName, pathID, pOptions );
		//return baseFileSystemHook->GetOriginal< fn_t >()(thisptr, szNewFileName, pOptions, pathID);
	}

	/*if ( !stricmp( pFileName, "//usrlocal/cfg/config.cfg" ) )
	{
		printf( "hkOpen changed to //mod/cfg/config.cfg\n" );
		pFileName = "//mod/cfg/config.cfg";
	}*/

	void* res = baseFileSystemHook->GetOriginal< fn_t >()(thisptr, pFileName, pOptions, pathID);

	return res;
}

void __fastcall hkClose( IBaseFileSystem* thisptr, void*, void* file )
{
	using fn_t = void(__thiscall*)(IBaseFileSystem*, void*);
	((fn_t)fnClose)( thisptr, file );
}

unsigned int __fastcall hkSize( IBaseFileSystem* thisptr, void*, const char* pFileName, const char* pPathID )
{
	using fn_t = unsigned int( __thiscall* )(IBaseFileSystem*, const char*, const char*);

	if ( strstr( pFileName, "config.cfg" ) )
		printf( "hkSize: filename: %s pathid: %s\n", pFileName, pPathID );

	return ((fn_t)fnSize)( thisptr, pFileName, pPathID );
}

bool __fastcall hkIsFileWritable( IBaseFileSystem* thisptr, void*, const char* pFileName, const char* pPathID )
{
	using fn_t = bool( __thiscall* )(IBaseFileSystem*, const char*, const char*);

	if ( strstr( pFileName, "config.cfg" ) )
		printf( "hkIsFileWritable: filename: %s pathid: %s\n", pFileName, pPathID );

	/*if ( !stricmp( pFileName, "//usrlocal/cfg/config.cfg" ) )
	{
		printf( "hkOpen changed to //mod/cfg/config.cfg\n" );
		pFileName = "//mod/cfg/config.cfg";
	} */

	return ((fn_t) fnIsFileWritable)(thisptr, pFileName, pPathID);
}

void __fastcall hkCreateDirHierarchy( IFileSystem* thisptr, void*, const char* path, const char* pathID )
{
	using fn_t = void( __thiscall* )(IFileSystem*, const char*, const char*);

	//printf( "hkCreateDirHierarchy: path: %s pathid: %s\n", path, pathID );

	if ( !strcmp( path, "cfg" ) )
	{
		printf( "hkOpen changed to pathid MOD\n" );
		pathID = "MOD";
	}

	((fn_t)fnCreateDirHierarchy)( thisptr, path, pathID );
}

int __fastcall hkAsyncWrite( IFileSystem* thisptr, void*, const char* pFileName, const void* pSrc, int nSrcBytes, bool bFreeMemory, bool bAppend, void* pControl )
{
	using fn_t = int( __thiscall* )(IFileSystem*, const char*, const void*, int, bool, bool, void*);
	
	if ( strstr( pFileName, "config.cfg" ) )
		printf( "hkAsyncWrite: filename: %s\n", pFileName );

	if ( strstr( pFileName, "cfg/config.cfg" ) )
	{
		sprintf( g_szBuffer, "%s\csco\cfg/config.cfg", g_pMainDirectory );
		printf( "hkOpen changed to %s\n", g_szBuffer );
		pFileName = g_szBuffer;
	}

	return ((fn_t)fnAsyncWrite)( thisptr, pFileName, pSrc, nSrcBytes, bFreeMemory, bAppend, pControl );
}

int __fastcall hkReadEx( IFileSystem* thisptr, void*, void* pOutput, int sizeDest, int size, void* file )
{
	using fn_t = int( __thiscall* )(IFileSystem*, void*, int, int, void*);
	return ((fn_t)fnReadEx)( thisptr, pOutput, sizeDest, size, file );
}

bool __fastcall hkFileSystem_GetExecutableDir( char* exedir ) // exeDirLen got optimized since it's always MAX_PATH
{
	printf( "GetExecutableDir called! exedir: %p %s\n", exedir, exedir );
	sprintf_s( exedir, MAX_PATH, "%S\\bin", g_pGameDirectory );
	printf( "New exedir: %s\n", exedir );
	return true;
}

#ifdef DEDICATED_LAUNCHER
void* FileSystemFactory( const char* pName, int* pReturnCode )
{
	using fn_t = void*(*)(const char*, int*);
	return ((fn_t) ((DWORD) g_hDedicated + 0x47D0))(pName, pReturnCode);
}
#endif

void OnFileSystemLoad( HMODULE hFileSystem )
{
	using CreateInterface_t = void*(*)(const char* pName, int* pReturnCode);

	printf( "Hooking FileSystem...\n" );

#ifdef DEDICATED_LAUNCHER
	void* pFileSystem = FileSystemFactory( "VFileSystem017", nullptr );
#else
	CreateInterface_t pSys = (CreateInterface_t) GetProcAddress( hFileSystem, "CreateInterface" );

	if ( !pSys )
	{
		printf( "Couldn't get CreateInterface from filesystem!\n" );
		return;
	}

	void* pFileSystem = pSys( "VFileSystem017", nullptr );
	void* pBaseFileSystem = pSys( "VBaseFileSystem011", nullptr );

	if ( !pBaseFileSystem )
	{
		printf( "Couldn't get VBaseFileSystem011!\n" );
		return;
	}
#endif

	if ( !pFileSystem )
	{
		printf( "Couldn't get VFileSystem017!\n" );
		return;
	}

	fileSystemHook->SetupHook( (BYTE*) pFileSystem, 11, (BYTE*) &hkAddSearchPath );
	fileSystemHook->Hook();
#ifndef	DEDICATED_LAUNCHER
	/*fnCreateDirHierarchy = fileSystemHook->HookAdditional< void* >( 21, (BYTE*) &hkCreateDirHierarchy );
	fnAsyncWrite = fileSystemHook->HookAdditional< void* >( 80, (BYTE*) &hkAsyncWrite );
	fnReadEx = fileSystemHook->HookAdditional< void* >( 73, (BYTE*) &hkReadEx );*/
#endif
	//fnGetSearchpath = fileSystemHook->HookAdditional< GetSearchPath_t >( 17, (BYTE*) &hkGetSearchPath );
	//fnOpenEx = fileSystemHook->HookAdditional< void* >( 72, (BYTE*) &hkOpenEx );

#ifndef	DEDICATED_LAUNCHER
	/*baseFileSystemHook->SetupHook( (BYTE*) pBaseFileSystem, 10, (BYTE*) &hkFileExists );
	baseFileSystemHook->Hook();
	fnIsFileWritable = baseFileSystemHook->HookAdditional< void* >( 11, (BYTE*) &hkIsFileWritable );;*/
	//baseFileSystemHook->SetupHook( (BYTE*) pBaseFileSystem, 2, (BYTE*) &hkOpen );
	//baseFileSystemHook->Hook();
	//fnClose = baseFileSystemHook->HookAdditional< void* >( 3, (BYTE*) &hkClose )
	//fnSize = baseFileSystemHook->HookAdditional< void* >( 7, (BYTE*) &hkSize );
#endif
	//fnGetFileTime = g_pBaseFileSystemHook->HookAdditional< void* >( 13, (BYTE*) &hkGetFileTime );

	printf( "Filesystem is donezo\n" );
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