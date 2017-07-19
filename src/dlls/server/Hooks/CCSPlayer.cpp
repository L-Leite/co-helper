#include "stdafx.h"
#include "CCSPlayer.h"														 

PLH::Detour* giveAmmoHook = new PLH::Detour;
PLH::Detour* clientCommandHook = new PLH::Detour;
PLH::Detour* setModelClassHook = new PLH::Detour;
PLH::Detour* precacheHook = new PLH::Detour;
PLH::Detour* handleCommand_JoinClassHook = new PLH::Detour;
PLH::Detour* secondPrimaryHook = new PLH::Detour;
PLH::Detour* holster_UspHook = new PLH::Detour;
PLH::Detour* fireBulletHook = new PLH::Detour;
PLH::Detour* postThinkHook = new PLH::Detour;
PLH::Detour* setFovHook = new PLH::Detour;

void __fastcall hkGiveAmmo( CWeaponCSBase* thisptr, void* edx, int a1, int iCount, bool bSuppressSound, int a2 )
{
	using fn_t = void( __thiscall* )(CWeaponCSBase*, int, int, bool, int);

	ConsoleDebugW( L"tried to give %i ammo for %S retn address: %p\n", iCount, thisptr->GetNetworkable()->GetClassName(), _ReturnAddress() );

	if ( iCount == 250 )
	{
		CCSWeaponInfo* wepInfo = (CCSWeaponInfo*) thisptr->GetCSWpnData();

		iCount = wepInfo->GetPrimaryClipSize( thisptr->GetAttributeContainer()->GetItem(), false ) * 2;

		ConsoleDebugW( L"Changed iCount to %i\n", iCount );
	}

	return giveAmmoHook->GetOriginal<fn_t>()(thisptr, a1, iCount, bSuppressSound, a2);
}

void BuyWeaponAmmo( CCSPlayer* player, CWeaponCSBase* weapon )
{
	CCSWeaponInfo* wepInfo = (CCSWeaponInfo*) weapon->GetCSWpnData();
	CCSAmmoDef* ammoDef = GetCSAmmoDef();

	ConsoleDebugW( L"buying ammo for %S %p\n", weapon->GetNetworkable()->GetClassName(), weapon );

	ConsoleDebugW( L"primary->m_iPrimaryReserveAmmoCount: %i ammoType: %i reserve ammo: %i maxclip: %i ammoCost: %i buySize: %i\n", weapon->m_iPrimaryReserveAmmoCount.Get(), weapon->GetPrimaryAmmoType(), wepInfo->GetPrimaryReserveAmmo( weapon->GetAttributeContainer()->GetItem(), false ), wepInfo->iMaxClip1, ammoDef->GetCost( weapon->m_iPrimaryAmmoType ), ammoDef->GetBuySize( weapon->m_iPrimaryAmmoType ) );
	ConsoleDebugW( L"%S::m_iAccount: %i\n", player->GetPlayerName(), player->GetAccount() );

	int ammoCost = ammoDef->GetCost( weapon->m_iPrimaryAmmoType );

	if ( !player->CanPlayerBuy( true ) )
		return;

	if ( player->GetAccount() < ammoCost )
	{
		player->HintMessage( "#Cstrike_TitlesTXT_Not_Enough_Money", false, true );
		return;
	}

	if ( weapon->m_iPrimaryReserveAmmoCount < wepInfo->GetPrimaryReserveAmmo( weapon->GetAttributeContainer()->GetItem(), false ) )
	{
		weapon->GiveAmmo( 1, weapon->m_iPrimaryReserveAmmoCount + ammoDef->GetBuySize( weapon->m_iPrimaryAmmoType ), true, 0 );
		player->AddAccount( -ammoCost );
		player->EmitSound( "BuyMenu.BuyAmmo", player );
	}
}

bool __fastcall hkClientCommand( CCSPlayer* thisptr, void* edx, const CCommand &args )
{
	using fn_t = bool( __thiscall* )(CCSPlayer*, const CCommand&);

	const char* pcmd = args[ 0 ];

	ConsoleDebugW( L"ServerCmd: %S called\n", pcmd );

	if ( FStrEq( pcmd, "buyammo1" ) )
	{
		CWeaponCSBase* primary = static_cast<CWeaponCSBase*>(thisptr->Weapon_GetSlot( WEAPON_SLOT_RIFLE ));

		if ( primary )
			BuyWeaponAmmo( thisptr, primary );

		return true;
	}

	else if ( FStrEq( pcmd, "buyammo2" ) )
	{
		CWeaponCSBase* secondary = static_cast<CWeaponCSBase*>(thisptr->Weapon_GetSlot( WEAPON_SLOT_PISTOL ));

		if ( secondary )
			BuyWeaponAmmo( thisptr, secondary );

		return true;
	}

	else if ( FStrEq( pcmd, "joinclass" ) )
	{
		if ( args.ArgC() < 2 )
		{
			Warning( "Player sent bad joinclass syntax. Syntax: joinclass [class number]\n" );
			return true;
		}

		if ( thisptr->ShouldRunRateLimitedCommand( args ) )
		{
			int iClass = atoi( args[ 1 ] );
			thisptr->HandleCommand_JoinClass( iClass );
		}

		return true;
	}

	else if ( FStrEq( pcmd, "menuselect" ) )
	{
		int iClass = atoi( args[ 1 ] );

		if ( thisptr->GetTeamNumber() == TEAM_CT )
			iClass += LAST_T_CLASS;

		ConsoleDebugW( L"ClientCommand: picking class %i\n", iClass );

		thisptr->HandleCommand_JoinClass( iClass );

		return true;
	}

	return clientCommandHook->GetOriginal<fn_t>()(thisptr, args);
}

void __fastcall hkSetModelFromClass( CCSPlayer* thisptr )
{
	using fn_t = void( __thiscall* )(CCSPlayer*);

	ConsoleDebugW( L"Setting Class model...\n" );

	thisptr->SetModelFromClass();
}

void __fastcall hkPrecache( CCSPlayer* thisptr )
{
	using fn_t = void( __thiscall* )(CCSPlayer*);

	Vector mins( -13, -13, -10 );
	Vector maxs( 13, 13, 75 );

	for ( int i = 0; i < CTPlayerModels.Count(); i++ )
	{
		thisptr->PrecacheModel( CTPlayerModels[ i ] );
		g_pEngineServer->ForceModelBounds( CTPlayerModels[ i ], mins, maxs );
	}

	for ( int i = 0; i < TerroristPlayerModels.Count(); i++ )
	{
		thisptr->PrecacheModel( TerroristPlayerModels[ i ] );
		g_pEngineServer->ForceModelBounds( TerroristPlayerModels[ i ], mins, maxs );
	}

	thisptr->PrecacheModel( "models/weapons/t_arms_phoenix.mdl" );
	strcpy_s( thisptr->m_szArmsModel, "models/weapons/t_arms_phoenix.mdl" );

	precacheHook->GetOriginal<fn_t>()(thisptr);
}

bool __fastcall hkHandleCommand_JoinClass( CCSPlayer* thisptr )
{
	using fn_t = bool( __thiscall* )(CCSPlayer*);

	ConsoleDebugW( L"hkHandleCommand_JoinClass called! %p\n", _ReturnAddress() );

	if ( _ReturnAddress() == (void*) (Addresses::ChangeTeam + 5) )
	{
		ConsoleDebugW( L"Server: calling classmenu...\n" );
		//g_pEngineServer->ClientCommand( thisptr->edict(), "classmenu" );
		//thisptr->State_Enter_PICKINGCLASS();
		thisptr->State_Transition( STATE_PICKINGCLASS );
		return true;
	}

	//return thisptr->HandleCommand_JoinClass( thisptr->m_iClass );
	return handleCommand_JoinClassHook->GetOriginal<fn_t>()(thisptr);
}

CON_COMMAND( print_weapon_id, "Prints active weapon's ID" )
{
	CCSPlayer* pLocalPlayer = (CCSPlayer*) UTIL_GetListenServerHost();

	if ( !pLocalPlayer )
		return;

	CWeaponCSBase* pActiveWeapon = pLocalPlayer->GetActiveCSWeapon();

	if ( !pActiveWeapon )
		return;

	DevMsg( "weapon id: %i weapon type: %i\n", pActiveWeapon->GetCSWeaponID(), pActiveWeapon->GetWeaponType() );
}

void __fastcall hkSecondaryAttack( CWeaponCSBaseGun* thisptr )
{
	using fn_t = void( __thiscall* )(CWeaponCSBaseGun*);

	CCSPlayer *pPlayer = thisptr->GetPlayerOwner();

	if ( !pPlayer )
		return;

	if ( thisptr->GetCSWeaponID() != WEAPON_AUG && thisptr->GetCSWeaponID() != WEAPON_SG556 )
	{
		secondPrimaryHook->GetOriginal<fn_t>()(thisptr);
		return;
	}  	

	if ( thisptr->GetCSZoomLevel() == 0 )
	{	 
		pPlayer->SetFOV( pPlayer, 55, 0.2f );
		pPlayer->m_bIsScoped = true;
		thisptr->m_weaponMode = Secondary_Mode;
		thisptr->SetCSZoomLevel( 1 );
	}
	else
	{ 
		pPlayer->SetFOV( pPlayer, 0, 0.15f );
		pPlayer->m_bIsScoped = false;
		thisptr->m_weaponMode = Primary_Mode;
		thisptr->SetCSZoomLevel( 0 );
	}

	/*if ( pPlayer->GetFOV() == pPlayer->GetDefaultFOV() )
	{	 
		pPlayer->SetFOV( pPlayer, 55, 0.2f );
	}
	else if ( pPlayer->GetFOV() == 55 )
	{	   
		pPlayer->SetFOV( pPlayer, 0, 0.15f );
	}
	else
	{
		//FIXME: This seems wrong
		pPlayer->SetFOV( pPlayer, pPlayer->GetDefaultFOV() );
	}*/

	thisptr->m_flNextSecondaryAttack = gpGlobals->curtime + 0.3;
}

bool __fastcall hkHolster_USP( CBaseCombatWeapon* thisptr, void*, CBaseCombatWeapon *pSwitchingTo )
{
	using fn_t = bool(__thiscall*)(CBaseCombatWeapon*, CBaseCombatWeapon*);
	return ((fn_t)Addresses::Holster)( thisptr, pSwitchingTo );
}

void __fastcall hkFireBullet( CCSPlayer* thisptr, void*, Vector vecSrc, const QAngle &shootAngles, float fPenetration, void* pEconItemAttributes, int iBulletType, int iDamage, float flRangeModifier, CBaseEntity *pevAttacker, bool bDoEffects, float x, float y )
{					  
	thisptr->FireBullet( vecSrc, shootAngles, iBulletType, iDamage, fPenetration, flRangeModifier, pevAttacker, bDoEffects, x, y );
}

void __fastcall hkPostThink( CCSPlayer* thisptr )
{					
	/*if ( thisptr == UTIL_GetListenServerHost() )
	{
		Vector vEnd;
		AngleVectors( thisptr->PlayerData()->v_angle += (thisptr->m_Local.m_aimPunchAngle * 2.0f), &vEnd );

		Vector vStart = thisptr->EyePosition();
		vEnd = vStart + (vEnd * 8192.0f);

		trace_t tr;
		UTIL_TraceLine( vStart, vEnd, MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, thisptr, COLLISION_GROUP_NONE, &tr );

		char textBuffer[200];

		NDebugOverlay::ScreenText( 10.0f, 10.0f, "Trace test", 255, 255, 255, 0, -1.0f );

		V_snprintf( textBuffer, sizeof( textBuffer ), "startpos: %f %f %f", tr.startpos.x, tr.startpos.y, tr.startpos.z );
		NDebugOverlay::ScreenText( 10.0f, 15.0f, textBuffer, 255, 255, 255, 0, -1.0f );

		V_snprintf( textBuffer, sizeof( textBuffer ), "endpos: %f %f %f", tr.endpos.x, tr.endpos.y, tr.endpos.z );
		NDebugOverlay::ScreenText( 10.0f, 20.0f, textBuffer, 255, 255, 255, 0, -1.0f );

		V_snprintf( textBuffer, sizeof( textBuffer ), "surface name: %s", tr.surface.name );
		NDebugOverlay::ScreenText( 10.0f, 25.0f, textBuffer, 255, 255, 255, 0, -1.0f );

		V_snprintf( textBuffer, sizeof( textBuffer ), "entity: %p %s", tr.m_pEnt, tr.m_pEnt ? tr.m_pEnt->GetEntityNameAsCStr() : "null" );
		NDebugOverlay::ScreenText( 10.0f, 30.0f, textBuffer, 255, 255, 255, 0, -1.0f );

		NDebugOverlay::Line( tr.startpos, tr.endpos, 255, 255, 255, true, -1.0f );
	}*/  	

	postThinkHook->GetOriginal<decltype(&hkPostThink)>()( thisptr );
}

bool __fastcall hkSetFOV( CBasePlayer* thisptr, void* edx, CBaseEntity *pRequester, int FOV, /*float zoomRate,*/ int iZoomStart )
{
	using fn_t = bool( __thiscall* )(CBasePlayer*, CBaseEntity*, int, int);

	float zoomRate;
	__asm movss zoomRate, xmm3; // zoomRate is stored in xmm0 thanks to compiler optimizations

	return setFovHook->GetOriginal<fn_t>()(thisptr, pRequester, FOV, iZoomStart);
}

void HookCSPlayer()
{
	giveAmmoHook->SetupHook( (BYTE*) Addresses::GiveAmmo, (BYTE*) &hkGiveAmmo );
	giveAmmoHook->Hook();

	clientCommandHook->SetupHook( (BYTE*) Addresses::ClientCommand, (BYTE*) &hkClientCommand );
	clientCommandHook->Hook();

	setModelClassHook->SetupHook( (BYTE*) Addresses::SetModelFromClass, (BYTE*) &hkSetModelFromClass );
	setModelClassHook->Hook();

	precacheHook->SetupHook( (BYTE*) Addresses::Precache, (BYTE*) &hkPrecache );
	precacheHook->Hook();

	handleCommand_JoinClassHook->SetupHook( (BYTE*) Addresses::HandleCommand_JoinClass, (BYTE*) &hkHandleCommand_JoinClass );
	handleCommand_JoinClassHook->Hook();

	DWORD secondPrimaryAddress = g_dwServerBase + 0x4AB880;
	ConsoleDebugW( L"Second primary: %X", secondPrimaryAddress );
	secondPrimaryHook->SetupHook( (BYTE*) secondPrimaryAddress, (BYTE*) &hkSecondaryAttack );
	secondPrimaryHook->Hook();

	//holster_UspHook->SetupHook( (BYTE*) Addresses::Holster_hpk2000, (BYTE*) &hkHolster_USP );
	//holster_UspHook->Hook();

	fireBulletHook->SetupHook( (BYTE*) Addresses::FireBullet, (BYTE*) &hkFireBullet );
	fireBulletHook->Hook();

	postThinkHook->SetupHook( (BYTE*) Addresses::PostThink, (BYTE*) &hkPostThink );
	postThinkHook->Hook();

	setFovHook->SetupHook( (BYTE*) Addresses::SetFOV, (BYTE*) &hkSetFOV );
	setFovHook->Hook();
}