#include "stdafx.h"

namespace Addresses
{
DWORD EmitSound = 0;
DWORD AmmoDef = 0;
DWORD GiveAmmo = 0;
DWORD HintMessage = 0;
DWORD PrecacheModel = 0;
DWORD AddModel = 0;
DWORD ShouldRunRateLimitedCommand = 0;
DWORD StateTransition = 0;
DWORD CheckWinConditions = 0;
DWORD IsThereABomber = 0;
DWORD IsThereABomb = 0;
DWORD GiveC4 = 0;
DWORD SetFOV = 0;
DWORD RegisterConCommandBase = 0;
DWORD ClientCommand = 0;
DWORD SetModelFromClass = 0;
DWORD Precache = 0;
DWORD ChangeTeam = 0;
DWORD HandleCommand_JoinClass = 0;
DWORD GetIntoGame = 0;
DWORD DLLShutdown = 0;
DWORD Duck = 0;
DWORD FinishDuck = 0;
}

void GetAddresses()
{
	Addresses::EmitSound = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x70\x56\x57\x8B\xF9\x8B\x0D\xCC\xCC\xCC\xCC\x83\xB9\xCC\xCC\xCC\xCC\xCC" );
	ConsoleDebugW( L"EmitSound: %X\n", Addresses::EmitSound );

	Addresses::AmmoDef = SearchPattern( L"server.dll", "\xB9\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x8B\xF0\x85\xF6\x78\xD8" ) + 1;
	ConsoleDebugW( L"AmmoDef: %X\n", Addresses::AmmoDef );

	Addresses::GiveAmmo = SearchPattern( L"server.dll", "\x55\x8B\xEC\x53\x8B\x5D\x14\x85\xDB" );
	ConsoleDebugW( L"GiveAmmo: %X\n", Addresses::GiveAmmo );

	Addresses::HintMessage = SearchPattern( L"server.dll", "\x55\x8B\xEC\x51\x53\x8B\xD9\x89\x5D\xFC" );
	ConsoleDebugW( L"HintMessage: %X\n", Addresses::HintMessage );

	Addresses::PrecacheModel = SearchPattern( L"server.dll", "\x56\x8B\xF1\x85\xF6\x74\x4F" );
	ConsoleDebugW( L"PrecacheModel: %X\n", Addresses::PrecacheModel );

	Addresses::AddModel = SearchPattern( L"server.dll", "\x55\x8B\xEC\x81\xEC\xCC\xCC\xCC\xCC\x56\x57\x8B\x7D\x08\x8B\xF1\x85\xFF\x0F\x84" );
	ConsoleDebugW( L"AddModel: %X\n", Addresses::AddModel );

	Addresses::ShouldRunRateLimitedCommand = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x10\x53\x56\x57\x8B\x7D\x08\x8B\xD9\x83\x3F\xCC" );
	ConsoleDebugW( L"ShouldRunRateLimitedCommand: %X\n", Addresses::ShouldRunRateLimitedCommand );

	Addresses::StateTransition = SearchPattern( L"server.dll", "\x55\x8B\xEC\x56\x8B\xF1\x8B\x8E\xCC\xCC\xCC\xCC\x85\xC9\x74\x0E" );
	ConsoleDebugW( L"StateTransition: %X\n", Addresses::StateTransition );

	Addresses::CheckWinConditions = SearchPattern( L"server.dll", "\x55\x8B\xEC\xA1\xCC\xCC\xCC\xCC\x83\xEC\x18\x56\x8B\xF1\xB9\xCC\xCC\xCC\xCC\x57" );
	ConsoleDebugW( L"CheckWinConditions: %X\n", Addresses::CheckWinConditions );

	Addresses::IsThereABomber = SearchPattern( L"server.dll", "\x8B\x0D\xCC\xCC\xCC\xCC\x53\x56\x57\xBF" );
	ConsoleDebugW( L"IsThereABomber: %X\n", Addresses::IsThereABomber );

	Addresses::IsThereABomb = SearchPattern( L"server.dll", "\x53\x68\xCC\xCC\xCC\xCC\x6A\xCC\x32\xDB" );
	ConsoleDebugW( L"IsThereABomb: %X\n", Addresses::IsThereABomb );

	Addresses::GiveC4 = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x20\x53\x56\x57\x8D\x45\xFF" );
	ConsoleDebugW( L"GiveC4: %X\n", Addresses::GiveC4 );

	Addresses::SetFOV = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x08\x8B\x55\x08\xF3\x0F\x11\x5D" );
	ConsoleDebugW( L"SetFOV: %X\n", Addresses::SetFOV );

	Addresses::RegisterConCommandBase = SearchPattern( L"server.dll", "\x55\x8B\xEC\x53\x56\x57\x8B\x7D\x08\x8B\xCF" );
	ConsoleDebugW( L"RegisterConCommandBase: %X\n", Addresses::RegisterConCommandBase );	   

	Addresses::ClientCommand = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x5C\x53\x8B\x5D\x08\x56\x57\x8B\xF1" );
	ConsoleDebugW( L"ClientCommand: %X\n", Addresses::ClientCommand );

	Addresses::SetModelFromClass = SearchPattern( L"server.dll", "\x55\x8B\xEC\x51\x56\x57\x8B\xF9\x8B\x87" );
	ConsoleDebugW( L"SetModelFromClass: %X\n", Addresses::SetModelFromClass );

	Addresses::Precache = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x1C\x53\x56\x57\x8B\xF9\xC7\x45" );
	ConsoleDebugW( L"Precache: %X\n", Addresses::Precache );

	Addresses::ChangeTeam = SearchPattern( L"server.dll", "\xE8\xCC\xCC\xCC\xCC\x8B\x0D\xCC\xCC\xCC\xCC\x8D\x45\x08\x50\x8D\x45\xF0" );
	ConsoleDebugW( L"ChangeTeam: %X\n", Addresses::ChangeTeam );

	Addresses::HandleCommand_JoinClass = SearchPattern( L"server.dll", "\x53\x56\x8B\xF1\x33\xDB\x57\x8B\x8E" );
	ConsoleDebugW( L"HandleCommand_JoinClass: %X\n", Addresses::HandleCommand_JoinClass );

	Addresses::GetIntoGame = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xE4\xC0\x83\xEC\x34\x0F\x57\xDB" );
	ConsoleDebugW( L"GetIntoGame: %X\n", Addresses::GetIntoGame );

	Addresses::DLLShutdown = SearchPattern( L"server.dll", "\x55\x8B\xEC\x51\x8B\x0D\xCC\xCC\xCC\xCC\x8B\x01\x8B\x80" );
	ConsoleDebugW( L"DLLShutdown: %X\n", Addresses::DLLShutdown );

	Addresses::Duck = SearchPattern( L"server.dll", "\x55\x8B\xEC\x81\xEC\xCC\xCC\xCC\xCC\x56\x57\x8B\xF9\x8B\x57\x04" );
	ConsoleDebugW( L"Duck: %X\n", Addresses::Duck );

	Addresses::FinishDuck = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x18\x56\x57\x8B\xF9\x8B\x47\x08\x8B\x57\x04\xF3\x0F\x7E\x80\xCC\xCC\xCC\xCC\x8B\x8A\xCC\xCC\xCC\xCC\x8B\x80\xCC\xCC\xCC\xCC\x66\x0F\xD6\x45\xCC\x89\x45\xF0\x83\xF9\xFF\x74\x20\x0F\xB7\xC1\xC1\xE9\x10\x8D\x04\x40\x39\x0C\xC5" );
	ConsoleDebugW( L"FinishDuck: %X\n", Addresses::FinishDuck );


	g_pEntityList = *(CBaseEntityList**)( SearchPattern( L"server.dll", "\xB9\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x51\x8B\x0E" ) + 1 );
	ConsoleDebugW( L"g_pEntityList: %p\n", g_pEntityList );

	g_pPlayerStateInfos = *(CCSPlayerStateInfo**) ( SearchPattern( L"server.dll", "\x8D\x04\xC5\xCC\xCC\xCC\xCC\x8B\xE5" ) + 3 );
	ConsoleDebugW( L"g_pPlayerStateInfos: %p\n", g_pPlayerStateInfos );

	g_pGameRules = *(CGameRules***) (SearchPattern( L"server.dll", "\x89\x1D\xCC\xCC\xCC\xCC\x8B\x40\x18" ) + 2);
	ConsoleDebugW( L"g_pGameRules: %p\n", g_pGameRules );
}