#include "stdafx.h"
#include "Hooks.h"

uintptr_t g_dwServerBase = 0;

char g_szBuffer[ 1024 ];
wchar_t g_wzBuffer[ 1024 ];

CGlobalVars* gpGlobals = nullptr;
CGlobalEntityList* gEntList = nullptr;
CBaseEntityList *g_pEntityList = nullptr;
IVEngineServer* g_pEngineServer = nullptr;
IVEngineServer* engine = nullptr;
IServerGameDLL* g_pServerGameDLL = nullptr;
IGameMovement *g_pGameMovement = nullptr;
IGameEventManager2* gameeventmanager = nullptr;
IEngineTrace* enginetrace = nullptr;
IEngineVGui* enginevgui = nullptr;
IVModelInfo* modelinfo = nullptr;
IUniformRandomStream* random = nullptr;
IPhysicsSurfaceProps* physprops = nullptr;
IStaticPropMgrServer* staticpropmgr = nullptr;
IVDebugOverlay* debugoverlay = nullptr;

void InitializeSharedInterfaces()
{
	HMODULE hEngine = GetModuleHandleW( L"engine.dll" );
	HMODULE localize = GetModuleHandleW( L"localize.dll" );
	HMODULE materialSystem = GetModuleHandleW( L"MaterialSystem.dll" );
	HMODULE datacache = GetModuleHandleW( L"datacache.dll" );
	HMODULE vstdlib = GetModuleHandleW( L"vstdlib.dll" );

	CreateInterfaceFn createInterfaces[] =
	{
		(CreateInterfaceFn) GetProcAddress( hEngine, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( localize, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( materialSystem, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( datacache, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( vstdlib, "CreateInterface" )
	};

	ConnectInterfaces( createInterfaces, sizeof( createInterfaces ) / sizeof( CreateInterfaceFn ) );

	gpGlobals = *(CGlobalVars**) (SearchPattern( L"engine.dll", "\x68\xCC\xCC\xCC\xCC\x50\x50\x8B\x02" ) + 1);
	ConsoleDebugW( L"gpGlobals: %p\n", gpGlobals );

	CreateInterfaceFn physicsFactory = (CreateInterfaceFn) GetProcAddress( GetModuleHandleW( L"vphysics.dll" ), "CreateInterface" );

	g_pEngineServer = (IVEngineServer*) createInterfaces[ 0 ]( INTERFACEVERSION_VENGINESERVER, nullptr );
	engine = g_pEngineServer;
	enginetrace = (IEngineTrace*) createInterfaces[ 0 ]( INTERFACEVERSION_ENGINETRACE_SERVER, nullptr );
	enginevgui = (IEngineVGui*) createInterfaces[ 0 ]( VENGINE_VGUI_VERSION, nullptr );
	random = (IUniformRandomStream*) createInterfaces[ 0 ]( VENGINE_SERVER_RANDOM_INTERFACE_VERSION, nullptr );
	physprops = (IPhysicsSurfaceProps*) physicsFactory( VPHYSICS_SURFACEPROPS_INTERFACE_VERSION, nullptr );
	staticpropmgr = (IStaticPropMgrServer*) createInterfaces[ 0 ]( INTERFACEVERSION_STATICPROPMGR_SERVER, nullptr );
	modelinfo = (IVModelInfo*) createInterfaces[ 0 ]( VMODELINFO_SERVER_INTERFACE_VERSION, nullptr );
	gameeventmanager = (IGameEventManager2*) createInterfaces[ 0 ]( INTERFACEVERSION_GAMEEVENTSMANAGER2, nullptr );
	debugoverlay = (IVDebugOverlay*) createInterfaces[ 0 ]( VDEBUG_OVERLAY_INTERFACE_VERSION, nullptr );

	ConsoleDebugW( L"engine: %p\n", engine );
	ConsoleDebugW( L"enginetrace: %p\n", enginetrace );		 
	ConsoleDebugW( L"enginevgui: %p\n", enginevgui );
	ConsoleDebugW( L"random: %p\n", random );
	ConsoleDebugW( L"physprops: %p\n", physprops );
	ConsoleDebugW( L"staticpropmgr: %p\n", staticpropmgr );
	ConsoleDebugW( L"modelinfo: %p\n", modelinfo );
	ConsoleDebugW( L"gameeventmanager: %p\n", gameeventmanager );
	ConsoleDebugW( L"debugoverlay: %p\n", debugoverlay );
}

void InitializeServerInterfaces()
{
	ConsoleDebugW( L"\n### SERVER INTERFACE START\n" );		  

	InitializeSharedInterfaces();

	CreateInterfaceFn serverFactory = (CreateInterfaceFn) GetProcAddress( (HMODULE) g_dwServerBase, "CreateInterface" );
	g_pServerGameDLL = (IServerGameDLL*) serverFactory( INTERFACEVERSION_SERVERGAMEDLL, nullptr );
	g_pGameMovement = (IGameMovement*) serverFactory( INTERFACENAME_GAMEMOVEMENT, nullptr );   	

	ConsoleDebugW( L"ServerGameDLL: %p\n", g_pServerGameDLL );
	ConsoleDebugW( L"g_pGameMovement: %p\n", g_pGameMovement );

	ConsoleDebugW( L"### SERVER INTERFACE END\n\n" );
}

void OnServerAttach()
{
	ConsoleDebugW( L"server.dll is being loaded!\n" );

	Main_UnprotectModule( (HMODULE) g_dwServerBase );

	// It should be a good time to call this since most of the libraries should be loaded already
	InitializeServerInterfaces();
	GetAddresses();
	MathLib_Init( 2.2f, 2.2f, 0.0f, 2.0f );
	InitializeConVars();

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

	BYTE* checkparamsAddress = (BYTE*) SearchPattern( L"server.dll", "\x74\x6E\x8B\x77\x04" );
	ConsoleDebugW( L"checkparamsAddress: %p\n", checkparamsAddress );
	*checkparamsAddress = 0xEB;

	/*DWORD walkMoveAddress = g_dwServerBase + 0x27A1ED;
	ConsoleDebugW( L"walkMoveAddress: %X\n", walkMoveAddress );
	memset( (void*) walkMoveAddress, 0x90, 42 );*/
				   
	// It wont auto reload ammo when transitioning to a new round
	BYTE* roundReloadAddress = (BYTE*) SearchPattern( L"server.dll", "\x74\x2B\x8B\xCE\x80\x79\x58\xCC\x74\x06\x80\x49\x5C\x01\xEB\x17\x8B\x51\x1C\x85\xD2\x74\x10\x8D\x86\xCC\xCC\xCC\xCC\x2B\xC1\x8B\xCA\x50\xE8\xCC\xCC\xCC\xCC\x89\x9E\xCC\xCC\xCC\xCC\x8B\x07" );
	ConsoleDebugW( L"roundReloadAddress: %p\n", roundReloadAddress );
	*roundReloadAddress = 0xEB;

	ConVar* sv_penetration_type = cvar->FindVar( "sv_penetration_type" );
	ConsoleDebugW( L"sv_penetration_type: %p\n", sv_penetration_type );

	if ( sv_penetration_type )
	{
		sv_penetration_type->SetValue( 0 );
		sv_penetration_type->m_pszDefaultValue = "0";
		sv_penetration_type->RemoveFlags( FCVAR_HIDDEN );
		ConsoleDebugW( L"Changed sv_penetration_type value and flags...\n" );
	}

	//GetCSViewVectors()->m_vView.z = 30.0f;

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