#include "stdafx.h"
#include "CGameDLL_ConVarAccessor.h"

PLH::Detour* registerConCommandBaseHook = new PLH::Detour;

void SetOtherCvars()
{
	// We can set these to overwrite the hardcoded values and not worry about config files since they're exec'd later on
	// Probably.

	ConVar* cl_use_new_headbob = cvar->FindVar( "cl_use_new_headbob" );

	if ( cl_use_new_headbob )
	{
		cl_use_new_headbob->SetValue( 0 );
		cl_use_new_headbob->m_pszDefaultValue = "0";
	}

	// Viewmodel values based from http://steamcommunity.com/app/730/discussions/0/540744934887760005/#c412449508287000307
	ConVar* viewmodel_fov = cvar->FindVar( "viewmodel_fov" );

	if ( viewmodel_fov )
	{
		viewmodel_fov->SetValue( 90.0f );
		viewmodel_fov->m_pszDefaultValue = "90";
	}

	ConVar* viewmodel_offset_x = cvar->FindVar( "viewmodel_offset_x" );

	if ( viewmodel_offset_x )
	{
		viewmodel_offset_x->SetValue( 6.0f );
		viewmodel_offset_x->m_pszDefaultValue = "6";
	}

	ConVar* viewmodel_offset_y = cvar->FindVar( "viewmodel_offset_y" );

	if ( viewmodel_offset_y )
	{
		viewmodel_offset_y->SetValue( 2.0f );
		viewmodel_offset_y->m_pszDefaultValue = "2";
	}

	ConVar* viewmodel_offset_z = cvar->FindVar( "viewmodel_offset_z" );

	if ( viewmodel_offset_z )
	{
		viewmodel_offset_z->SetValue( -5.0f );
		viewmodel_offset_z->m_pszDefaultValue = "-5";
	}

	ConVar* viewmodel_presetpos = cvar->FindVar( "viewmodel_presetpos" );

	if ( viewmodel_presetpos )
	{
		viewmodel_presetpos->SetValue( 0 );
		viewmodel_presetpos->m_pszDefaultValue = "0";
	}

	ConVar* cl_bob_lower_amt = cvar->FindVar( "cl_bob_lower_amt" );

	if ( cl_bob_lower_amt )
	{
		cl_bob_lower_amt->SetValue( 5.0f );
		cl_bob_lower_amt->m_pszDefaultValue = "5.0";
	}

	ConVar* cl_bobamt_lat = cvar->FindVar( "cl_bobamt_lat" );

	if ( cl_bobamt_lat )
	{
		cl_bobamt_lat->SetValue( 0.1f );
		cl_bobamt_lat->m_pszDefaultValue = "0.1";
	}

	ConVar* cl_bobamt_vert = cvar->FindVar( "cl_bobamt_vert" );

	if ( cl_bobamt_vert )
	{
		cl_bobamt_vert->SetValue( 0.1f );
		cl_bobamt_vert->m_pszDefaultValue = "0.1";
	}

	ConVar* cl_bobcycle = cvar->FindVar( "cl_bobcycle" );

	if ( cl_bobcycle )
	{
		cl_bobcycle->SetValue( 0.98f );
		cl_bobcycle->m_pszDefaultValue = "0.98";
	}

	ConVar* cl_viewmodel_shift_left_amt = cvar->FindVar( "cl_viewmodel_shift_left_amt" );

	if ( cl_viewmodel_shift_left_amt )
	{
		cl_viewmodel_shift_left_amt->SetValue( 0.5f );
		cl_viewmodel_shift_left_amt->m_pszDefaultValue = "0.5";
	}

	ConVar* cl_viewmodel_shift_right_amt = cvar->FindVar( "cl_viewmodel_shift_right_amt" );

	if ( cl_viewmodel_shift_right_amt )
	{
		cl_viewmodel_shift_right_amt->SetValue( 0.25f );
		cl_viewmodel_shift_right_amt->m_pszDefaultValue = "0.25";
	}

	ConVar* sv_penetration_type = cvar->FindVar( "sv_penetration_type" );
	ConsoleDebugW( L"sv_penetration_type: %p\n", sv_penetration_type );

	if ( sv_penetration_type )
	{
		sv_penetration_type->SetValue( 0 );
		sv_penetration_type->m_pszDefaultValue = "0";
		sv_penetration_type->RemoveFlags( FCVAR_HIDDEN );
		sv_penetration_type->RemoveFlags( FCVAR_DEVELOPMENTONLY );				
		ConsoleDebugW( L"Changed sv_penetration_type value and flags...\n" );
	}
}

bool __fastcall hkRegisterConCommandBase( IConCommandBaseAccessor* thisptr, void* edx, ConCommandBase* pCommand )
{
	using fn_t = bool( __thiscall* )(IConCommandBaseAccessor*, ConCommandBase*);

	static bool bSetOthers = false;

	if ( !bSetOthers )
	{
		bSetOthers = true;
		SetOtherCvars(); // Maybe find a better place to do this?
	}

	auto res = registerConCommandBaseHook->GetOriginal< fn_t >()(thisptr, pCommand);

	return res;
}

void HookGameDLL_ConVarAccessor()
{
	registerConCommandBaseHook->SetupHook( (BYTE*) Addresses::RegisterConCommandBase, (BYTE*) hkRegisterConCommandBase );
	registerConCommandBaseHook->Hook();
}