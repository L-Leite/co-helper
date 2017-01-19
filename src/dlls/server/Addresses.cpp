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
DWORD FinishUnDuck = 0;
DWORD TraceHull = 0;
DWORD Holster_hpk2000 = 0;
DWORD Holster = 0;
DWORD BloodDrips = 0;
DWORD DispatchEffect = 0;
DWORD ApplyMultiDamage = 0;
DWORD ApplyMultiDamage2 = 0;
DWORD ClearMultiDamage = 0;
DWORD CalculateBulletDamageForce = 0;
DWORD LockStudioHdr = 0;
DWORD CalcAbsolutePosition = 0;
DWORD BulletGroupCounter = 0;
}

void GetAddresses()
{
	ConsoleDebugW( L"\n### SERVER ADDRESSES START\n" );

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

	Addresses::FinishUnDuck = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x18\x56\x57\x8B\xF9\x8B\x47\x08\x8B\x57\x04\xF3\x0F\x7E\x80\xCC\xCC\xCC\xCC\x8B\x8A\xCC\xCC\xCC\xCC\x8B\x80\xCC\xCC\xCC\xCC\x66\x0F\xD6\x45\xCC\x89\x45\xF0\x83\xF9\xFF\x74\x20\x0F\xB7\xC1\xC1\xE9\x10\x8D\x04\x40\x39\x0C\xC5" );
	ConsoleDebugW( L"FinishUnDuck: %X\n", Addresses::FinishUnDuck );

	Addresses::TraceHull = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xE4\xF0\x81\xEC\xCC\xCC\xCC\xCC\x56\xFF\x75\x0C" );
	ConsoleDebugW( L"TraceHull: %X\n", Addresses::TraceHull );

	Addresses::Holster_hpk2000 = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x0C\x53\x56\x8B\xF1\x57\x8B\x86\xCC\xCC\xCC\xCC\x3D\xCC\xCC\xCC\xCC\x75\x09\x80\xBE\xCC\xCC\xCC\xCC\xCC\x74\x18\x3D\xCC\xCC\xCC\xCC\x0F\x85\xCC\xCC\xCC\xCC\x80\xBE\xCC\xCC\xCC\xCC\xCC\x0F\x85\xCC\xCC\xCC\xCC\x8B\x1D\xCC\xCC\xCC\xCC\xF3\x0F\x10\x86" );
	ConsoleDebugW( L"Holster_hpk2000: %X\n", Addresses::Holster_hpk2000 );

	Addresses::Holster = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x10\x53\x56\x8B\x35\xCC\xCC\xCC\xCC\x57" );
	ConsoleDebugW( L"Holster: %X\n", Addresses::Holster );

	Addresses::BloodDrips = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x74\x53\x8B\x5D\x08\x89\x55\xFC" );
	ConsoleDebugW( L"BloodDrips: %X\n", Addresses::BloodDrips );

	Addresses::DispatchEffect = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x20\x56\x8B\xF1\x8D\x4C\x24\x04" );
	ConsoleDebugW( L"DispatchEffect: %X\n", Addresses::DispatchEffect );

	Addresses::ApplyMultiDamage = SearchPattern( L"server.dll", "\xA1\xCC\xCC\xCC\xCC\x56\x83\xF8\xFF\x74\x66" );
	ConsoleDebugW( L"ApplyMultiDamage: %X\n", Addresses::ApplyMultiDamage );

	Addresses::ApplyMultiDamage2 = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x0C\x53\x56\x57\x8B\xFA\x8B\xF1\x85\xFF" );
	ConsoleDebugW( L"ApplyMultiDamage2: %X\n", Addresses::ApplyMultiDamage2 );

	Addresses::ClearMultiDamage = SearchPattern( L"server.dll", "\xF3\x0F\x10\x15\xCC\xCC\xCC\xCC\xF3\x0F\x10\x0D\xCC\xCC\xCC\xCC\xF3\x0F\x10\x05\xCC\xCC\xCC\xCC\xA1" );
	ConsoleDebugW( L"ClearMultiDamage: %X\n", Addresses::ClearMultiDamage );

	Addresses::CalculateBulletDamageForce = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xEC\x14\x53\x56\x8B\x75\x0C\x8B\xD9" );
	ConsoleDebugW( L"CalculateBulletDamageForce: %X\n", Addresses::CalculateBulletDamageForce );

	Addresses::LockStudioHdr = SearchPattern( L"server.dll", "\x57\x8B\xF9\x83\xBF\xCC\xCC\xCC\xCC\xCC\x0F\x84\xCC\xCC\xCC\xCC\xA1\xCC\xCC\xCC\xCC\x56" );
	ConsoleDebugW( L"LockStudioHdr: %X\n", Addresses::LockStudioHdr );

	Addresses::CalcAbsolutePosition = SearchPattern( L"server.dll", "\x55\x8B\xEC\x83\xE4\xF0\x83\xEC\x68\x56\x8B\xF1\x57\x8B\x8E" );
	ConsoleDebugW( L"CalcAbsolutePosition: %X\n", Addresses::CalcAbsolutePosition );


	g_pEntityList = *(CBaseEntityList**)( SearchPattern( L"server.dll", "\xB9\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x51\x8B\x0E" ) + 1 );
	ConsoleDebugW( L"g_pEntityList: %p\n", g_pEntityList );
	gEntList = (CGlobalEntityList*) g_pEntityList;

	g_pPlayerStateInfos = *(CCSPlayerStateInfo**) ( SearchPattern( L"server.dll", "\x8D\x04\xC5\xCC\xCC\xCC\xCC\x8B\xE5" ) + 3 );
	ConsoleDebugW( L"g_pPlayerStateInfos: %p\n", g_pPlayerStateInfos );

	g_pGameRules = *(CGameRules***) (SearchPattern( L"server.dll", "\x89\x1D\xCC\xCC\xCC\xCC\x8B\x40\x18" ) + 2);
	ConsoleDebugW( L"g_pGameRules: %p\n", g_pGameRules );

	//g_pTE = *(ITempEntsSystem***) (SearchPattern( L"server.dll", "\xB9\xCC\xCC\xCC\xCC\xFF\x50\x50\x8D\x4C\x24\x50" ) + 1);
	//ConsoleDebugW( L"&te: %p\n", g_pTE );

	CBaseEntity::m_nDebugPlayer = *(int**) (SearchPattern( L"server.dll", "\x8B\x15\xCC\xCC\xCC\xCC\x83\xEC\x3C" ) + 2);
	ConsoleDebugW( L"&CBaseEntity::m_nDebugPlayer: %p\n", CBaseEntity::m_nDebugPlayer );

	CCSPlayer::s_BulletGroupCounter = *(int**) (SearchPattern( L"server.dll", "\xA1\xCC\xCC\xCC\xCC\x51\x88\x4F\x5C" ) + 1);
	ConsoleDebugW( L"&CCSPlayer::s_BulletGroupCounter: %p\n", CCSPlayer::s_BulletGroupCounter );

	ConsoleDebugW( L"### SERVER ADDRESSES END\n\n" );
}