#pragma once

class IFileSystem;

void HookLauncher();
void OnFileSystemLoad( HMODULE hFileSystem );