#include "stdafx.h"
#include "Hooks.h"

uintptr_t g_dwClientBase = 0;

char g_szBuffer[ 1024 ];
wchar_t g_wzBuffer[ 1024 ];

IClientEntityList* entitylist = nullptr;
IBaseClientDLL* g_pClientDLL = nullptr;
IVEngineClient* g_pEngineClient = nullptr;
IVEngineClient* engine = nullptr;

void InitializeSharedInterfaces()
{							  
	HMODULE hEngine = GetModuleHandleW( L"engine.dll" );
	HMODULE filesystem_stdio = GetModuleHandleW( L"filesystem_stdio.dll" );
	HMODULE localize = GetModuleHandleW( L"localize.dll" );
	HMODULE materialSystem = GetModuleHandleW( L"MaterialSystem.dll" );
	HMODULE shaderapidx9 = GetModuleHandleW( L"shaderapidx9.dll" );
	HMODULE vgui2 = GetModuleHandleW( L"vgui2.dll" );
	HMODULE vguimatsurface = GetModuleHandleW( L"vguimatsurface.dll" );
	HMODULE vstdlib = GetModuleHandleW( L"vstdlib.dll" );

	CreateInterfaceFn createInterfaces[] =
	{
		(CreateInterfaceFn) GetProcAddress( hEngine, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( filesystem_stdio, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( localize, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( materialSystem, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( shaderapidx9, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( vgui2, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( vguimatsurface, "CreateInterface" ),
		(CreateInterfaceFn) GetProcAddress( vstdlib, "CreateInterface" )
	};

	ConnectInterfaces( createInterfaces, sizeof( createInterfaces ) / sizeof( CreateInterfaceFn ) );

	g_pEngineClient = (IVEngineClient*) createInterfaces[ 0 ]( VENGINE_CLIENT_INTERFACE_VERSION, nullptr );
	engine = g_pEngineClient;
}

void InitializeClientInterfaces()
{
	ConsoleDebugW( L"\n### CLIENT INTERFACE START\n" );
	InitializeSharedInterfaces();

	CreateInterfaceFn clientFactory = (CreateInterfaceFn) GetProcAddress( (HMODULE) g_dwClientBase, "CreateInterface" );

	g_pClientDLL = (IBaseClientDLL*) clientFactory( CLIENT_DLL_INTERFACE_VERSION, nullptr );
	ConsoleDebugW( L"ClientDLL: %p\n", g_pClientDLL );

	entitylist = (IClientEntityList*) clientFactory( VCLIENTENTITYLIST_INTERFACE_VERSION, nullptr );
	ConsoleDebugW( L"entitylist: %p\n", entitylist );
	ConsoleDebugW( L"### CLIENT INTERFACE END\n\n" );
}

void OnClientAttach()
{ 
	ConsoleDebugW( L"client.dll is being loaded!\n" );	

	Main_UnprotectModule( (HMODULE) g_dwClientBase );

	InitializeClientInterfaces();  	  
	GetAddresses();
	MathLib_Init( 2.2f, 2.2f, 0.0f, 2.0f );

	HookPanel();
#ifdef VGUI_TEST
	HookCounterStrikeViewport();
#endif

	RegisterClientConVars();

	// Bypass swf checks
	DWORD swfBypassAddress = SearchPattern( L"client.dll", "\x55\x8B\xEC\x83\xE4\xF0\x81\xEC\xCC\xCC\xCC\xCC\x8B\x4D\x08" );
	ConsoleDebugW( L"swfBypassAddress: %X\n", swfBypassAddress );
	uint8_t swfBypassBytes[ 8 ] = { 0xB8, 0x01, 0x00, 0x00, 0x00, 0xC2, 0x0C, 0x00 }; // mov eax, 1; retn 0C
	memcpy( (void*) swfBypassAddress, swfBypassBytes, sizeof( swfBypassBytes ) );

	// We patch thses addresses so sv_competitive_minspec wont fuck us
	BYTE* compSpecsAddress = (BYTE*) SearchPattern( L"client.dll", "\x55\x8B\xEC\x56\x8B\x75\x08\x85\xF6\x74\x05\x83\xC6\xE8" );
	ConsoleDebugW( L"compSpecsAddress: %p\n", compSpecsAddress );
	*compSpecsAddress = 0xC3;

	DWORD compSpecsAddress2 = SearchPattern( L"client.dll", "\xF3\x0F\x10\x45\xCC\x8B\x47\x44" );
	ConsoleDebugW( L"compSpecsAddress2: %X\n", compSpecsAddress2 );
	memset( (void*) compSpecsAddress2, 0x90, 23 );

	/*WORD* compSpecsAddress3 = (WORD*) (g_dwClientBase + 0x7112D0 );
	ConsoleDebugW( L"compSpecsAddress3: %p\n", compSpecsAddress3 );
	*compSpecsAddress3 = 0x9090;*/

	// This will fix acog weapons not scoping when cl_use_new_bob is disabled
	BYTE* newBobFix = (BYTE*) SearchPattern( L"client.dll", "\x85\xC0\x75\x17\xFF\x75\x10" );
	ConsoleDebugW( L"newBobFix: %p\n", newBobFix );
	newBobFix[ 0 ] = 0x90;
	newBobFix[ 1 ] = 0x90;
	newBobFix[ 2 ] = 0xEB; // jmp short

	// This will disable cs from trying to update workshop files, leave that to regular cs
	BYTE* workshopAddress = (BYTE*) SearchPattern( L"client.dll", "\x55\x8B\xEC\x83\xE4\xF8\x80\x3D" );
	ConsoleDebugW( L"workshopAddress: %p\n", workshopAddress );
	workshopAddress[ 0 ] = 0xC2;
	workshopAddress[ 1 ] = 0x04;
	workshopAddress[ 2 ] = 0x00;

	// We may or may not need this later on to save our config in the mod folder. I had some issues
	// changing the remote steam config file.
	/*DWORD execUsrlocalAddress = (DWORD) GetModuleHandleW( L"engine.dll" ) + 0x2217E5;
	ConsoleDebugW( L"execUsrlocalAddress: %X\n", execUsrlocalAddress );
	WriteToCodeSection( execUsrlocalAddress, &noop, 45, true );*/
}

extern "C" __declspec(dllexport) bool HookClient()
{	
	ConsoleDebugW( L"HookClient called!\n" );

	g_dwClientBase = (uintptr_t) GetModuleHandleW( L"client.dll" );
	OnClientAttach();		  	

	return true;
}