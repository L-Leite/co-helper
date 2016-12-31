#pragma once

class IFileSystem;
class IBaseFileSystem;

void HookLauncher();
void OnFileSystemLoad( HMODULE hFileSystem );