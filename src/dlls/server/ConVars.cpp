#include "stdafx.h"

ConVar* friendlyfire = nullptr;		 
ConVar* mp_buytime = nullptr;
ConVar* mp_buy_anywhere = nullptr;
ConVar* mp_c4timer = nullptr;
ConVar* mp_freezetime = nullptr;
ConVar* mp_playerid = nullptr;
ConVar* mp_autoteambalance = nullptr;

ConVar* r_visualizetraces = nullptr;   

ConVar* sv_showimpacts = nullptr;
ConVar* sv_showimpacts_time = nullptr;
ConVar* sv_showplayerhitboxes = nullptr;

void InitializeConVars()
{
	ConsoleDebugW( L"\n\n### Server convars start\n" );

	ConVar_Register();

	friendlyfire = cvar->FindVar( "mp_friendlyfire" );
	mp_buytime = cvar->FindVar( "mp_buytime" );
	mp_buy_anywhere = cvar->FindVar( "mp_buy_anywhere" );
	mp_c4timer = cvar->FindVar( "mp_c4timer" );
	mp_freezetime = cvar->FindVar( "mp_freezetime" );
	mp_playerid = cvar->FindVar( "mp_playerid" );
	mp_autoteambalance = cvar->FindVar( "mp_autoteambalance" );
	r_visualizetraces = cvar->FindVar( "r_visualizetraces" );
	sv_showimpacts = cvar->FindVar( "sv_showimpacts" );
	sv_showimpacts_time = cvar->FindVar( "sv_showimpacts_time" );
	sv_showplayerhitboxes = cvar->FindVar( "sv_showplayerhitboxes" );
	
	ConsoleDebugW( L"mp_buytime: %p\n", mp_buytime );
	ConsoleDebugW( L"mp_buy_anywhere: %p\n", mp_buy_anywhere );
	ConsoleDebugW( L"mp_friendlyfire: %p\n", friendlyfire );
	ConsoleDebugW( L"mp_c4timer: %p\n", mp_c4timer );
	ConsoleDebugW( L"mp_freezetime: %p\n", mp_freezetime );
	ConsoleDebugW( L"mp_playerid: %p\n", mp_playerid );
	ConsoleDebugW( L"mp_autoteambalance: %p\n", mp_autoteambalance );
	ConsoleDebugW( L"r_visualizetraces: %p\n", r_visualizetraces );
	ConsoleDebugW( L"sv_showimpacts: %p\n", sv_showimpacts );
	ConsoleDebugW( L"sv_showimpacts_time: %p\n", sv_showimpacts_time );
	ConsoleDebugW( L"sv_showplayerhitboxes: %p\n", sv_showplayerhitboxes );

	ConsoleDebugW( L"### Server convars end\n\n" );
}