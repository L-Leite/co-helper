#pragma once

size_t SearchPattern( void* scanStart, size_t scanSize, const char* pattern, uint8_t wildcard = 0xCC, uintptr_t value_offset = 0 );
DWORD SearchPattern( const wchar_t* moduleName, const char* pattern );