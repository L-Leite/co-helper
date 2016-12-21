#include "stdafx.h"
#include "Updater.h"
#include "tier1\checksum_crc.h"	   
#include "tinyxml2.h"

class CFilesRequiredFile
{
public:
	CFilesRequiredFile() {}

	bool ParseFilesRequired();

	std::list< const char* >& GetCopyDirectories() { return m_CopyDirectories; }
	std::list< const char* >& GetCopyFiles() { return m_CopyFiles; }
	std::list< const char* >& GetDeleteDirectories() { return m_DeleteDirectories; }
	std::list< const char* >& GetCreateEmptyFiles() { return m_CreateEmptyFiles; }

private:
	std::list< const char* > m_CopyDirectories;
	std::list< const char* > m_CopyFiles;
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
		m_CopyFiles.push_back( fileElement->Attribute( "Name" ) );
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

bool HandleRequiredFiles()
{
	static wchar_t extraBuffer[ MAX_PATH ];

	HRESULT hRes = CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );

	if ( FAILED( hRes ) )
	{
		printf( "CoInitializeEx failed with %X!\n", hRes );
		return false;
	}

	IFileOperation* pFileOp = nullptr;

	hRes = CoCreateInstance( CLSID_FileOperation, nullptr, CLSCTX_ALL, IID_PPV_ARGS( &pFileOp ) );

	if ( FAILED( hRes ) )
	{
		printf( "CoCreateInstance failed with %X!\n", hRes );
		return false;
	}

	CFilesRequiredFile filesReqFile;

	if ( !filesReqFile.ParseFilesRequired() )
		return false;			   	

	pFileOp->SetOperationFlags( FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOFX_DONTDISPLAYLOCATIONS );

	for ( auto& dir : filesReqFile.GetCopyDirectories() )
	{	
		swprintf( g_wzBuffer, L"%s\\csgo\\%S\0\0", g_pGameDirectory, dir );
		swprintf( extraBuffer, L"%s\\csco\\%S\0\0", g_pMainDirectory, dir );				

		DWORD attributes = GetFileAttributesW( extraBuffer );

		if ( ( attributes != INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY) ) && !PathIsDirectoryEmptyW( extraBuffer ) )
		{	   				
			printf( "%s already exists, continuing...\n", dir );
			continue;
		}

		IShellItem* pFrom = nullptr;
		IShellItem* pTo = nullptr;
		hRes = SHCreateItemFromParsingName( g_wzBuffer, nullptr, IID_PPV_ARGS( &pFrom ) );
		hRes += SHCreateItemFromParsingName( extraBuffer, nullptr, IID_PPV_ARGS( &pTo ) );

		hRes += pFileOp->CopyItem( pFrom, pTo, nullptr, nullptr );

		if ( hRes )
		{
			printf( "Something went wrong copying folders...\n" );
			return false;
		}

		pFrom->Release();
		pTo->Release();
	}

	for ( auto& file : filesReqFile.GetCopyFiles() )
	{
		swprintf( g_wzBuffer, L"%s\\csgo\\%S", g_pGameDirectory, file );
		swprintf( extraBuffer, L"%s\\csco\\%S", g_pMainDirectory, file );  

		IShellItem* pFrom = nullptr;
		IShellItem* pTo = nullptr;
		hRes = SHCreateItemFromParsingName( g_wzBuffer, nullptr, IID_PPV_ARGS( &pFrom ) );
		hRes += SHCreateItemFromParsingName( extraBuffer, nullptr, IID_PPV_ARGS( &pTo ) );

		hRes += pFileOp->CopyItem( pFrom, pTo, nullptr, nullptr );

		if ( hRes )
		{
			printf( "Something went wrong copying folders...\n" );
			return false;
		}

		pFrom->Release();
		pTo->Release();
	}

	for ( auto& deleteDir : filesReqFile.GetDeleteDirectories() )
	{
		swprintf( g_wzBuffer, L"%s\\csco\\%S", g_pMainDirectory, deleteDir );

		IShellItem* pItem = nullptr;
		hRes = SHCreateItemFromParsingName( g_wzBuffer, nullptr, IID_PPV_ARGS( &pItem ) );

		hRes += pFileOp->DeleteItem( pItem, nullptr );

		if ( hRes )
		{
			printf( "Something went wrong copying folders...\n" );
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

	hRes = pFileOp->PerformOperations();

	if ( FAILED( hRes ) )
	{	
		printf( "PerformOperations failed with %X!\n", hRes );

		pFileOp->Release();
		CoUninitialize();

		return false;
	}

	printf( "Handled required files successfully...\n" );

	if ( pFileOp )
		pFileOp->Release();

	CoUninitialize();	  	

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