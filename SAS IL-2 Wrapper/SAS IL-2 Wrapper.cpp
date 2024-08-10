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
//#include <windows.h>
#include <Shellapi.h>

#include "SAS IL-2 Wrapper.h"
#include "sfs.h"
#include "trace.h"

#include "resource.h"
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
TSFS_open_cpp* SFS_open_cpp = NULL;
TSFS_openf_cpp* SFS_openf_cpp = NULL;

std::wstring logFileName, filesFolder, modsFolder, cachedListFileName, exeName, exePath, iniFileName, curParam, linkBackName;

std::vector<MyFileListItem*> fileList;

bool g_bDumpFileAccess = FALSE;
bool g_bDumpClassFiles = FALSE;
bool g_bDumpOtherFiles = FALSE;

//************************************
// Method:    ThreadAttach
// FullName:  ThreadAttach
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void threadAttach()
{
	iThreadAttachCounter++;
	TRACE(L"ThreadAttach, attached Threads =  %d\r\n", iThreadAttachCounter);
}

//************************************
// Method:    ThreadDetach
// FullName:  ThreadDetach
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void threadDetach()
{
	iThreadAttachCounter--;
	TRACE(L"ThreadDetach, attached Threads =  %d\r\n", iThreadAttachCounter);
}

//************************************
// Method:    StartWrapper
// FullName:  StartWrapper
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HINSTANCE hInstance
//************************************
void startWrapper(HINSTANCE hInstance)
{
#ifdef _DEBUG
	HWND hSplash = FindWindow(L"Il2SASLauncherWnd", NULL);
	if (hSplash != NULL) PostMessage(hSplash, WM_CLOSE, 0, 0);
#endif
	iProcessAttachCounter++;
	if (iProcessAttachCounter == 1) {
		exeName = getCurrentExecutablePathName();
		linkBackName = exeName;
		exePath = getDirectoryWithCurrentExecutable() + L"\\";
		logFileName = exePath + LOGFILE_NAME;
	}
	TRACE(L"ProcessAttach, attached Processes =  %d\r\n", iProcessAttachCounter);

	if (iProcessAttachCounter > 1) {
		return;
	}

	filesFolder = L"FILES";
	modsFolder = L"MODS";
	fileList.clear();
	ULONGLONG ullFreq;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&ullFreq));
	ulFreq = (ULONG)ullFreq;
	g_dwIndex = 0;
	TRACE(L"Calling getCommandLineParams()\r\n");
	getCommandLineParams();
	TRACE(L"Calling linkIl2fbExe()\r\n");
	linkIl2fbExe();
	float fCreateMods = 0, fCreateFiles = 0, fSortList = 0, fRemoveDups = 0;

	try {
		if (!iequals(modsFolder, L"none")) {
			if (directoryExists(modsFolder)) {
				cachedListFileName = exePath + modsFolder + L"\\~wrapper.cache";
				if (fileExists(cachedListFileName)) {
					DeleteFileW(cachedListFileName.c_str());
				}
				TRACE(L"Calling createModsFolderList()\r\n");
				createModsFolderList(&fCreateMods);
				TRACE(L"Scanning %s folder took %.0f milliseconds.\r\n", modsFolder.c_str(), fCreateMods / DIVIDER_MILLISECONDS);
			}
		}
	}
	catch (...) {
		TRACE(L"Error loading files from Folder %s\r\n", modsFolder.c_str());
	}

	try {
		if (!iequals(filesFolder, L"none")) {
			if (directoryExists(filesFolder)) {
				cachedListFileName = exePath + filesFolder + L"\\~wrapper.cache";
				if (fileExists(cachedListFileName)) {
					DeleteFileW(cachedListFileName.c_str());
				}
				TRACE(L"Calling createFilesFolderList()\r\n");
				createFilesFolderList(&fCreateFiles);
				TRACE(L"Scanning %s folder took %.0f milliseconds.\r\n", filesFolder.c_str(), fCreateFiles / DIVIDER_MILLISECONDS);
			}
		}
	}
	catch (...) {
		TRACE(L"Error loading files from Folder %s\r\n", filesFolder.c_str());
	}

	TRACE(L"Total number of modded files = %d.\r\n", g_dwIndex);
	TRACE(L"Calling sortList()\r\n");
	sortList(&fSortList);
	TRACE(L"Sorting modded files list took %.3f milliseconds.\r\n", fSortList / DIVIDER_MILLISECONDS);
	TRACE(L"Calling removeDuplicates()\r\n");
	int iDups = removeDuplicates(&fRemoveDups);
	TRACE(L"Removing %d Duplicates took %.3f milliseconds.\r\n", iDups, fRemoveDups / DIVIDER_MILLISECONDS);
}

//************************************
// Method:    StopWrapper
// FullName:  StopWrapper
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void stopWrapper()
{
	iProcessAttachCounter--;
	TRACE(L"ProcessDetach, attached Processes =  %d\r\n", iProcessAttachCounter);

	if (iProcessAttachCounter > 0) {
		return;
	}

	fileList.clear();
	TRACE(L"Total files opened = %d\r\n", g_SFSOpenfCalls);
	TRACE(L"Files loaded from SFS archives = %d\r\n", g_SFSOpenf_from_SFS);
	TRACE(L"Files loaded from mod folders  = %d\r\n", g_SFSOpenf_found_hash1 + g_SFSOpenf_found_hash2);
	TRACE(L"  ~ using 1st level hash match = %d\r\n", g_SFSOpenf_found_hash1);
	TRACE(L"  ~ using 2nd level hash match = %d\r\n", g_SFSOpenf_found_hash2);
	TRACE(L"Files not found                = %d\r\n", g_SFSOpenf_not_found);
	float fTotalSearchSeconds = g_SearchPikoseconds / DIVIDER_SECONDS;
	TRACE(L"Total search time consumed = %.3f milliseconds (%.12f Seconds)\r\n", g_SearchPikoseconds / DIVIDER_MILLISECONDS, fTotalSearchSeconds);
	float fTest = g_SearchPikoseconds / float(g_SFSOpenfCalls) / 1000.0f;
	TRACE(L"Search Time per File = %.3f nanoseconds (%.12f Seconds)\r\n", g_SearchPikoseconds / float(g_SFSOpenfCalls) / 1000.0f, g_SearchPikoseconds / float(g_SFSOpenfCalls) / DIVIDER_SECONDS);
	float fSearchIterationsPerFile = float(g_SearchIterations) / float(g_SFSOpenfCalls);
	TRACE(L"Average Search Iterations required per File = %.1f\r\n", fSearchIterationsPerFile);

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
int __cdecl compareFileList(const void* mfli1, const void* mfli2)
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
void sortList(float* pfPikoSeconds)
{
	stopWatchStart(pfPikoSeconds);
	if (!fileList.empty()) std::qsort(&*fileList.begin(), fileList.size(), sizeof(MyFileListItem*), compareFileList);   // sort and keep original relative order of equivalent elements.
	stopWatchStop(pfPikoSeconds);
}

//************************************
// Method:    CreateModsFolderList
// FullName:  CreateModsFolderList
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: float * pfPikoSeconds
//************************************
void createModsFolderList(float* pfPikoSeconds)
{
	stopWatchStart(pfPikoSeconds);
	std::vector<wstring> folderNames;

	try {
		if (!iequals(modsFolder, L"none")) {
			WIN32_FIND_DATA FindFileData;
			std::wstring searchPattern = exePath + modsFolder + L"\\*.*";
			HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &FindFileData);

			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					std::wstring fileName(FindFileData.cFileName);
					if (fileName._Equal(L".")
						|| fileName._Equal(L"..")
						|| fileName._Starts_with(L"-")) {
						continue;
					}
					if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						folderNames.push_back(fileName);
				} while (FindNextFile(hFind, &FindFileData));

				FindClose(hFind);

				std::sort(folderNames.begin(), folderNames.end());

				for (std::vector<wstring>::iterator folderName = folderNames.begin(); folderName != folderNames.end(); ++folderName)
				{
					std::wstring listFilesPath1 = exePath + modsFolder + L"\\" + *folderName + L"\\";
					std::wstring listFilesPath2 = modsFolder + L"\\" + *folderName + L"\\";
					listFiles(listFilesPath1, listFilesPath1, listFilesPath2);
				}
			}
		}
	}
	catch (...) {
		TRACE(L"Error creating Files List from Folder %s\r\n", modsFolder.c_str());
	}

	stopWatchStop(pfPikoSeconds);
}

//************************************
// Method:    CreateFilesFolderList
// FullName:  CreateFilesFolderList
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: float * pfPikoSeconds
//************************************
void createFilesFolderList(float * pfPikoSeconds)
{
	stopWatchStart(pfPikoSeconds);

	try {
		if (!iequals(filesFolder, L"none")) {
			std::wstring listFilesPath1 = exePath + filesFolder + L"\\";
			std::wstring listFilesPath2 = filesFolder + L"\\";
			listFiles(listFilesPath1, listFilesPath1, listFilesPath2);
		}
	}
	catch (...) {
		TRACE(L"Error creating Files List from Folder %s\r\n", filesFolder.c_str());
	}

	stopWatchStop(pfPikoSeconds);
}

//************************************
// Method:    GetCommandLineParams
// FullName:  GetCommandLineParams
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void getCommandLineParams()
{
	try {
		if (isServerExe()) {
			iniFileName = exePath + SERVER_INI;
			TRACE(L"Server Launcher detected.\r\n");
		}
		else {
			iniFileName = exePath + CLIENT_INI;
		}
		int iModType = GetPrivateProfileIntW(L"Settings", L"ModType", 1, iniFileName.c_str());

		wchar_t* format = L"Modtype_%02d";
		int size = std::swprintf(nullptr, 0, format, iModType);
		std::wstring modTypeSection(size + 1, '\0');
		std::swprintf(&modTypeSection[0], modTypeSection.size(), format, iModType);
		LPWSTR iniBuf = new WCHAR[MAX_PATH];
		GetPrivateProfileStringW(modTypeSection.c_str(), L"Files", L"none", iniBuf, MAX_PATH, iniFileName.c_str());
		filesFolder = std::wstring(iniBuf);
		GetPrivateProfileStringW(modTypeSection.c_str(), L"Mods", L"none", iniBuf, MAX_PATH, iniFileName.c_str());
		modsFolder = std::wstring(iniBuf);
		delete[] iniBuf;

		if (iModType != 0
			&& iequals(filesFolder, L"none")
			&& iequals(modsFolder, L"none")) {
			TRACE(L"Modtype != 0 but neither Files nor Mods Folder set, using static assignments!");
			TRACE(L"***       It seems you are using an outdated il2fb.ini file format!       ***");

			switch (iModType) {
			case 1:
				filesFolder = FILES_DEFAULT;
				modsFolder = MODS_DEFAULT;
				break;

			case 2:
				filesFolder = FILES_MODACT3;
				modsFolder = MODS_MODACT3;
				break;

			case 3:
				filesFolder = FILES_UP3;
				modsFolder = MODS_UP3;
				break;

			case 4:
				filesFolder = FILES_DBW;
				modsFolder = MODS_DBW;
				break;

			case 5:
				filesFolder = FILES_DBW_1916;
				modsFolder = MODS_DBW_1916;
				break;

			case 0:
			default:
				filesFolder = MODS_STOCK;
				modsFolder = FILES_STOCK;
				break;
			}
		}

		LPWSTR *argList;
		int nArgs;
		argList = CommandLineToArgvW(GetCommandLineW(), &nArgs);
		if ((argList != NULL) && (nArgs != 0)) {
			for (int i = 1; i < nArgs; i++) {
				curParam = toLower(std::wstring(argList[i]));
				trim(curParam);
				TRACE(L"Command Line Parameter No. %d = %s\r\n", i, curParam.c_str());
				if (curParam.rfind(L"/", 0) == 0) {
					curParam = curParam.substr(1);
				}
				if (curParam.rfind(L"f:", 0) == 0) {
					filesFolder = curParam.substr(2);
					trim(filesFolder, L"\"");
				}
				else if (curParam.rfind(L"m:", 0) == 0) {
					modsFolder = curParam.substr(2);
					trim(modsFolder, L"\"");
				}
				else if (curParam.rfind(L"lb:", 0) == 0) {
					linkBackName = curParam.substr(3);
					trim(linkBackName, L"\"");
					linkBackName = exePath + linkBackName;
				}
			}
		}

		if (modsFolder.length() == 0 || iequals(modsFolder, L"none")) {
			TRACE(L"No MODS Folder set\r\n");
		}
		else {
			TRACE(L"MODS Folder = \"%s\"\r\n", modsFolder.c_str());
		}

		if (filesFolder.length() == 0 || iequals(filesFolder, L"none")) {
			TRACE(L"No FILES Folder set\r\n");
		}
		else {
			TRACE(L"FILES Folder = \"%s\"\r\n", filesFolder.c_str());
		}

		int iDumpMode = GetPrivateProfileIntW(L"Settings", L"DumpMode", 0, iniFileName.c_str());
		if (iDumpMode & 0x01) g_bDumpFileAccess = true;
		if (iDumpMode & 0x02) g_bDumpClassFiles = true;
		if (iDumpMode & 0x04) g_bDumpOtherFiles = true;

	}
	catch (...) {
		TRACE(L"Error reading command line parameters / ini file\r\n");
	}
}

//************************************
// Method:    LinkIl2fbExe
// FullName:  LinkIl2fbExe
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void linkIl2fbExe()
{
	try {
		if (hExecutable == NULL) {
			TRACE(L"Trying to link back to %s through LoadLibrary()\r\n", linkBackName.c_str());
			hExecutable = LoadLibrary(linkBackName.c_str());
		}

		if (hExecutable == NULL) {
			TRACE(L"Linking back to %s through LoadLibrary() failed, linking back to calling process.\r\n", linkBackName.c_str());
			hExecutable = GetModuleHandle(NULL);
		}

		if (hExecutable == NULL) {
			TRACE(L"Linking back to calling process failed.\r\n");
			MessageBoxW(NULL, L"Attaching to IL-2 Executable failed!", L"SAS wrapper", MB_OK | MB_ICONERROR | MB_TOPMOST);
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
			TRACE(L"SFS open function pointer missing.\r\n");
			MessageBoxW(NULL, L"SFS open function pointer missing!", L"SAS wrapper", MB_OK | MB_ICONERROR | MB_TOPMOST);
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
			TRACE(L"SFS openf function pointer missing.\r\n");
			MessageBoxW(NULL, L"SFS openf function pointer missing!", L"SAS wrapper", MB_OK | MB_ICONERROR | MB_TOPMOST);
			ExitProcess(-1);
		}
	}
	catch (...) {
		TRACE(L"Error creating backlink to IL-2 executable\r\n");
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
int removeDuplicates(float * pfPikoSeconds)
{
	stopWatchStart(pfPikoSeconds);
	int iRetVal = 0;

	if (!fileList.empty()) {
		try {
			MyFileListItem* dupElement = NULL;

			for (int i = 0; i < (int)fileList.size(); i++) {
				MyFileListItem* listElement = fileList[i];

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
			TRACE(L"Error removing duplicates from list\r\n");
		}
	}

	stopWatchStop(pfPikoSeconds);
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
	if (fileList.empty()) return -1;
	float fPikoSeconds = 0.;
	stopWatchStart(&fPikoSeconds);
	int first = 0;
	int last = fileList.size() - 1;
	int mid = 0;

	while (first < last) {
		g_SearchIterations++;
		unsigned __int64 hashSpan = fileList[last]->hash - fileList[first]->hash;
		unsigned __int64 hashDistance = theHash - fileList[first]->hash;
		float fHashDistance = (float)hashDistance / (float)hashSpan;

		if (fHashDistance > 1.0) {
			return -1;
		}

		mid = (int)((last - first) * fHashDistance) + first;

		if (theHash > fileList[mid]->hash) {
			first = mid + 1;
		}
		else if (theHash < fileList[mid]->hash) {
			last = mid - 1;
		}
		else {
			stopWatchStop(&fPikoSeconds);
			g_SearchPikoseconds += fPikoSeconds;
			return mid;      // found it. return position
		}
	}

	if (theHash == fileList[mid]->hash) {
		return mid;
	}

	return -1;     // failed to find key
}

bool isHashedClass(const std::wstring& filePath, const std::wstring& fileName) {
	if (fileName.length() != 16) return false;
	for (size_t i = 0; i < fileName.length(); i++) {
		if (!std::iswxdigit(fileName.at(i))) return false;
	}
	return true;
}

void listFiles(const std::wstring& parent, const std::wstring& root, const std::wstring& addFront)
{
	std::vector<wstring> folderNames;
	std::vector<wstring> fileNames;

	std::wstring dir = parent;
	if (dir.compare(dir.length() - 1, 1, L"\\") != 0) {
		dir += L"\\";
	}
	WIN32_FIND_DATA FindFileData;
	std::wstring searchPattern = dir + L"*.*";
	HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			std::wstring fileName(FindFileData.cFileName);
			if (fileName.compare(L".") == 0
				|| fileName.compare(L"..") == 0) {
				continue;
			}
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				folderNames.push_back(fileName);
			else
				fileNames.push_back(fileName);
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);

		std::sort(folderNames.begin(), folderNames.end());
		std::sort(fileNames.begin(), fileNames.end());

		for (std::vector<wstring>::iterator name = folderNames.begin(); name != folderNames.end(); ++name)
		{
			std::wstring foundItem = dir + *name;
			listFiles(foundItem, root, addFront);
		}

		for (std::vector<wstring>::iterator name = fileNames.begin(); name != fileNames.end(); ++name)
		{
			std::wstring foundItem = dir + *name;
			MyFileListItem* fileListItem = new MyFileListItem();
			std::wstring foundItemFromRoot = foundItem.substr(root.length());
			using convert_typeX = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_typeX, wchar_t> converterX;
			fileListItem->filePath = converterX.to_bytes(addFront + foundItemFromRoot);
			std::wstring foundItemUpperCase = toUpper(foundItemFromRoot);
			int foundLen = (*name).length();
			if (isHashedClass(foundItem, *name))
				fileListItem->hash = std::wcstoull((*name).c_str(), NULL, 16);   //  _tcstoull(Name->c_str(), NULL, 16);
			else if (foundLen > 6 && iequals((*name).substr((*name).length()-6,6), L".class")) {
				foundItemFromRoot = foundItemFromRoot.substr(0, foundItemFromRoot.length() - 6);
				std::replace(foundItemFromRoot.begin(), foundItemFromRoot.end(), L'\\', L'.');
				std::replace(foundItemFromRoot.begin(), foundItemFromRoot.end(), L'/', L'.');
				std::wstring hash1 = L"sdw" + foundItemFromRoot + L"cwc2w9e";
				std::wstring hash2 = L"cod/" + std::to_wstring((int)intFN(0, hash1));
				fileListItem->hash = longFN(0, hash2);
			}
			else
				fileListItem->hash = sfs_hashW(0, foundItemUpperCase);
			fileListItem->dwIndex = g_dwIndex++;
			fileList.push_back(fileListItem);
			//TRACE("File: Index %d Hash %016I64X Path \"%s\"\r\n", fileListItem->dwIndex, fileListItem->hash, fileListItem->filePath.c_str());
		}
	}
}

unsigned __int64 sfs_hashW(const unsigned __int64 hash, const std::wstring &str)
{
	unsigned char c;
	unsigned a = (unsigned)(hash & 0xFFFFFFFF);
	unsigned b = (unsigned)(hash >> 32 & 0xFFFFFFFF);

	int len = str.length();

	for (int pos = 0; pos < len; pos++) {
		c = ((static_cast<unsigned char>(str.at(pos))));
		a = (a << 8 | c) ^ FPaTable[a >> 24];
		b = (b << 8 | c) ^ FPbTable[b >> 24];
	}

	return (unsigned __int64)a & 0xFFFFFFFF | (unsigned __int64)b << 32;
}

unsigned __int64 longFN(unsigned __int64 i, const std::wstring& str) {
	int len = str.length();
	unsigned __int32 c;
	unsigned __int32 a = (unsigned)(i & 0xFFFFFFFF);
	unsigned __int32 b = (unsigned)(i >> 32 & 0xFFFFFFFF);
	for (int pos = 0; pos < len; pos++) {
		c = (unsigned __int32)(static_cast<unsigned char>(str.at(pos)));
		if ((c > 96) && (c < 123))
			c &= 223;
		else if (c == 47)
			c = 92;
		a = (a << 8 | c) ^ FPaTable[a >> 24];
		b = (b << 8 | c) ^ FPbTable[b >> 24];
	}
	return ((unsigned __int64)a & 0xFFFFFFFF) | ((unsigned __int64)b << 32);
}

unsigned __int32 intFN(unsigned __int32 i, const std::wstring& str) {
	int len = str.length();
	unsigned __int32 c;
	unsigned __int32 b;
	unsigned __int32 a = i;
	for (int pos = 0; pos < len; pos++) {
		c = (unsigned __int32)(static_cast<unsigned char>(str.at(pos)));
//		c = (unsigned __int32)str.c_str()[pos];
		b = c & 0xFF;
		a = (a << 8 | b) ^ FPaTable[a >> 24];
		b = c >> 8 & 0xFF;
		a = (a << 8 | b) ^ FPaTable[a >> 24];
	}
	return (unsigned __int32)a;
}

void stopWatchStart(float *pfPikoSeconds)
{
	if (pfPikoSeconds != NULL) {
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&First));
	}
}

void stopWatchStop(float *pfPikoSeconds)
{
	if (pfPikoSeconds != NULL) {
		ULONGLONG Last;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&Last));
		ULONG ulEclapsedCount = (ULONG)(Last - First);
		float fDiffSecs = float(ulEclapsedCount) / float(ulFreq);
		*pfPikoSeconds = fDiffSecs * MULTIPLIER_PIKOSECONDS; //Pikoseconds
	}
}

bool fileExists(const std::wstring& fileName)
{
	return std::filesystem::exists(std::filesystem::path(fileName));
}

bool isServerExe()
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
				TRACE(L"Error in IsServerExe: Couldn't get access to modded il2fb.exe server ID memory area!\r\n");
			}
		}
	}
	catch (...) {
		TRACE(L"Error in IsServerExe: Couldn't check modded il2fb.exe server ID!\r\n");
	}

	CloseHandle(hCurProc);
	return bIsServer;
}

bool directoryExists(const std::wstring& dirName_in)
{
	return std::filesystem::is_directory(std::filesystem::path(dirName_in));
}

std::wstring getCurrentExecutablePathName()
{
	int size = MAX_PATH;
	std::vector<wchar_t> charBuffer(size);
	while (GetModuleFileNameW(NULL, charBuffer.data(), size) == size) {
		size *= 2;
		charBuffer.resize(size);
	}
	return charBuffer.data();
}

std::wstring getDirectoryWithCurrentExecutable()
{
	std::experimental::filesystem::path path(getCurrentExecutablePathName());
	return path.remove_filename().wstring();
}

std::wstring trim(const std::wstring& str, std::wstring whitespace)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::wstring::npos)
		return L""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::wstring toLower(std::wstring data)
{
	std::transform(data.begin(), data.end(), data.begin(),
		[](wchar_t c) { return std::towlower(c); });
	return data;
}

std::wstring toUpper(std::wstring data)
{
	std::transform(data.begin(), data.end(), data.begin(),
		[](wchar_t c) { return std::towupper(c); });
	return data;
}

bool iequals(const std::wstring& a, const std::wstring& b)
{
	if (a.length() != b.length()) return false;
	return (_wcsnicmp(a.c_str(), b.c_str(), a.length()) == 0);
}

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
	if (!fileList.empty()) {
		listPos = binarySearchFileList(hash);

		if (listPos == -1) {
			wchar_t* format = L"%016I64X";
			int size = std::swprintf(nullptr, 0, format, hash);
			std::wstring buf(size + 1, '\0');
			std::swprintf(&buf[0], buf.size(), format, hash);
			hash2 = sfs_hashW(0, buf);
			listPos = binarySearchFileList(hash2);
			if (listPos != -1) foundIn = 2;
		}
		else {
			foundIn = 1;
		}

		//TRACE(L"__SFS_openf(%016I64X, %d) - hash2=%016I64X, listPos=%d\r\n", hash, flags, hash2, listPos);

		if (listPos != -1) {
			if (SFS_open != NULL)
				filePointer = SFS_open(const_cast<char*>(fileList[listPos]->filePath.c_str()), flags);
			else
				filePointer = SFS_open_cpp(const_cast<char*>(fileList[listPos]->filePath.c_str()), flags);
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
	//TRACE(L"__SAS_openf(%016I64X, %d)\r\n", hash, flags);
	return __SFS_openf(hash, flags);
}