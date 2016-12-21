#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <fstream>	
#include <vector>
#include <memory>

#include "FindPattern.h"
#include "Utilities.h"
#include "PolyHook.h"

#ifdef CLIENT_DLL
#include "Client.h"
#else
#include "Server.h"
#endif
#include "Addresses.h"

#include "SDK.h"