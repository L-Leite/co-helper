#include "stdafx.h"
#include "ConVars.h"

void OpenClassMenu( const CCommand& command );
ConCommand classmenu( "classmenu", OpenClassMenu, "Opens class menu" );

class CMenuParams
{		
	// Can't remember if this is on the sdk already
	class customstring
	{
	public:
		customstring( char* menu )
		{
			m_szMenu = menu;
			m_strLength = strlen( menu );
		}

	public:
		char*			m_szMenu;
		uint8_t Pad[ 0x10 ];
		int m_strLength;	 
	};

public:
	CMenuParams() {}
	CMenuParams( char* menu, int bitsValidSlots, float shutoffTime )
	{	 
		m_szMenu = new customstring( menu );
		m_bitsValidSlots = bitsValidSlots;
		m_flShutoffTime = shutoffTime;
	}

public:
	uint8_t Pad[ 0x8 ];
	int				m_bitsValidSlots;
	float			m_flShutoffTime;
	customstring*			m_szMenu;
};

int GetWhichMenu()
{					 
	return *(int*) Addresses::whichMenu;
}

void SetWhichMenu( int which )
{
	*(int*) Addresses::whichMenu = which;
}

bool OpenMenu( CMenuParams* menuParms )
{
	using fn_t = bool(*)(CMenuParams*);   
	return ((fn_t)Addresses::OpenMenu)( menuParms );
}

void OpenClassMenu( const CCommand& command )
{
	ConsoleDebugW( L"OpenClassMenu called!\n" );

	// Only the local player should be able to call this anyway
	IClientEntity* localPlayer = entitylist->GetClientEntity( g_pEngineClient->GetLocalPlayer() );

	if ( !localPlayer )
	{
		ConsoleDebugW( L"OpenClassMenu: couldn't get local player!\n" );
		return;
	}

	int teamNum = *(int*) ((DWORD) localPlayer + 0xF0); // Gotta reverse client CSPlayer

	ConsoleDebugW( L"OpenClassMenu: local player team: %i\n", teamNum );

	char* menuString = nullptr;

	if ( teamNum == TEAM_CT )
		menuString = "#CSCO_ClassMenu_CT";
	else if ( teamNum == TEAM_TERRORIST )
		menuString = "#CSCO_ClassMenu_TER";
	else
	{
		ConsoleDebugW( L"OpenClassMenu: local player is invalid\n" );
		return;
	}

	CMenuParams menuParms( menuString, 0x23f, -1.f );

	SetWhichMenu( 0 );
	//*(int*) (g_dwClientBase + 0x2F0A194) = 0;

	OpenMenu( &menuParms );
}				

void RegisterClientConVars()
{
	cvar->RegisterConCommand( &classmenu );
}