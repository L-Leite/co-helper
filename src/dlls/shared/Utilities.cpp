#include "stdafx.h"

// From 4D1
void Main_UnprotectModule( HMODULE hModule )
{
	PIMAGE_DOS_HEADER header = (PIMAGE_DOS_HEADER) hModule;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS) ((DWORD) hModule + header->e_lfanew);

	// unprotect the entire PE image
	SIZE_T size = ntHeader->OptionalHeader.SizeOfImage;
	DWORD oldProtect;
	VirtualProtect( (LPVOID) hModule, ntHeader->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READWRITE, &oldProtect );
}