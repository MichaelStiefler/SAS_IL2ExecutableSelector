//*****************************************************************
// wrapper.dll - il2fb.exe mod files wrapper
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of wrapper.dll.
//
// wrapper.dll is free software.
// It is distributed under the DWTFYWTWIPL license:
//
// DO WHAT THE FUCK YOU WANT TO WITH IT PUBLIC LICENSE
// Version 1, March 2012
//
// Copyright (C) 2013 SAS~Storebror <mike@sas1946.com>
//
// Everyone is permitted to copy and distribute verbatim or modified
// copies of this license document, and changing it is allowed as long
// as the name is changed.
//
// DO WHAT THE FUCK YOU WANT TO WITH IT PUBLIC LICENSE
// TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
//
// 0. You just DO WHAT THE FUCK YOU WANT TO WITH IT.
//
//*****************************************************************

#include "stdafx.h"
//#include <vector>
//#include <algorithm>
#include <Shellapi.h>

#include "SAS IL-2 Wrapper.h"
#include "sfs.h"
#include "trace.h"

#include "resource.h"
//#include <set>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

#define DIVIDER_SECONDS         1000000000000.0f
#define DIVIDER_MILLISECONDS    1000000000.0f
#define DIVIDER_MICROSECONDS    1000000.0f
#define DIVIDER_NANOSECONDS     1000.0f
#define MULTIPLIER_PIKOSECONDS  1000000000000.0f

#define CLIENT_INI				L"il2fb.ini"
#define SERVER_INI				L"il2server.ini"
#define LOGFILE_NAME			L"initlog.lst"

DWORD	g_dwIndex = 0,
g_dwDups = 0,
g_dwNotFound = 0;
int iProcessAttachCounter = 0,
iThreadAttachCounter = 0;

float g_SearchPikoseconds;
ULONG g_SFSOpenfCalls = 0, g_SearchIterations = 0, ulFreq = 0, g_SFSOpenf_found_hash1 = 0, g_SFSOpenf_found_hash2 = 0, g_SFSOpenf_from_SFS = 0, g_SFSOpenf_not_found = 0;
ULONGLONG First;

HINSTANCE hExecutable = NULL;
TSFS_open* SFS_open = NULL;
TSFS_openf* SFS_openf = NULL;
//TSFS_read* SFS_read = NULL;
//TSFS_lseek* SFS_lseek = NULL;
TSFS_open_cpp* SFS_open_cpp = NULL;
TSFS_openf_cpp* SFS_openf_cpp = NULL;

bool g_bUseCachedFileLists = FALSE;
FILE *g_cachedListFile = NULL;

static TCHAR	szHashString[32],
szBuf[MAX_PATH],
ExeName[MAX_PATH],
ExePath[MAX_PATH],
FilesFolder[MAX_PATH],
ModsFolder[MAX_PATH],
CurParam[MAX_PATH],
LinkBackName[MAX_PATH],
IniFileName[MAX_PATH],
CachedListFileName[MAX_PATH];
static CHAR cBuf[MAX_PATH];
TCHAR LogFileName[MAX_PATH];

std::vector<MyFileListItem*> FileList;

BOOL g_bDumpFileAccess = FALSE;
BOOL g_bDumpClassFiles = FALSE;
BOOL g_bDumpOtherFiles = FALSE;

//************************************
// Method:    ThreadAttach
// FullName:  ThreadAttach
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void ThreadAttach()
{
	iThreadAttachCounter++;
	TRACE("ThreadAttach, attached Threads =  %d\r\n", iThreadAttachCounter);
}

//************************************
// Method:    ThreadDetach
// FullName:  ThreadDetach
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void ThreadDetach()
{
	iThreadAttachCounter--;
	TRACE("ThreadDetach, attached Threads =  %d\r\n", iThreadAttachCounter);
}

//************************************
// Method:    StartWrapper
// FullName:  StartWrapper
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HINSTANCE hInstance
//************************************
void StartWrapper(HINSTANCE hInstance)
{
#ifdef _DEBUG
	HWND hSplash = FindWindow(_T("Il2SASLauncherWnd"), NULL);
	if (hSplash != NULL) PostMessage(hSplash, WM_CLOSE, 0, 0);
#endif
	iProcessAttachCounter++;


	if (iProcessAttachCounter == 1) {
		GetModuleFileName(NULL, ExeName, MAX_PATH);
		_tcscpy(LinkBackName, ExeName);
		_tcscpy(ExePath, ExeName);
		LPTSTR lpExePathDelimiter = _tcsrchr(ExePath, L'\\');
		_tcscpy(&lpExePathDelimiter[1], L"");
		_stprintf(LogFileName, L"%s%s", ExePath, LOGFILE_NAME);
	}

	TRACE("ProcessAttach, attached Processes =  %d\r\n", iProcessAttachCounter);

	if (iProcessAttachCounter > 1) {
		return;
	}

	_tcscpy(FilesFolder, L"FILES");
	_tcscpy(ModsFolder, L"MODS");
	FileList.clear();
	ULONGLONG ullFreq;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&ullFreq));
	ulFreq = (ULONG)ullFreq;
	g_dwIndex = 0;
	TRACE("Calling GetCommandLineParams()\r\n");
	GetCommandLineParams();
	TRACE("Calling LinkIl2fbExe()\r\n");
	LinkIl2fbExe();
	float fCreateMods = 0, fCreateFiles = 0, fSortList = 0, fRemoveDups = 0;

	try {
		if (_tcsicmp(ModsFolder, L"none") != 0) {
			if (DirectoryExists(ModsFolder)) {
				if (g_bUseCachedFileLists) {
					TRACE("Calling CreateCachedModsFolderList()\r\n");
					bool bCacheUsed = CreateCachedModsFolderList(&fCreateMods);
					TRACE(L"Scanning %s folder took %.0f milliseconds %s\r\n", ModsFolder, fCreateMods / DIVIDER_MILLISECONDS, bCacheUsed ? L"(cached).\r\n" : L"(cache created).\r\n");
				}
				else {
					_stprintf(CachedListFileName, L"%s%s\\~wrapper.cache", ExePath, ModsFolder);

					if (FileExists(CachedListFileName)) {
						DeleteFile(CachedListFileName);
					}

					TRACE("Calling CreateModsFolderList()\r\n");
					CreateModsFolderList(&fCreateMods);
					TRACE(L"Scanning %s folder took %.0f milliseconds.\r\n", ModsFolder, fCreateMods / DIVIDER_MILLISECONDS);
				}
			}
		}
	}
	catch (...) {
		TRACE(L"Error loading files from Folder %s\r\n", ModsFolder);
	}

	try {
		if (_tcsicmp(FilesFolder, L"none") != 0) {
			if (DirectoryExists(FilesFolder)) {
				if (g_bUseCachedFileLists) {
					TRACE("Calling CreateCachedFilesFolderList()\r\n");
					bool bCacheUsed = CreateCachedFilesFolderList(&fCreateFiles);
					TRACE(L"Scanning %s folder took %.0f milliseconds ", FilesFolder, fCreateFiles / DIVIDER_MILLISECONDS);

					if (bCacheUsed) {
						TRACE("(cached).\r\n");
					}
					else {
						TRACE("(cache created).\r\n");
					}
				}
				else {
					_stprintf(CachedListFileName, L"%s%s\\~wrapper.cache", ExePath, FilesFolder);

					if (FileExists(CachedListFileName)) {
						DeleteFile(CachedListFileName);
					}

					TRACE("Calling CreateFilesFolderList()\r\n");
					CreateFilesFolderList(&fCreateFiles);
					TRACE(L"Scanning %s folder took %.0f milliseconds.\r\n", FilesFolder, fCreateFiles / DIVIDER_MILLISECONDS);
				}
			}
		}
	}
	catch (...) {
		TRACE(L"Error loading files from Folder %s\r\n", FilesFolder);
	}

	TRACE("Total number of modded files = %d.\r\n", g_dwIndex);
	TRACE("Calling SortList()\r\n");
	SortList(&fSortList);
	TRACE("Sorting modded files list took %.3f milliseconds.\r\n", fSortList / DIVIDER_MILLISECONDS);
	TRACE("Calling RemoveDuplicates()\r\n");
	int iDups = RemoveDuplicates(&fRemoveDups);
	TRACE("Removing %d Duplicates took %.3f milliseconds.\r\n", iDups, fRemoveDups / DIVIDER_MILLISECONDS);

}

//************************************
// Method:    StopWrapper
// FullName:  StopWrapper
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void StopWrapper()
{
	iProcessAttachCounter--;
	TRACE("ProcessDetach, attached Processes =  %d\r\n", iProcessAttachCounter);

	if (iProcessAttachCounter > 0) {
		return;
	}

	FileList.clear();
	TRACE("Total files opened = %d\r\n", g_SFSOpenfCalls);
	TRACE("Files loaded from SFS archives = %d\r\n", g_SFSOpenf_from_SFS);
	TRACE("Files loaded from mod folders  = %d\r\n", g_SFSOpenf_found_hash1 + g_SFSOpenf_found_hash2);
	TRACE("  ~ using 1st level hash match = %d\r\n", g_SFSOpenf_found_hash1);
	TRACE("  ~ using 2nd level hash match = %d\r\n", g_SFSOpenf_found_hash2);
	TRACE("Files not found                = %d\r\n", g_SFSOpenf_not_found);
	float fTotalSearchSeconds = g_SearchPikoseconds / DIVIDER_SECONDS;
	TRACE("Total search time consumed = %.3f milliseconds (%.12f Seconds)\r\n", g_SearchPikoseconds / DIVIDER_MILLISECONDS, fTotalSearchSeconds);
	LONG lSFSOpenfCalls = long(g_SFSOpenfCalls);
	float fTest = g_SearchPikoseconds / float(lSFSOpenfCalls) / 1000.0f;
	TRACE("Search Time per File = %.3f nanoseconds (%.12f Seconds)\r\n", g_SearchPikoseconds / float(lSFSOpenfCalls) / 1000.0f, g_SearchPikoseconds / float(lSFSOpenfCalls) / DIVIDER_SECONDS);
	float fSearchIterationsPerFile = float(g_SearchIterations) / float(g_SFSOpenfCalls);
	TRACE("Average Search Iterations required per File = %.1f\r\n", fSearchIterationsPerFile);

	if (hExecutable != NULL) {
		FreeLibrary(hExecutable);
	}

}

//************************************
// Method:    CompareFileList
// FullName:  CompareFileList
// Access:    public 
// Returns:   int __cdecl
// Qualifier:
// Parameter: const void * mfli1
// Parameter: const void * mfli2
//************************************
int __cdecl CompareFileList(const void *mfli1, const void *mfli2)
{
	int iRet = 0;

	if ((*(MyFileListItem**)mfli1)->hash < (*(MyFileListItem**)mfli2)->hash) {
		iRet = -1;
	}
	else if ((*(MyFileListItem**)mfli1)->hash > (*(MyFileListItem**)mfli2)->hash) {
		iRet = 1;
	}
	else if ((*(MyFileListItem**)mfli1)->dwIndex < (*(MyFileListItem**)mfli2)->dwIndex) {
		iRet = -1;
	}
	else if ((*(MyFileListItem**)mfli1)->dwIndex > (*(MyFileListItem**)mfli2)->dwIndex) {
		iRet = 1;
	}

	return iRet;
}

//************************************
// Method:    SortList
// FullName:  SortList
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: float * pfPikoSeconds
//************************************
void SortList(float * pfPikoSeconds)
{
	StopWatchStart(pfPikoSeconds);
	if (!FileList.empty()) std::qsort(&*FileList.begin(), FileList.size(), sizeof(MyFileListItem*), CompareFileList);   // sort and keep original relative order of equivalent elements.
	StopWatchStop(pfPikoSeconds);
}

//************************************
// Method:    CreateModsFolderList
// FullName:  CreateModsFolderList
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: float * pfPikoSeconds
//************************************
void CreateModsFolderList(float * pfPikoSeconds)
{
	StopWatchStart(pfPikoSeconds);
	TCHAR searchPattern[MAX_PATH];
	TCHAR listFilesPath1[MAX_PATH];
	TCHAR listFilesPath2[MAX_PATH];

	std::vector<wstring> FolderNames;

	try {
		if (_tcsicmp(ModsFolder, L"none") != 0) {
			WIN32_FIND_DATA FindFileData;
			_stprintf(searchPattern, L"%s%s\\*.*", ExePath, ModsFolder);
			HANDLE hFind = FindFirstFile(searchPattern, &FindFileData);

			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if ((_tcscmp(FindFileData.cFileName, L".") == 0)
						|| (_tcscmp(FindFileData.cFileName, L"..") == 0)
						|| (_tcsncmp(FindFileData.cFileName, L"-", 1) == 0)) {
						continue;
					}

					if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						FolderNames.push_back(FindFileData.cFileName);
				} while (FindNextFile(hFind, &FindFileData));

				FindClose(hFind);

				std::sort(FolderNames.begin(), FolderNames.end());

				for (std::vector<wstring>::iterator Name = FolderNames.begin(); Name != FolderNames.end(); ++Name)
				{
					_stprintf(listFilesPath1, L"%s%s\\%s\\", ExePath, ModsFolder, Name->c_str());
					_stprintf(listFilesPath2, L"%s\\%s\\", ModsFolder, Name->c_str());
					ListFiles(listFilesPath1, listFilesPath1, listFilesPath2);
				}
			}
		}
	}
	catch (...) {
		TRACE(L"Error creating Files List from Folder %s\r\n", ModsFolder);
	}

	StopWatchStop(pfPikoSeconds);
}

//************************************
// Method:    CreateCachedModsFolderList
// FullName:  CreateCachedModsFolderList
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: float * pfPikoSeconds
//************************************
bool CreateCachedModsFolderList(float * pfPikoSeconds)
{
	bool bRetVal = false;
	CHAR cCachedListFileName[MAX_PATH];

	try {
		if (ModsFolder != L"none") {
			StopWatchStart(pfPikoSeconds);
			_stprintf(CachedListFileName, L"%s%s\\~wrapper.cache", ExePath, ModsFolder);

			if (FileExists(CachedListFileName)) {
				bRetVal = ReadCachedFileList(CachedListFileName);
			}

			if (!bRetVal) {
				wcstombs(cCachedListFileName, CachedListFileName, MAX_PATH);
				g_cachedListFile = fopen(cCachedListFileName, "wt");
				CreateModsFolderList(NULL);
				fflush(g_cachedListFile);
				fclose(g_cachedListFile);
				g_cachedListFile = NULL;
			}

			StopWatchStop(pfPikoSeconds);
		}
		else {
			bRetVal = true;
		}
	}
	catch (...) {
		TRACE(L"Error creating cached Files List from Folder %s\r\n", ModsFolder);
	}

	return bRetVal;
}

//************************************
// Method:    CreateFilesFolderList
// FullName:  CreateFilesFolderList
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: float * pfPikoSeconds
//************************************
void CreateFilesFolderList(float * pfPikoSeconds)
{
	StopWatchStart(pfPikoSeconds);
	TCHAR listFilesPath1[MAX_PATH];
	TCHAR listFilesPath2[MAX_PATH];

	try {
		if (FilesFolder != L"none") {
			_stprintf(listFilesPath1, L"%s%s\\", ExePath, FilesFolder);
			_stprintf(listFilesPath2, L"%s\\", FilesFolder);
			ListFiles(listFilesPath1, listFilesPath1, listFilesPath2);
		}
	}
	catch (...) {
		TRACE(L"Error creating Files List from Folder %s\r\n", FilesFolder);
	}

	StopWatchStop(pfPikoSeconds);
}

//************************************
// Method:    CreateCachedFilesFolderList
// FullName:  CreateCachedFilesFolderList
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: float * pfPikoSeconds
//************************************
bool CreateCachedFilesFolderList(float * pfPikoSeconds)
{
	bool bRetVal = false;
	CHAR cCachedListFileName[MAX_PATH];

	try {
		if (FilesFolder != L"none") {
			StopWatchStart(pfPikoSeconds);
			_stprintf(CachedListFileName, L"%s%s\\~wrapper.cache", ExePath, FilesFolder);

			if (FileExists(CachedListFileName)) {
				bRetVal = ReadCachedFileList(CachedListFileName);
			}

			if (!bRetVal) {
				wcstombs(cCachedListFileName, CachedListFileName, MAX_PATH);
				g_cachedListFile = fopen(cCachedListFileName, "wt");
				CreateFilesFolderList(NULL);
				fflush(g_cachedListFile);
				fclose(g_cachedListFile);
				g_cachedListFile = NULL;
			}

			StopWatchStop(pfPikoSeconds);
		}
		else {
			bRetVal = true;
		}
	}
	catch (...) {
		TRACE(L"Error creating cached Files List from Folder %s\r\n", FilesFolder);
	}

	return bRetVal;
}

//************************************
// Method:    ReadCachedFileList
// FullName:  ReadCachedFileList
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const TCHAR * pCachedFileListName
//************************************
bool ReadCachedFileList(LPCTSTR pCachedFileListName)
{
	CHAR cCachedListFileName[MAX_PATH];
	CHAR cachedFileLineBuf[MAX_PATH * 2];
	wcstombs(cCachedListFileName, pCachedFileListName, MAX_PATH);

	try {
		g_cachedListFile = fopen(cCachedListFileName, "rt");

		if (g_cachedListFile == NULL) {
			return false;
		}

		while (!feof(g_cachedListFile)) {
			fgets(cachedFileLineBuf, MAX_PATH * 2, g_cachedListFile);
			int iLineLen = strlen(cachedFileLineBuf);
			while (((cachedFileLineBuf[iLineLen - 1] == '\r')
				|| (cachedFileLineBuf[iLineLen - 1] == '\n'))
				&& iLineLen > 0) {
				cachedFileLineBuf[iLineLen - 1] = '\0';
				iLineLen--;
			}

			if (iLineLen > 20) {  // ensure the line is valid
				try {
					if (strstr(cachedFileLineBuf, "~wrapper.cache") != NULL) {
						continue;
					}

					LPSTR fileString = strstr(cachedFileLineBuf, "?");
					strcpy(fileString, "\0");
					fileString++;
					MyFileListItem* MyStruct = new MyFileListItem();
					MyStruct->filePath = new CHAR[strlen(fileString) + 1];
					strcpy(MyStruct->filePath, fileString);
					sscanf(cachedFileLineBuf, "%I64X", &MyStruct->hash);
					MyStruct->dwIndex = g_dwIndex++;
					FileList.push_back(MyStruct);
				}
				catch (...) {
					TRACE(L"Error Parsing Line \"%s\" from wrapper cache file \"%s\"!\r\n", cachedFileLineBuf, pCachedFileListName);
				}
			}
		}
	}
	catch (...) {
		TRACE("Error reading cached file list\r\n");
	}

	fclose(g_cachedListFile);
	g_cachedListFile = NULL;
	return true;
}

//************************************
// Method:    GetCommandLineParams
// FullName:  GetCommandLineParams
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void GetCommandLineParams()
{
	try {
		if (IsServerExe()) {
			_stprintf(IniFileName, L"%s%s", ExePath, SERVER_INI);
			TRACE("Server Launcher detected.\r\n");
		}
		else {
			_stprintf(IniFileName, L"%s%s", ExePath, CLIENT_INI);
		}

		int iModType = GetPrivateProfileInt(L"Settings", L"ModType", 1, IniFileName);
		TCHAR modTypeSection[16];
		_stprintf(modTypeSection, L"Modtype_%02d", iModType);
		GetPrivateProfileString(modTypeSection, L"Files", L"none", FilesFolder, MAX_PATH, IniFileName);
		GetPrivateProfileString(modTypeSection, L"Mods", L"none", ModsFolder, MAX_PATH, IniFileName);
		g_bUseCachedFileLists = GetPrivateProfileInt(L"Settings", L"UseCachedFileLists", false, IniFileName) != 0;

		if ((iModType != 0)
			&& (_tcsicmp(FilesFolder, L"none") == 0)
			&& (_tcsicmp(ModsFolder, L"none") == 0)) {
			TRACE("Modtype != 0 but neither Files nor Mods Folder set, using static assignments!");
			TRACE("***       It seems you are using an outdated il2fb.ini file format!       ***");

			switch (iModType) {
			case 1:
				_tcscpy(FilesFolder, L"FILES");
				_tcscpy(ModsFolder, L"MODS");
				break;

			case 2:
				_tcscpy(FilesFolder, L"none");
				_tcscpy(ModsFolder, L"#SAS");
				break;

			case 3:
				_tcscpy(FilesFolder, L"none");
				_tcscpy(ModsFolder, L"#UP#");
				break;

			case 4:
				_tcscpy(FilesFolder, L"none");
				_tcscpy(ModsFolder, L"#DBW");
				break;

			case 5:
				_tcscpy(FilesFolder, L"none");
				_tcscpy(ModsFolder, L"#DBW_1916");
				break;

			case 0:
			default:
				_tcscpy(FilesFolder, L"none");
				_tcscpy(ModsFolder, L"none");
				break;
			}
		}

		LPTSTR *argList;
		int nArgs;
		// Get the argument list
		argList = CommandLineToArgvW(GetCommandLineW(), &nArgs);
		// Parse Command Line Arguments
		if ((argList != NULL) && (nArgs != 0)) {
			for (int i = 1; i < nArgs; i++) {
				_tcscpy(CurParam, argList[i]);
				_tcslwr(CurParam);
				LPTSTR lpCurParam = CurParam;
				TRACE(L"Command Line Parameter No. %d = %s\r\n", i, lpCurParam);
				trim(lpCurParam, L" ");

				if (_tcschr(L"/", lpCurParam[0])) {
					lpCurParam = _tcsinc(lpCurParam);
				}

				if (_tcsstr(lpCurParam, L"f:") == lpCurParam) {
					lpCurParam = _tcsninc(lpCurParam, 2);
					trim(lpCurParam, L"\"");
					_tcscpy(FilesFolder, lpCurParam);
				}
				else if (_tcsstr(lpCurParam, L"m:") == lpCurParam) {
					lpCurParam = _tcsninc(lpCurParam, 2);
					trim(lpCurParam, L"\"");
					_tcscpy(ModsFolder, lpCurParam);
				}
				else if (_tcsstr(lpCurParam, L"lb:") == lpCurParam) {
					lpCurParam = _tcsninc(lpCurParam, 3);
					trim(lpCurParam, L"\"");
					_tcscpy(LinkBackName, ExePath);
					_tcscat(LinkBackName, lpCurParam);
				}
				else if (_tcsstr(lpCurParam, L"cache") == lpCurParam) {
					g_bUseCachedFileLists = true;
				}
			}
		}

		if (_tcslen(ModsFolder) == 0 || _tcsicmp(ModsFolder, L"none") == 0) {
			TRACE("No MODS Folder set\r\n");
		}
		else {
			TRACE(L"MODS Folder = \"%s\"\r\n", ModsFolder);
		}

		if (_tcslen(FilesFolder) == 0 || _tcsicmp(FilesFolder, L"none") == 0) {
			TRACE("No FILES Folder set.\r\n");
		}
		else {
			TRACE(L"FILES Folder = \"%s\".\r\n", FilesFolder);
		}

		int iDumpMode = GetPrivateProfileInt(L"Settings", L"DumpMode", 0, IniFileName);
		if (iDumpMode & 0x01) g_bDumpFileAccess = TRUE;
		if (iDumpMode & 0x02) g_bDumpClassFiles = TRUE;
		if (iDumpMode & 0x04) g_bDumpOtherFiles = TRUE;

	}
	catch (...) {
		TRACE("Error reading command line parameters / ini file\r\n");
	}
}

//************************************
// Method:    LinkIl2fbExe
// FullName:  LinkIl2fbExe
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void LinkIl2fbExe()
{
	try {
		if (hExecutable == NULL) {
			TRACE(L"Trying to link back to %s through LoadLibrary()\r\n", LinkBackName);
			hExecutable = LoadLibrary(LinkBackName);
		}

		if (hExecutable == NULL) {
			TRACE(L"Linking back to %s through LoadLibrary() failed, linking back to calling process.\r\n", LinkBackName);
			hExecutable = GetModuleHandle(NULL);
		}

		if (hExecutable == NULL) {
			TRACE("Linking back to calling process failed.\r\n");
			MessageBox(NULL, L"Attaching to IL-2 Executable failed!", L"SAS wrapper", MB_OK | MB_ICONERROR | MB_TOPMOST);
			ExitProcess(-1);
		}

		if (SFS_open == NULL) {
			SFS_open = (TSFS_open*)GetProcAddress(hExecutable, "SFS_open");
		}

		if (SFS_open == NULL) {
			SFS_open = (TSFS_open*)GetProcAddress(hExecutable, (LPCSTR)0x87);
		}

		if (SFS_open == NULL) {
			HINSTANCE hinst = LoadLibrary(L"DT.dll");
			SFS_open_cpp = (TSFS_open_cpp*)GetProcAddress(hinst, "SFS_open@8");
		}

		if (SFS_open == NULL && SFS_open_cpp == NULL) {
			TRACE("SFS open function pointer missing.\r\n");
			MessageBox(NULL, L"SFS open function pointer missing!", L"SAS wrapper", MB_OK | MB_ICONERROR | MB_TOPMOST);
			ExitProcess(-1);
		}

		if (SFS_openf == NULL) {
			SFS_openf = (TSFS_openf*)GetProcAddress(hExecutable, "SFS_openf");
		}

		if (SFS_openf == NULL) {
			SFS_openf = (TSFS_openf*)GetProcAddress(hExecutable, (LPCSTR)0x88);
		}

		if (SFS_openf == NULL) {
			HINSTANCE hinst = LoadLibrary(L"DT.dll");
			SFS_openf_cpp = (TSFS_openf_cpp*)GetProcAddress(hinst, "SFS_openf@12");
		}
			
		if (SFS_openf == NULL && SFS_openf_cpp == NULL) {
			TRACE("SFS openf function pointer missing.\r\n");
			MessageBox(NULL, L"SFS openf function pointer missing!", L"SAS wrapper", MB_OK | MB_ICONERROR | MB_TOPMOST);
			ExitProcess(-1);
		}

		//if (SFS_read == NULL) {
		//	SFS_read = (TSFS_read*)GetProcAddress(hExecutable, "SFS_read");
		//}
		//if (SFS_lseek == NULL) {
		//	SFS_lseek = (TSFS_lseek*)GetProcAddress(hExecutable, "SFS_lseek");
		//}

	}
	catch (...) {
		TRACE("Error creating backlink to IL-2 executable\r\n");
	}
}


//************************************
// Method:    RemoveDuplicates
// FullName:  RemoveDuplicates
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: float * pfPikoSeconds
//************************************
int RemoveDuplicates(float * pfPikoSeconds)
{
	StopWatchStart(pfPikoSeconds);
	int iRetVal = 0;

	if (!FileList.empty()) {
		try {
			MyFileListItem* dupElement = NULL;

			for (int i = 0; i < (int)FileList.size(); i++) {
				MyFileListItem* listElement = FileList[i];

				if (dupElement != NULL) {
					if (listElement->hash == dupElement->hash) {
						listElement->filePath = dupElement->filePath;
						iRetVal++;
						continue;
					}
				}

				dupElement = listElement;
			}
		}
		catch (...) {
			TRACE("Error removing duplicates from list\r\n");
		}
	}

	StopWatchStop(pfPikoSeconds);
	return iRetVal;
}


//************************************
// Method:    binarySearchFileList
// FullName:  binarySearchFileList
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: unsigned __int64 theHash
//************************************
int binarySearchFileList(unsigned __int64 theHash)
{
	if (FileList.empty()) return -1;
	float fPikoSeconds = 0.;
	StopWatchStart(&fPikoSeconds);
	int first = 0;
	int last = FileList.size() - 1;
	int mid = 0;

	while (first < last) {
		g_SearchIterations++;
		unsigned __int64 hashSpan = FileList[last]->hash - FileList[first]->hash;
		unsigned __int64 hashDistance = theHash - FileList[first]->hash;
		float fHashDistance = (float)hashDistance / (float)hashSpan;

		if (fHashDistance > 1.0) {
			return -1;
		}

		mid = (int)((last - first) * fHashDistance) + first;

		if (theHash > FileList[mid]->hash) {
			first = mid + 1;
		}
		else if (theHash < FileList[mid]->hash) {
			last = mid - 1;
		}
		else {
			StopWatchStop(&fPikoSeconds);
			g_SearchPikoseconds += fPikoSeconds;
			return mid;      // found it. return position
		}
	}

	if (theHash == FileList[mid]->hash) {
		return mid;
	}

	return -1;     // failed to find key
}

unsigned __int64 SFS_hashW(const unsigned __int64 hash, LPCTSTR buf, const int len)
{
	ZeroMemory(cBuf, sizeof(cBuf));
	wcstombs(cBuf, buf, _tcslen(buf));
	unsigned __int64 retVal = SFS_hash(hash, cBuf, len);
	return retVal;
}

bool isHashedClass(LPCTSTR lpFilePath, LPCTSTR lpFileName) {
	if (!(_tcslen(lpFileName) == 16)) return false;
	for (UINT32 i = 0; i < _tcslen(lpFileName); i++)
		if (!isxdigit(lpFileName[i])) return false;
	return true;
}

LPTSTR _tcsreplace(LPTSTR theString, char theCharToReplace, char replaceWith) {
	while (*theString != L'\0') {
		if (*theString == theCharToReplace)
			*theString = replaceWith;
		theString++;
	}
	return theString;
}

void ListFiles(LPCTSTR lpParent, LPCTSTR lpRoot, LPCTSTR lpAddFront)
{
	TCHAR Dir[MAX_PATH];
	TCHAR foundItem[MAX_PATH];
	TCHAR foundItemUpperCase[MAX_PATH];
	TCHAR searchPattern[MAX_PATH];
	std::vector<wstring> FolderNames;
	std::vector<wstring> FileNames;
	_tcscpy(Dir, lpParent);

	if (_tcschr(L"\\", Dir[_tcslen(Dir) - 1]) == NULL) {
		_tcscat(Dir, L"\\");
	}

	WIN32_FIND_DATA FindFileData;
	_stprintf(searchPattern, L"%s*.*", Dir);
	HANDLE hFind = FindFirstFile(searchPattern, &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if ((_tcscmp(FindFileData.cFileName, L".") == 0)
				|| (_tcscmp(FindFileData.cFileName, L"..") == 0)) {
				continue;
			}
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				FolderNames.push_back(FindFileData.cFileName);
			else
				FileNames.push_back(FindFileData.cFileName);
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);

		std::sort(FolderNames.begin(), FolderNames.end());
		std::sort(FileNames.begin(), FileNames.end());

		for (std::vector<wstring>::iterator Name = FolderNames.begin(); Name != FolderNames.end(); ++Name)
		{
			_stprintf(foundItem, L"%s%s", Dir, Name->c_str());
			ListFiles(foundItem, lpRoot, lpAddFront);
		}

		for (std::vector<wstring>::iterator Name = FileNames.begin(); Name != FileNames.end(); ++Name)
		{
			_stprintf(foundItem, L"%s%s", Dir, Name->c_str());
			MyFileListItem* MyStruct = new MyFileListItem();
			LPTSTR foundItemFromRoot = foundItem + _tcslen(lpRoot);
			int iFilePathLen = _tcslen(lpAddFront) + _tcslen(foundItemFromRoot);
			TCHAR* szFilePath = (TCHAR*)calloc((iFilePathLen + 1), sizeof(TCHAR));
			_stprintf(szFilePath, L"%s%s", lpAddFront, foundItemFromRoot);
			MyStruct->filePath = new CHAR[iFilePathLen + 1];
			wcstombs(MyStruct->filePath, szFilePath, iFilePathLen);
			MyStruct->filePath[iFilePathLen] = '\0';
			free(szFilePath);
			_tcscpy(foundItemUpperCase, foundItemFromRoot);
			_tcsupr(foundItemUpperCase);
			int foundLen = _tcslen(Name->c_str());
			if (isHashedClass(foundItem, Name->c_str())) // Fixed 2017-08-30, no need to double-hash files
				MyStruct->hash = _tcstoull(Name->c_str(), NULL, 16);
			else if (foundLen > 6 && _tcsnicmp(&(Name->c_str())[foundLen - 6], L".class", 6) == 0) { // New 2017-08-30, read clearname class files
				ZeroMemory(szBuf, sizeof(szBuf));
				foundItemFromRoot[_tcslen(foundItemFromRoot) - 6] = '\0';
				_tcsreplace(foundItemFromRoot, L'\\', L'.');
				_tcsreplace(foundItemFromRoot, L'/', L'.');
				_stprintf(szBuf, L"sdw%scwc2w9e", foundItemFromRoot);
				_stprintf(szBuf, L"cod/%d", IntFN(0, szBuf));
				MyStruct->hash = LongFN(0, szBuf);
			}
			else
				MyStruct->hash = SFS_hashW(0, foundItemUpperCase, _tcslen(foundItemUpperCase));
			MyStruct->dwIndex = g_dwIndex++;
			FileList.push_back(MyStruct);

			if (g_cachedListFile != NULL) {
				fprintf(g_cachedListFile, "%016I64X?%s\n", MyStruct->hash, MyStruct->filePath);
			}
		}
	}
}

unsigned __int64 SFS_hash(const unsigned __int64 hash, const void *buf, const int len)
{
	unsigned char c;
	unsigned a = (unsigned)(hash & 0xFFFFFFFF);
	unsigned b = (unsigned)(hash >> 32 & 0xFFFFFFFF);

	for (int i = 0; i < len; i++) {
		c = ((unsigned char *)buf)[i];
		a = (a << 8 | c) ^ FPaTable[a >> 24];
		b = (b << 8 | c) ^ FPbTable[b >> 24];
	}

	return (unsigned __int64)a & 0xFFFFFFFF | (unsigned __int64)b << 32;
}

unsigned __int64 LongFN(unsigned __int64 paramLong, LPCTSTR paramString) {
	int paramLen = _tcslen(paramString);
	unsigned __int32 c;
	unsigned __int32 a = (unsigned)(paramLong & 0xFFFFFFFF);
	unsigned __int32 b = (unsigned)(paramLong >> 32 & 0xFFFFFFFF);
	for (int i = 0; i < paramLen; i++) {
		c = (unsigned __int32)paramString[i];
		if ((c > 96) && (c < 123))
			c &= 223;
		else if (c == 47)
			c = 92;
		a = (a << 8 | c) ^ FPaTable[a >> 24];
		b = (b << 8 | c) ^ FPbTable[b >> 24];
	}
	return ((unsigned __int64)a & 0xFFFFFFFF) | ((unsigned __int64)b << 32);
}

unsigned __int32 IntFN(unsigned __int32 paramInt, LPCTSTR paramString) {
	int paramLen = _tcslen(paramString);
	unsigned __int32 c;
	unsigned __int32 b;
	unsigned __int32 a = paramInt;
	for (int i = 0; i < paramLen; i++) {
		c = (unsigned __int32)paramString[i];
		b = c & 0xFF;
		a = (a << 8 | b) ^ FPaTable[a >> 24];
		b = c >> 8 & 0xFF;
		a = (a << 8 | b) ^ FPaTable[a >> 24];
	}
	return (unsigned __int32)a;
}

void StopWatchStart(float *pfPikoSeconds)
{
	if (pfPikoSeconds != NULL) {
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&First));
	}
}

void StopWatchStop(float *pfPikoSeconds)
{
	if (pfPikoSeconds != NULL) {
		ULONGLONG Last;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&Last));
		ULONG ulEclapsedCount = (ULONG)(Last - First);
		float fDiffSecs = float(ulEclapsedCount) / float(ulFreq);
		*pfPikoSeconds = fDiffSecs * MULTIPLIER_PIKOSECONDS; //Pikoseconds
	}
}

BOOL FileExists(LPCTSTR lpcFilename)
{
	return((GetFileAttributes(lpcFilename) == INVALID_FILE_ATTRIBUTES)
		? FALSE
		: TRUE);
}

bool IsServerExe()
{
	DWORD dwOldProtect = 0;
	DWORD dwProcRights = DELETE | READ_CONTROL | SYNCHRONIZE | WRITE_DAC | WRITE_OWNER
		| PROCESS_TERMINATE | PROCESS_CREATE_THREAD | PROCESS_SET_SESSIONID | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_DUP_HANDLE | PROCESS_CREATE_PROCESS | PROCESS_SET_QUOTA | PROCESS_SET_INFORMATION | PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME;
	DWORD dwCurProc = GetCurrentProcessId();
	HANDLE hCurProc = OpenProcess(dwProcRights, FALSE, dwCurProc);
	bool bIsServer = false;

	try {
		int iIl2ServerID = 0x0041EBA8;
		char* cFilesServer = "filesserver.sfs";

		if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2ServerID, 15, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
			if (memcmp((LPVOID)iIl2ServerID, cFilesServer, 15) == 0) {
				bIsServer = true;
			}

			VirtualProtectEx(hCurProc, (LPVOID)iIl2ServerID, 9, dwOldProtect, NULL);
		}
		if (!bIsServer) {
			int iIl2ServerID = 0x00408d8d; // il2server.exe 4.15
			if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2ServerID, 15, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
				if (memcmp((LPVOID)iIl2ServerID, cFilesServer, 15) == 0) {
					bIsServer = TRUE;
				}

				VirtualProtectEx(hCurProc, (LPVOID)iIl2ServerID, 9, dwOldProtect, NULL);
			}
			else {
				TRACE("Error in IsServerExe: Couldn't get access to modded il2fb.exe server ID memory area!\r\n");
			}
		}
	}
	catch (...) {
		TRACE("Error in IsServerExe: Couldn't check modded il2fb.exe server ID!\r\n");
	}

	CloseHandle(hCurProc);
	return bIsServer;
}

bool DirectoryExists(LPCTSTR dirName_in)
{
	DWORD ftyp = GetFileAttributes(dirName_in);

	if (ftyp == INVALID_FILE_ATTRIBUTES) {
		return false;    //something is wrong with your path!
	}

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
		return true;    // this is a directory!
	}

	return false;    // this is not a directory!
}

TCHAR * trim(TCHAR * c, TCHAR*space)
{
	int i = 0;

	while (c[0] && _tcschr(space, c[0])) {
		c = _tcsinc(c);
	}

	i = _tcslen(c);

	do {
		c[i] = 0;
		i--;
	} while (i >= 0 && _tcschr(space, c[i]));

	return c;
}

//SASIL2WRAPPER_C_API void __cdecl ReadDump(void *buf, unsigned int len)
//{
//	return;
//}

#ifdef USE_415_CODE
SASIL2WRAPPER_C_API int __stdcall __SFS_openf(const unsigned __int64 hash, const int flags)
#else
SASIL2WRAPPER_C_API int __cdecl __SFS_openf(const unsigned __int64 hash, const int flags)
#endif
{
	g_SFSOpenfCalls++;
	int filePointer = -1;
	int listPos;
	unsigned __int64 hash2 = 0;
	int foundIn = -1;
	if (!FileList.empty()) {
		listPos = binarySearchFileList(hash);

		if (listPos == -1) {
			ZeroMemory(szHashString, sizeof(szHashString));
			_stprintf(szHashString, L"%016I64X", hash);
			hash2 = SFS_hashW(0, szHashString, _tcslen(szHashString));
			listPos = binarySearchFileList(hash2);
			if (listPos != -1) foundIn = 2;
		}
		else {
			foundIn = 1;
		}

		//TRACE("__SFS_openf(%016I64X, %d) - hash2=%016I64X, listPos=%d\r\n", hash, flags, hash2, listPos);

		if (listPos != -1) {
			if (SFS_open != NULL)
				filePointer = SFS_open(FileList[listPos]->filePath, flags);
			else
				filePointer = SFS_open_cpp(FileList[listPos]->filePath, flags);
		}
	}

	if (filePointer == -1) {
		foundIn = 0;
		if (SFS_openf != NULL)
			filePointer = SFS_openf(hash, flags);
		else
			filePointer = SFS_openf_cpp(hash, flags);
	}

	if (filePointer == -1) {
		g_SFSOpenf_not_found++;
	}
	else {
		switch (foundIn) {
			case 0:
				g_SFSOpenf_from_SFS++;
				break;
			case 1:
				g_SFSOpenf_found_hash1++;
				break;
			case 2:
				g_SFSOpenf_found_hash2++;
				break;
			default:
				break;
		}
	}

	return filePointer;
}

SASIL2WRAPPER_C_API int __stdcall _SAS_openf(const unsigned __int64 hash, const int flags)
{
	//TRACE("__SAS_openf(%016I64X, %d)\r\n", hash, flags);
	return __SFS_openf(hash, flags);
}