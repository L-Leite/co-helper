#include "stdafx.h"
#include "Hooks.h"

uintptr_t g_dwServerBase = 0;

char g_szBuffer[ 1024 ];
wchar_t g_wzBuffer[ 1024 ];

CBaseEntityList *g_pEntityList = nullptr;
IGameMovement *g_pGameMovement = nullptr;

void InitializeSharedInterfaces()
{
	HMODULE engine = GetModuleHandleW( L"engine.dll" );
	HMODULE localize = GetModuleHandleW( L"localize.dll" );
	HMODULE materialSystem = GetModuleHandleW( L"MaterialSystem.dll" );
	HMODULE shaderapidx9 = GetModuleHandleW( L"shaderapidx9.dll" );
	HMODULE vgui2 = GetModuleHandleW( L"vgui2.dll" );
	HMODULE vguimatsurface = GetModuleHandleW( L"vguimatsurface.dll" );
	HMODULE vstdlib = GetModuleHandleW( L"vstdlib.dll" );

	CreateInterfaceFn createInterfaces[] =
	{
		(CreateInterfaceFn) GetProcAddress( engine, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( localize, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( materialSystem, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( shaderapidx9, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( vgui2, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( vguimatsurface, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( vstdlib, "CreateInterface" )
	};

	ConnectInterfaces( createInterfaces, sizeof( createInterfaces ) / sizeof( CreateInterfaceFn ) );

	enginetrace = (IEngineTrace*) createInterfaces[ 0 ]( INTERFACEVERSION_ENGINETRACE_SERVER, nullptr );
	ConsoleDebugW( L"enginetrace: %p\n", enginetrace );

	gpGlobals = *(CGlobalVars**) (SearchPattern( L"engine.dll", "\x68\xCC\xCC\xCC\xCC\x50\x50\xFF\x35" ) + 1);
	ConsoleDebugW( L"gpGlobals: %p\n", gpGlobals );
}

void InitializeServerInterfaces()
{
	InitializeSharedInterfaces();

	CreateInterfaceFn serverFactory = (CreateInterfaceFn) GetProcAddress( (HMODULE) g_dwServerBase, "CreateInterface" );
	g_pServerGameDLL = (IServerGameDLL*) serverFactory( INTERFACEVERSION_SERVERGAMEDLL, nullptr );
	g_pGameMovement = (IGameMovement*) serverFactory( INTERFACENAME_GAMEMOVEMENT, nullptr );

	ConsoleDebugW( L"ServerGameDLL: %p\n", g_pServerGameDLL );
	ConsoleDebugW( L"GameMovement: %p\n", g_pGameMovement );
}

void OnServerAttach()
{
	ConsoleDebugW( L"server.dll is being loaded!\n" );

	Main_UnprotectModule( (HMODULE) g_dwServerBase );

	// It should be a good time to call this since most of the libraries should be loaded already
	InitializeServerInterfaces();
	GetAddresses();

	DWORD buyGunAmmo = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x08\x53\x56\x57\x6A\xCC\x8B\xF9\xE8\xCC\xCC\xCC\xCC\x84\xC0" );
	ConsoleDebugW( L"buyGunAmmo: %X\n", buyGunAmmo );
	memset( (void*) buyGunAmmo, 0x90, 18 );

	// Nops out the function call that gives bought weapons free ammo
	DWORD stockAmmoAddress = SearchPattern( L"server.dll", "\x6A\xCC\x6A\xCC\x68\xCC\xCC\xCC\xCC\x6A\x01\x8B\xCE\xE8\xCC\xCC\xCC\xCC\x8B\x06\x8B\xCE\xFF\x90\xCC\xCC\xCC\xCC\x8B\xD8" );
	ConsoleDebugW( L"StockAmmoAddress: %X\n", stockAmmoAddress );
	memset( (void*) stockAmmoAddress, 0x90, 18 );

	DWORD stockAmmoAddress2 = SearchPattern( L"server.dll", "\x6A\xCC\x6A\xCC\x68\xCC\xCC\xCC\xCC\x6A\x01\x8B\xCE\xE8\xCC\xCC\xCC\xCC\x8B\x06\x8B\xCE\xFF\x90\xCC\xCC\xCC\xCC\x8B\xF8" );
	ConsoleDebugW( L"StockAmmoAddress2: %X\n", stockAmmoAddress2 );
	memset( (void*) stockAmmoAddress2, 0x90, 18 );

	BYTE* checkparamsAddress = (BYTE*) SearchPattern( L"server.dll", "\x74\x7B\x8B\x77\x04" );
	ConsoleDebugW( L"checkparamsAddress: %p\n", checkparamsAddress );
	*checkparamsAddress = 0xEB;

	/*DWORD walkMoveAddress = g_dwServerBase + 0x27A1ED;
	ConsoleDebugW( L"walkMoveAddress: %X\n", walkMoveAddress );
	memset( (void*) walkMoveAddress, 0x90, 42 );*/

	HookCSPlayer();
	HookCSGameMovement();
	HookGameDLL_ConVarAccessor();
	HookServerGameDLL();

	// weapon_usp_silencer isn't a real weapon entity fuck
	/*ConVar* mp_ct_default_secondary = cvar->FindVar( "mp_ct_default_secondary" );

	if ( mp_ct_default_secondary )
	{
		mp_ct_default_secondary->SetValue( "weapon_usp_silencer" );
		mp_ct_default_secondary->m_pszDefaultValue = "weapon_usp_silencer";
		//mp_ct_default_secondary->m_fnChangeCallbacks.AddToTail( callbackDefaultSecondary );
		ConsoleDebugW( L"Changed %S to %S\n", mp_ct_default_secondary->GetName(), mp_ct_default_secondary->GetString() );
	}

	// Remove bhop speed limit
	/*DWORD oldProtection = 0;

	BYTE* kek = (BYTE*) (address + 0x3BA500); // For Inferno patch

	// I guess this is a dirty way to do it, maybe I could leave the page with the new page flags
	VirtualProtect( kek, sizeof( BYTE ), PAGE_EXECUTE_READWRITE, &oldProtection );

	*kek = 0xC3; // retn

	VirtualProtect( kek, sizeof( BYTE ), oldProtection, &oldProtection );*/
}

extern "C" __declspec(dllexport) bool HookServer()
{
	ConsoleDebugW( L"HookServer called!\n" );

	g_dwServerBase = (uintptr_t) GetModuleHandleW( L"server.dll" );
	OnServerAttach();

	return true;
}