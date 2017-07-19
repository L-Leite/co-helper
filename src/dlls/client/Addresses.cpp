#include "stdafx.h"

namespace Addresses
{
DWORD OpenMenu = 0;
DWORD whichMenu = 0;
}

void GetAddresses()
{
	ConsoleDebugW( L"\n### CLIENT ADDRESSES START\n" );

	Addresses::OpenMenu = SearchPattern( L"client.dll", "\x55\x8B\xEC\x68\xCC\xCC\xCC\xCC\xB9\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x85\xC0\x74\x0C\xFF\x75\x08\x8B\xC8\xE8\xCC\xCC\xCC\xCC\x5D\xC3\xB0\x01\x5D\xC3" );
	ConsoleDebugW( L"OpenMenu: %X\n", Addresses::OpenMenu );

	Addresses::whichMenu = *(DWORD*) ( SearchPattern( L"client.dll", "\x83\x3D\x00\xCC\xCC\xCC\xCC\x75\x2E\x83\xF8\x01" ) + 2 );
	ConsoleDebugW( L"whichMenu: %X\n", Addresses::whichMenu );

	ConsoleDebugW( L"### CLIENT ADDRESSES END\n\n" );
}