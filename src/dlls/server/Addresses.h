#pragma once

namespace Addresses
{ 
extern DWORD EmitSound;
extern DWORD AmmoDef;
extern DWORD GiveAmmo;
extern DWORD HintMessage;
extern DWORD PrecacheModel;
extern DWORD AddModel;
extern DWORD ShouldRunRateLimitedCommand;
extern DWORD StateTransition;
extern DWORD CheckWinConditions;
extern DWORD IsThereABomber;
extern DWORD IsThereABomb;
extern DWORD GiveC4;
extern DWORD SetFOV;
extern DWORD RegisterConCommandBase;
extern DWORD ClientCommand;
extern DWORD SetModelFromClass;
extern DWORD Precache;
extern DWORD ChangeTeam;
extern DWORD HandleCommand_JoinClass;
extern DWORD GetIntoGame;
extern DWORD DLLShutdown;
extern DWORD Duck;
extern DWORD FinishUnDuck;
extern DWORD TraceHull;
extern DWORD Holster_hpk2000;
extern DWORD Holster;
extern DWORD BloodDrips;
extern DWORD DispatchEffect;
extern DWORD ApplyMultiDamage;
extern DWORD AddMultiDamage;
//extern DWORD ClearMultiDamage;	
extern DWORD MultiDamageInit;
extern DWORD CalculateBulletDamageForce;
extern DWORD LockStudioHdr;
extern DWORD CalcAbsolutePosition;
extern DWORD BulletGroupCounter;
extern DWORD FireBullet;
extern DWORD PostThink;
extern DWORD CSViewVectors;
extern DWORD MultiDamage;
}

void GetAddresses();