#include "stdafx.h"
#include "Hooks.h"
#include "PolyHook.h"

PLH::VTableSwap* panelHook = new PLH::VTableSwap;
vgui::HFont hTahoma = vgui::INVALID_FONT;

void Initialize()
{
	static bool m_bInitialized = false;

	if ( m_bInitialized )
		return;

	hTahoma = g_pVGuiSurface->CreateFont();

	g_pVGuiSurface->SetFontGlyphSet( hTahoma, "Tahoma", 24, 0, 0, 0, FONTFLAG_OUTLINE );

	m_bInitialized = true;
}

void DrawString( int x, int y, Color color, vgui::HFont font, const wchar_t* string, ... )
{				
	va_list va;

	va_start( va, string );
	vswprintf_s( g_wzBuffer, string, va );
	va_end( va );

	g_pVGuiSurface->DrawSetTextFont( font );
	g_pVGuiSurface->DrawSetTextColor( color );
	g_pVGuiSurface->DrawSetTextPos( x, y );
	g_pVGuiSurface->DrawPrintText( g_wzBuffer, wcslen( g_wzBuffer ) );
}

void __fastcall hkPaintTraverse( vgui::IPanel* thisptr, void*, vgui::VPANEL vguiPanel, bool forceRepaint, bool allowForce )
{
	using fn_t = void(__thiscall*)( vgui::IPanel*, vgui::VPANEL, bool, bool );
	static vgui::VPANEL vguiFocusOverlayPanel = vgui::INVALID_PANEL;

	panelHook->GetOriginal< fn_t >()( thisptr, vguiPanel, forceRepaint, allowForce );

	if ( vguiFocusOverlayPanel == vgui::INVALID_PANEL )
	{
		const char* name = thisptr->GetName( vguiPanel );

		if ( !strcmp( name, "FocusOverlayPanel" ) )
		{
			vguiFocusOverlayPanel = vguiPanel;
			Initialize();
		}		   			
	}

	if ( vguiPanel != vguiFocusOverlayPanel )
		return;

	int width, height;
	g_pEngineClient->GetScreenSize( width, height );

	int wide, tall; 
	g_pVGuiSurface->GetTextSize( hTahoma, L"Classic Offensive", wide, tall );		  	

	DrawString( width - wide - 5, 5, Color( 255, 255, 255, 255 ), hTahoma, L"Classic Offensive" );	  			   
}			 

void HookPanel()
{
	// I have no fucking clue why I have to do this, but if I don't surface calls crash
	CreateInterfaceFn matInterface = (CreateInterfaceFn) GetProcAddress( GetModuleHandleW( L"vguimatsurface.dll" ), "CreateInterface" );
	g_pVGuiSurface = (vgui::ISurface*) matInterface( VGUI_SURFACE_INTERFACE_VERSION, nullptr );

	panelHook->SetupHook( (BYTE*) g_pVGuiPanel, 41, (BYTE*) &hkPaintTraverse );
	panelHook->Hook();
}