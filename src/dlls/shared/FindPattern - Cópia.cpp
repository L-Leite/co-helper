#include "stdafx.h"
#include "FindPattern.h"   
#include <Psapi.h>

// From DarthTon
// https://github.com/learn-more/findpattern-bench/blob/master/patterns/DarthTon.h
		   
struct PartData
{
    int32_t mask = 0;
    __m128i needle; //C2797: list initialization inside member initializer list or non-static data member initializer is not implemented


    PartData()
    {
        memset(&needle, 0, sizeof(needle));
    }
};

const void* SearchPattern( const uint8_t* data, const uint32_t size, const uint8_t* pattern, const char* mask )
{
    const uint8_t* result = nullptr;
    auto len = strlen( mask );
    auto first = strchr( mask, '?' );
    size_t len2 = (first != nullptr) ? (first - mask) : len;
    auto firstlen = min( len2, 16 );
    intptr_t num_parts = (len < 16 || len % 16) ? (len / 16 + 1) : (len / 16);
    PartData parts[4];

    for (intptr_t i = 0; i < num_parts; ++i, len -= 16)
    {
        for (size_t j = 0; j < min( len, 16 ) - 1; ++j)
            if (mask[16 * i + j] == 'x')
                _bittestandset( (LONG*)&parts[i].mask, j );

        parts[i].needle = _mm_loadu_si128( (const __m128i*)(pattern + i * 16) );
    }

    bool abort = false;

#pragma omp parallel for
    for (intptr_t i = 0; i < static_cast<intptr_t>(size) / 32 - 1; ++i)
    {
        #pragma omp flush (abort)
        if(!abort)
        {
            auto block = _mm256_loadu_si256( (const __m256i*)data + i );
            if (_mm256_testz_si256( block, block ))
                continue;

            auto offset = _mm_cmpestri( parts->needle, firstlen, _mm_loadu_si128( (const __m128i*)(data + i * 32) ), 16, _SIDD_CMP_EQUAL_ORDERED );
            if (offset == 16)
            {
                offset += _mm_cmpestri( parts->needle, firstlen, _mm_loadu_si128( (const __m128i*)(data + i * 32 + 16) ), 16, _SIDD_CMP_EQUAL_ORDERED );
                if (offset == 32)
                    continue;
            }

            for (intptr_t j = 0; j < num_parts; ++j)
            {
                auto hay = _mm_loadu_si128( (const __m128i*)(data + (2 * i + j) * 16 + offset) );
                auto bitmask = _mm_movemask_epi8( _mm_cmpeq_epi8( hay, parts[j].needle ) );
                if ((bitmask & parts[j].mask) != parts[j].mask)
                    goto next;
            }

            result = data + 32 * i + offset;
            abort = true;
            #pragma omp flush (abort)
        }
        //break;  //C3010: 'break' : jump out of OpenMP structured block not allowed

    next:;
    }

    return result;
}

struct ModuleData
{ 
	wchar_t* name;
	MODULEINFO info;
};

DWORD SearchPattern( const wchar_t* moduleName, const char* pattern, const char* mask )
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

		ConsoleDebugW( L"SearchPattern: pushing back %s...\n", moduleName );

		moduleData.push_back( newData );
	}  	

	return (DWORD) SearchPattern( (uint8_t*) moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, (uint8_t*) pattern, mask );
}

size_t Search( void* scanStart, size_t scanSize, const char* pattern, uint8_t wildcard = 0x00, uintptr_t value_offset = 0 )
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