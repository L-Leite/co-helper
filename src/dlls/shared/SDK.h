#pragma once

#ifdef GetClassName
#undef GetClassName
#endif

#include "cbase.h"
#include "cdll_int.h"
#include "interfaces\interfaces.h"
#include "eiface.h"

#include "vgui\IPanel.h"
#include "vgui\ISurface.h"

#ifdef CLIENT_DLL
#include "cdll_int.h"		
//#include "hud.h"
//#include "hudelement.h"

#include "c_cs_player.h"
#else
#include "cs_shareddefs.h"
#include "cs_player.h"
#include "cs_gamerules.h"

// Let's keep these out of client until we reverse it
#include "cs_ammodef.h"
#include "cs_weapon_parse.h"
#include "cs_gamemovement.h"
#include "weapon_csbasegun.h"
#endif