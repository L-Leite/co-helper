#include "stdafx.h"   
#include <algorithm>
#include <Psapi.h>	 

#define REBASE(pRVA, baseOld, baseNew)       ((uintptr_t)pRVA - (uintptr_t)baseOld + (uintptr_t)baseNew)

// From DarthTon's BlackBone
// https://github.com/DarthTon/Blackbone/blob/master/src/BlackBone/Patterns/PatternSearch.cpp
		   
size_t SearchPattern( void* scanStart, size_t scanSize, const char* pattern, uint8_t wildcard /*= 0xCC*/, uintptr_t value_offset /*= 0*/ )
{
	const uint8_t* cstart = (const uint8_t*) scanStart;
	const uint8_t* cend = cstart + scanSize;

	std::vector< uint8_t > _pattern( pattern, pattern + strlen( pattern ) );

	for ( ;;)
	{
		const uint8_t* res = std::search( cstart, cend, _pattern.begin(), _pattern.end(),
			[ &wildcard ]( uint8_t val1, uint8_t val2 ) { return (val1 == val2 || val2 == wildcard); } );

		if ( res >= cend )
			break;

		if ( value_offset != 0 )
			return REBASE( res, scanStart, value_offset );
		else
			return reinterpret_cast<uintptr_t>(res);

		cstart = res + _pattern.size();
	}

	return 0;
}

struct ModuleData
{ 
	wchar_t* name;
	MODULEINFO info;
};

DWORD SearchPattern( const wchar_t* moduleName, const char* pattern )
{
	static std::vector< ModuleData > moduleData;

	MODULEINFO moduleInfo;

	bool bFound = false;

	for ( auto& data : moduleData )
	{
		if ( !wcscmp( moduleName, data.name ) )
		{
			moduleInfo = data.info;
			bFound = true;
		}
	}

	if ( !bFound )
	{	
		auto res = K32GetModuleInformation( GetCurrentProcess(), GetModuleHandleW( moduleName ), &moduleInfo, sizeof( MODULEINFO ) );

		if ( !res )
			return NULL;

		ModuleData newData;
		newData.name = (wchar_t*) moduleName;
		newData.info = moduleInfo;

		wprintf( L"SearchPattern: pushing back %s...\n", moduleName );

		moduleData.push_back( newData );
	}  	

	return (DWORD) SearchPattern( moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, pattern );
}