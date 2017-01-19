#include "stdafx.h"

ConVar* friendlyfire = nullptr;		 

ConVar* r_visualizetraces = nullptr;   

ConVar* sv_showimpacts = nullptr;
ConVar* sv_showimpacts_time = nullptr;
ConVar* sv_showplayerhitboxes = nullptr;

void InitializeConVars()
{
	ConsoleDebugW( L"\n\n### Server convars start\n" );

	ConVar_Register();

	friendlyfire = cvar->FindVar( "mp_friendlyfire" );
	r_visualizetraces = cvar->FindVar( "r_visualizetraces" );
	sv_showimpacts = cvar->FindVar( "sv_showimpacts" );
	sv_showimpacts_time = cvar->FindVar( "sv_showimpacts_time" );
	sv_showplayerhitboxes = cvar->FindVar( "sv_showplayerhitboxes" );

	ConsoleDebugW( L"mp_friendlyfire: %p\n", friendlyfire );
	ConsoleDebugW( L"r_visualizetraces: %p\n", r_visualizetraces );
	ConsoleDebugW( L"sv_showimpacts: %p\n", sv_showimpacts );
	ConsoleDebugW( L"sv_showimpacts_time: %p\n", sv_showimpacts_time );
	ConsoleDebugW( L"sv_showplayerhitboxes: %p\n", sv_showplayerhitboxes );

	ConsoleDebugW( L"### Server convars end\n\n" );
}