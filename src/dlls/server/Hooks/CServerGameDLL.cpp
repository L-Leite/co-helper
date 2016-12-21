#include "stdafx.h"
#include "CServerGameDLL.h"

PLH::Detour* dllShutdownHook = new PLH::Detour( false ); // We'll handle our own unhooking

void __fastcall hkDllShutdown( IServerGameDLL* thisptr )
{
	using fn_t = void(__thiscall*)( IServerGameDLL* );

	ConsoleDebugA( "Server DLL shutdown called!\n" );

	PLH::DestroyAll();

	dllShutdownHook->GetOriginal< fn_t >()(thisptr); 	
}

void HookServerGameDLL()
{		   
	dllShutdownHook->SetupHook( (BYTE*) Addresses::DLLShutdown, (BYTE*) &hkDllShutdown );
	dllShutdownHook->Hook();
}