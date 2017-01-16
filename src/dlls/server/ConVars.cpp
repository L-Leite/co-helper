#include "stdafx.h"

ConVar* sv_showimpacts = nullptr;
ConVar* sv_showplayerhitboxes = nullptr;

void InitializeConVars()
{
	ConsoleDebugW( L"### Server convars start" );

	sv_showimpacts = cvar->FindVar( "sv_showimpacts" );
	sv_showplayerhitboxes = cvar->FindVar( "sv_showplayerhitboxes" );

	ConsoleDebugW( L"sv_showimpacts: %p\n", sv_showimpacts );
	ConsoleDebugW( L"sv_showplayerhitboxes: %p\n", sv_showplayerhitboxes );

	ConsoleDebugW( L"### Server convars end" );
}