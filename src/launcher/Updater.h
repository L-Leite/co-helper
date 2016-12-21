#pragma once

bool ReadFileAndSize( const wchar_t* fileName, char** buffer, DWORD* fileSize, HANDLE* fileHandle = nullptr );
bool UpdateGameDlls();
bool HandleRequiredFiles();