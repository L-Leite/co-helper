#include "stdafx.h"
#include "Updater.h"
#include "tier1\checksum_crc.h"	   
#include "tinyxml2.h"

struct FileCopyInfo_t
{
	const char* szName;
	const char* szDestination;
};

class CFilesRequiredFile
{
public:
	CFilesRequiredFile() {}

	bool ParseFilesRequired();

	std::list< const char* >& GetCopyDirectories() { return m_CopyDirectories; }
	std::list< FileCopyInfo_t >& GetCopyFiles() { return m_CopyFiles; }
	std::list< const char* >& GetDeleteDirectories() { return m_DeleteDirectories; }
	std::list< const char* >& GetCreateEmptyFiles() { return m_CreateEmptyFiles; }

private:
	std::list< const char* > m_CopyDirectories;
	std::list< FileCopyInfo_t > m_CopyFiles;
	std::list< const char* > m_DeleteDirectories;
	std::list< const char* > m_CreateEmptyFiles;
	tinyxml2::XMLDocument m_XmlDocument;
};

bool CFilesRequiredFile::ParseFilesRequired()
{
	tinyxml2::XMLError error = m_XmlDocument.LoadFile( "filesrequired.xml" );

	if ( error != tinyxml2::XML_SUCCESS )
	{
		printf( "Failed to load \"requiredfiles.xml\" with %i!\n", error );
		return false;
	}

	auto rootElement = m_XmlDocument.RootElement();
	auto directoryElement = rootElement->FirstChildElement( "Directory" );

	while ( directoryElement )
	{
		m_CopyDirectories.push_back( directoryElement->Attribute( "Name" ) );
		directoryElement = directoryElement->NextSiblingElement( "Directory" );
	}

	auto fileElement = rootElement->FirstChildElement( "File" );

	while ( fileElement )
	{
		FileCopyInfo_t fileCopyInfo;

		fileCopyInfo.szName = fileElement->Attribute( "Name" );
		fileCopyInfo.szDestination = fileElement->Attribute( "Destination" );

		m_CopyFiles.push_back( fileCopyInfo );
		fileElement = fileElement->NextSiblingElement( "File" );
	}

	auto deleteDirElement = rootElement->FirstChildElement( "DeleteDirectory" );

	while ( deleteDirElement )
	{
		m_DeleteDirectories.push_back( deleteDirElement->Attribute( "Name" ) );
		deleteDirElement = deleteDirElement->NextSiblingElement( "DeleteDirectory" );
	}

	auto emptyFileElement = rootElement->FirstChildElement( "CreateEmptyFile" );

	while ( emptyFileElement )
	{
		m_CreateEmptyFiles.push_back( emptyFileElement->Attribute( "Name" ) );
		emptyFileElement = emptyFileElement->NextSiblingElement( "CreateEmptyFile" );
	}

	return true;
}

static const char* s_szDefaultCacheXmlContent = "<Cache>\n\t<FilesHandled State=\"false\"/>\n</Cache>";

bool HandleRequiredFiles()
{
	static wchar_t extraBuffer[ MAX_PATH ];

	tinyxml2::XMLDocument xmlDoc;	 
	tinyxml2::XMLError xmlError = xmlDoc.LoadFile( "cache.xml" );

	if ( xmlError != tinyxml2::XML_SUCCESS && xmlError != tinyxml2::XML_ERROR_FILE_NOT_FOUND )
	{
		printf( "Failed to load \"cache.xml\" with %i!\n", xmlError );
		return false;
	}

	if ( xmlError == tinyxml2::XML_ERROR_FILE_NOT_FOUND )
	{
		xmlDoc.Parse( s_szDefaultCacheXmlContent );
		xmlError = xmlDoc.SaveFile( "cache.xml" );

		if ( xmlError != tinyxml2::XML_SUCCESS )
		{
			printf( "Failed to create \"cache.xml\" with %i!\n", xmlError );
			return false;
		}
	}

	auto rootElement = xmlDoc.RootElement();
	auto filesHandledElement = rootElement->FirstChildElement( "FilesHandled" );

	if ( filesHandledElement->BoolAttribute( "State" ) )
	{
		printf( "Files have been handled already, continue...\n" );
		return true;
	}

	CFilesRequiredFile filesReqFile;

	if ( !filesReqFile.ParseFilesRequired() )
		return false;			   	

	SHFILEOPSTRUCTW fileOp = { 0 };
	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SIMPLEPROGRESS;

	for ( auto& dir : filesReqFile.GetCopyDirectories() )
	{	
		swprintf( g_wzBuffer, L"%s\\csgo\\%S\0\0", g_pGameDirectory, dir );
		swprintf( extraBuffer, L"%s\\csco\\%S\0\0", g_pMainDirectory, dir );	

		fileOp.wFunc = FO_COPY;
		fileOp.pFrom = g_wzBuffer;
		fileOp.pTo = extraBuffer;
		
		int res = SHFileOperationW( &fileOp );

		if ( res || fileOp.fAnyOperationsAborted )
		{
			printf( "SHFileOperation failed with: return %i aborted %i dir: %s\n", res, fileOp.fAnyOperationsAborted, dir );
			return false;
		}
	}

	for ( auto& file : filesReqFile.GetCopyFiles() )
	{
		swprintf( g_wzBuffer, L"%s\\csgo\\%S\0\0", g_pGameDirectory, file.szName );
		swprintf( extraBuffer, L"%s\\csco\\%S\0\0", g_pMainDirectory, file.szDestination );  

		fileOp.wFunc = FO_COPY;
		fileOp.pFrom = g_wzBuffer;
		fileOp.pTo = extraBuffer;

		int res = SHFileOperationW( &fileOp );

		if ( res || fileOp.fAnyOperationsAborted )
		{
			printf( "SHFileOperation failed with: return %i aborted %i file: %s dest: %s\n", res, fileOp.fAnyOperationsAborted, file.szName, file.szDestination );
			return false;
		}
	}

	for ( auto& deleteDir : filesReqFile.GetDeleteDirectories() )
	{
		swprintf( g_wzBuffer, L"%s\\csco\\%S\0\0", g_pMainDirectory, deleteDir );

		fileOp.wFunc = FO_DELETE;
		fileOp.pFrom = g_wzBuffer;
		fileOp.pTo = nullptr;

		int res = SHFileOperationW( &fileOp );

		if ( res || fileOp.fAnyOperationsAborted )
		{
			printf( "SHFileOperation failed with: return %i aborted %i\n", res, fileOp.fAnyOperationsAborted );
			return false;
		}

		/*if ( !RemoveDirectoryW( g_wzBuffer ) )
		{
			printf( "RemoveDirectory failed!\n" );
			return false;
		}*/
	}

	for ( auto& createFile : filesReqFile.GetCreateEmptyFiles() )
	{
		swprintf( g_wzBuffer, L"%s\\csco\\%S", g_pMainDirectory, createFile );

		HANDLE file = CreateFileW( g_wzBuffer, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );

		if ( file == INVALID_HANDLE_VALUE )
		{
			printf( "Failed to create empty file %s!\n", createFile );
			return false;
		}

		CloseHandle( file );
	}  	

	filesHandledElement->SetAttribute( "State", true );
	xmlDoc.SaveFile( "cache.xml" );

	printf( "Handled required files successfully...\n" );

	return true;
}

bool ReadFileAndSize( const wchar_t* fileName, char** buffer, DWORD* fileSize, HANDLE* fileHandle /*= nullptr*/ )
{
	DWORD attributes = GetFileAttributesW( fileName );

	if ( attributes == INVALID_FILE_ATTRIBUTES )
	{
		wprintf( L"%s path is invalid! Attributes: 0x%X\n", fileName, attributes );
		return false;
	}

	HANDLE hFile = CreateFileW( fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		wprintf( L"Couldn't open %s!\n", fileName );
		return false;
	}

	DWORD dwFileSize = GetFileSize( hFile, nullptr );
	char* pFileBuffer = new char[ dwFileSize ];

	if ( !ReadFile( hFile, pFileBuffer, dwFileSize, nullptr, nullptr ) )
	{
		wprintf( L"Couldn't read %s!\n", fileName );
		MessageBoxW( nullptr, L"Couldn't read file!", L"Launcher Error", MB_OK | MB_ICONERROR );
		return false;
	}

	if ( buffer )
		*buffer = pFileBuffer;
	if ( fileSize )
		*fileSize = dwFileSize;
	if ( fileHandle )
		*fileHandle = hFile;

	if ( !fileHandle )
		CloseHandle( hFile );

	return true;
}

// We shouldn't need to update the matchmaking libraries since we don't use them
bool UpdateGameDlls()
{
	bool bFirstRun = false;

	// Check client.dll
	swprintf_s( g_wzBuffer, L"%s\\csgo\\bin\\client.dll", g_pGameDirectory );

	DWORD csClientFileSize = 0;
	char* csClientFileBuffer = nullptr;

	if ( !ReadFileAndSize( g_wzBuffer, &csClientFileBuffer, &csClientFileSize ) )
	{
		printf( "Failed to read original client.dll!\n" );
		return false;
	}

	CRC32_t csClientFileCRC = CRC32_ProcessSingleBuffer( csClientFileBuffer, csClientFileSize );
	printf( "Original client.dll CRC: %X\n", csClientFileCRC );

	swprintf_s( g_wzBuffer, L"%s\\csco\\bin\\client.dll", g_pMainDirectory );

	DWORD clientFileSize = 0;
	char* clientFileBuffer = nullptr;

	if ( !ReadFileAndSize( g_wzBuffer, &clientFileBuffer, &clientFileSize ) )
	{
		printf( "Failed to read our client.dll! Let's just copy since this is probably a first run...\n" );

		bFirstRun = true;

		wchar_t originalDll[ MAX_PATH ];
		swprintf_s( originalDll, L"%s\\csgo\\bin\\client.dll", g_pGameDirectory );
		swprintf_s( g_wzBuffer, L"%s\\csco\\bin", g_pMainDirectory );

		CreateDirectoryW( g_wzBuffer, nullptr );

		wcscat_s( g_wzBuffer, L"\\client.dll" );

		BOOL res = CopyFileW( originalDll, g_wzBuffer, false );

		if ( !res )
		{
			printf( "Failed to copy client.dll!\n" );
			return false;
		}

		printf( "Copied client.dll successfully...\n" );

		swprintf_s( originalDll, L"%s\\csgo\\bin\\matchmaking.dll", g_pGameDirectory );
		swprintf_s( g_wzBuffer, L"%s\\csco\\bin\\matchmaking.dll", g_pMainDirectory );

		res = CopyFileW( originalDll, g_wzBuffer, false );

		if ( !res )
		{
			printf( "Failed to copy matchmaking.dll!\n" );
			return false;
		}

		printf( "Copied matchmaking.dll successfully...\n" );

#ifdef DEDICATED_LAUNCHER
		swprintf_s( originalDll, L"%s\\csgo\\bin\\matchmaking_ds.dll", g_pGameDirectory );
		swprintf_s( g_wzBuffer, L"%s\\csco\\bin\\matchmaking_ds.dll", g_pMainDirectory );

		res = CopyFileW( originalDll, g_wzBuffer, false );

		if ( !res )
		{
			printf( "Failed to copy matchmaking_ds.dll!\n" );
			return false;
		}

		printf( "Copied matchmaking_ds.dll successfully...\n" );
#endif

		swprintf_s( originalDll, L"%s\\csgo\\bin\\server.dll", g_pGameDirectory );
		swprintf_s( g_wzBuffer, L"%s\\csco\\bin\\server.dll", g_pMainDirectory );

		res = CopyFileW( originalDll, g_wzBuffer, false );

		if ( !res )
		{
			printf( "Failed to copy server.dll!\n" );
			return false;
		}

		printf( "Copied server.dll successfully...\n" );

		return true;
	}

	CRC32_t clientFileCRC = CRC32_ProcessSingleBuffer( clientFileBuffer, clientFileSize );
	printf( "Our client.dll CRC: %X\n", clientFileCRC );

	// Now check server.dll		
	swprintf_s( g_wzBuffer, L"%s\\csgo\\bin\\server.dll", g_pGameDirectory );

	DWORD csServerFileSize = 0;
	char* csServerFileBuffer = nullptr;

	if ( !ReadFileAndSize( g_wzBuffer, &csServerFileBuffer, &csServerFileSize ) )
	{
		printf( "Failed to read original server.dll!\n" );
		return false;
	}

	CRC32_t csServerFileCRC = CRC32_ProcessSingleBuffer( csServerFileBuffer, csServerFileSize );
	printf( "Original server.dll CRC: %X\n", csServerFileCRC );

	swprintf_s( g_wzBuffer, L"%s\\csco\\bin\\server.dll", g_pMainDirectory );

	DWORD serverFileSize = 0;
	char* serverFileBuffer = nullptr;

	if ( !ReadFileAndSize( g_wzBuffer, &serverFileBuffer, &serverFileSize ) )
	{
		printf( "Failed to read our server.dll!\n" );
		return false;
	}

	CRC32_t serverFileCRC = CRC32_ProcessSingleBuffer( serverFileBuffer, serverFileSize );
	printf( "Our server.dll CRC: %X\n", serverFileCRC );

	bool bUpdated = false;
	wchar_t* szUpdateString = nullptr;

	if ( csClientFileCRC != clientFileCRC )
	{
		szUpdateString = new wchar_t[ 26 ];
		wcscpy_s( szUpdateString, 26, L"client.dll" );
		bUpdated = true;
	}

	if ( csServerFileCRC != serverFileCRC )
	{
		if ( bUpdated )
			wcscat_s( szUpdateString, 26, L" and server.dll" );
		else
		{
			szUpdateString = L"server.dll";
			bUpdated = true;
		}
	}

	if ( bUpdated )
	{
		swprintf_s( g_wzBuffer, L"%s of original CSGO have been updated.\nWanna update Classic Offensive with them?", szUpdateString );
		int res = MessageBoxW( nullptr, g_wzBuffer, L"Launcher", MB_YESNO | MB_ICONQUESTION );

		delete szUpdateString;

		if ( res == IDYES )
		{
			printf( "Writing to our dlls...\n" );

			wchar_t originalDll[ MAX_PATH ];
			swprintf_s( originalDll, L"%s\\csgo\\bin\\client.dll", g_pGameDirectory );
			swprintf_s( g_wzBuffer, L"%s\\csco\\bin\\client.dll", g_pMainDirectory );

			BOOL res = CopyFileW( originalDll, g_wzBuffer, false );

			if ( !res )
			{
				printf( "Failed to copy client.dll! LastError: %X\n", GetLastError() );
				return false;
			}

			swprintf_s( originalDll, L"%s\\csgo\\bin\\server.dll", g_pGameDirectory );
			swprintf_s( g_wzBuffer, L"%s\\csco\\bin\\server.dll", g_pMainDirectory );

			res = CopyFileW( originalDll, g_wzBuffer, false );

			if ( !res )
			{
				printf( "Failed to copy server.dll! LastError: %X\n", GetLastError() );
				return false;
			}
		}

		else if ( res == IDNO )
			printf( "Fine don't update, idc\n" );
	}

	return true;
}