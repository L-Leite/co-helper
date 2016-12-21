#include "stdafx.h"

#define DOUBLEDUCK_JUMP_HEIGHT 32.0f

PLH::Detour* duckHook = new PLH::Detour;
PLH::Detour* finishUnduckHook = new PLH::Detour;

bool g_bShouldDoubleDuck = false;

void __fastcall hkDuck( CCSGameMovement* thisptr )
{
	using fn_t = void( __thiscall* )(CCSGameMovement*);

	if ( thisptr->player->GetFlags() & FL_ONGROUND )
	{
		static bool bCanDoubleDuck = false;

		if ( thisptr->player->GetFlags() & FL_DUCKING )
			bCanDoubleDuck = false;
		else if ( thisptr->mv->m_nButtons & IN_DUCK )
			bCanDoubleDuck = true;
		else if ( thisptr->player->m_Local.m_bDucking && bCanDoubleDuck )
			g_bShouldDoubleDuck = true;
	}

	duckHook->GetOriginal<fn_t>()(thisptr);
}

void __fastcall hkFinishUnDuck( CCSGameMovement* thisptr )
{
	using fn_t = void( __thiscall* )(CCSGameMovement*);

	if ( g_bShouldDoubleDuck )
	{
		Vector origin = thisptr->mv->GetAbsOrigin();
		origin.z += DOUBLEDUCK_JUMP_HEIGHT;

		trace_t trace;

		//UTIL_TraceHull( thisptr->mv->GetAbsOrigin(), origin, VEC_HULL_MIN, VEC_HULL_MAX, thisptr->PlayerSolidMask(), thisptr->player, COLLISION_GROUP_PLAYER_MOVEMENT, &trace );

		//if ( !trace.startsolid && trace.fraction == 1.0f )
			thisptr->mv->SetAbsOrigin( origin );

		g_bShouldDoubleDuck = false;
	}

	finishUnduckHook->GetOriginal<fn_t>()(thisptr);
}

void HookCSGameMovement()
{
	duckHook->SetupHook( (BYTE*) Addresses::Duck, (BYTE*) &hkDuck );
	duckHook->Hook();

	finishUnduckHook->SetupHook( (BYTE*) Addresses::FinishUnDuck, (BYTE*) &hkFinishUnDuck );
	finishUnduckHook->Hook();
}