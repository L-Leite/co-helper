#include "stdafx.h"
#ifdef VGUI_TEST
#include "PolyHook.h"
#include "CounterStrikeViewport.h"

#include "VGUI\cstrikeclassmenu.h"

#pragma comment( lib, "vgui_controls.lib" )

PLH::Detour* CreatePanelByNameHook = new PLH::Detour;
PLH::Detour* CreateDefaultPanelsHook = new PLH::Detour;

IViewPortPanel* __fastcall hkCreatePanelByName( IViewPort* thisptr, void*, const char* szPanelName )
{
	using fn_t = IViewPortPanel*(__thiscall*)( IViewPort*, const char* );

	ConsoleDebugW( L"CreatePanelByName called! PanelName: %S\n", szPanelName );

	IViewPortPanel* newpanel = NULL;

	if ( Q_strcmp( PANEL_CLASS_CT, szPanelName ) == 0 )
	{
		newpanel = new CClassMenu( thisptr, PANEL_CLASS_CT );
	}

	else if ( Q_strcmp( PANEL_CLASS_TER, szPanelName ) == 0 )
	{
		newpanel = new CClassMenu( thisptr, PANEL_CLASS_TER );
	}

	else
	{
		newpanel = CreatePanelByNameHook->GetOriginal< fn_t >()(thisptr, szPanelName);
	}

	return newpanel;
}

void __fastcall hkCreateDefaultPanels( IViewPort* thisptr )
{
	using fn_t = void(__thiscall*)( IViewPort* );

	ConsoleDebugW( L"CreateDefaultPanels called!\n" );

	//CClassMenu* newClassMenu = new CClassMenu( thisptr, "class_ct" );
	//CClassMenu* newClassMenu2 = new CClassMenu( thisptr, "class_ter" );

	DWORD v2 = *(DWORD *) thisptr;
	DWORD v3 = (*(int( __thiscall ** )(void*, char*, char*))(*(DWORD *) thisptr + 944))(thisptr, "class_ct", "PANEL_CLASS_CT");
	(*(void( __thiscall ** )(void *, DWORD))(v2 + 952))(thisptr, v3);
	DWORD v4 = (*(int( __thiscall ** )(void*, char*, char*))(*(DWORD *) thisptr + 944))(thisptr, "class_ter", "PANEL_CLASS_TER");
	(*(void( __thiscall ** )(void *, DWORD))(v2 + 952))(thisptr, v4);

	CreateDefaultPanelsHook->GetOriginal< fn_t >()(thisptr);
}

PLH::Detour* SetProportionalHook = new PLH::Detour;
PLH::Detour* onRequestFocusHook = new PLH::Detour;

void __fastcall hkSetProportional( vgui::Panel* thisptr, void*, bool state )
{
	using fn_t = void(__thiscall*)( vgui::Panel*, bool );

	ConsoleDebugW( L"SetProportional: setting for %S parent: %S %p\n", thisptr->GetName(), thisptr->GetParent() ? thisptr->GetParent()->GetName() : "(none)", thisptr );

	thisptr->SetProportional( state );
	//SetProportionalHook->GetOriginal< fn_t >()( thisptr, state );
}

void __fastcall hkOnRequestFocus( vgui::Panel* thisptr, void*, vgui::VPANEL subFocus, vgui::VPANEL defaultPanel )
{
	using fn_t = void( __thiscall* )( vgui::Panel*, vgui::VPANEL, vgui::VPANEL );

	ConsoleDebugW( L"OnRequestFocus: setting for %S parent: %S %p\n", thisptr->GetName(), thisptr->GetParent() ? thisptr->GetParent()->GetName() : "(none)", thisptr );

	thisptr->OnRequestFocus( subFocus, defaultPanel );
}

void HookCounterStrikeViewport()
{

	VGui_InitInterfacesList( "CLIENT", nullptr, 0 );

	//counterStrikeViewportHook->SetupHook( (BYTE*) GetViewPortInterface(), 236, (BYTE*) &hkCreatePanelByName );
	DWORD createPanelAddress = g_dwClientBase + 0x5CCD00;
	ConsoleDebugW( L"CreatePanelByPanel: %X\n", createPanelAddress );

	CreatePanelByNameHook->SetupHook( (BYTE*) createPanelAddress, (BYTE*) &hkCreatePanelByName );
	CreatePanelByNameHook->Hook();

	DWORD createDefaultPanelsAddress = g_dwClientBase + 0x537480;
	ConsoleDebugW( L"CreateDefaultPanels: %X\n", createDefaultPanelsAddress );

	CreateDefaultPanelsHook->SetupHook( (BYTE*) createDefaultPanelsAddress, (BYTE*) &hkCreateDefaultPanels );
	CreateDefaultPanelsHook->Hook();

	DWORD setProportionalAddress = g_dwClientBase + 0x734580;
	ConsoleDebugW( L"SetProportional: %X\n", setProportionalAddress );

	SetProportionalHook->SetupHook( (BYTE*) setProportionalAddress, (BYTE*) &hkSetProportional );
	SetProportionalHook->Hook();

	DWORD OnRequestFocusAddress = g_dwClientBase + 0x73E2F0;
	ConsoleDebugW( L"OnRequestFocus: %X\n", OnRequestFocusAddress );

	onRequestFocusHook->SetupHook( (BYTE*) OnRequestFocusAddress, (BYTE*) &hkOnRequestFocus );
	onRequestFocusHook->Hook();
}
#endif