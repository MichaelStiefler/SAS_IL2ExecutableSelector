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

#ifdef SASIL2WRAPPER_EXPORTS
#define SASIL2WRAPPER_API __declspec(dllexport)
#define SASIL2WRAPPER_C_API extern "C" __declspec(dllexport)
#else
#define SASIL2WRAPPER_API __declspec(dllimport)
#define SASIL2WRAPPER_C_API extern "C" __declspec(dllimport)
#endif

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <filesystem>
#include <cwctype>

#define lengthof(a) (sizeof a / sizeof a[0])

#define IL2FB_INI_FILE		L"il2fb.ini"
#define MODS_DEFAULT		L"MODS"
#define FILES_DEFAULT		L"FILES"
#define MODS_STOCK			L"none"
#define FILES_STOCK			L"none"
#define MODS_MODACT3		L"#SAS"
#define FILES_MODACT3		L"none"
#define MODS_UP3			L"#UP#"
#define FILES_UP3			L"none"
#define MODS_DBW			L"#DBW"
#define FILES_DBW			L"none"
#define MODS_DBW_1916		L"#DBW_1916"
#define FILES_DBW_1916		L"none"

#define IL2FB_INI_SECTION	L"Settings"
#define IL2FB_INI_MODTYPE	L"ModType"
#define IL2FB_INI_MODS		L"ModsFolder"
#define IL2FB_INI_FILES		L"FilesFolder"

typedef unsigned int __cdecl TSFS_open(char *filename, int flags);
typedef unsigned int __cdecl TSFS_openf(unsigned __int64 hash, int flags);
typedef unsigned int __cdecl TCalcCryptDump(int checkSecond2, BYTE* baseAddrData, int len);
typedef unsigned int __stdcall TSFS_open_cpp(char* filename, int flags);
typedef unsigned int __stdcall TSFS_openf_cpp(unsigned __int64 hash, int flags);

struct MyFileListItem {
	unsigned __int64 hash;
	std::string filePath;
	DWORD	dwIndex;
};

void listFiles(const std::wstring& Parent, const std::wstring& root, const std::wstring& addFront);
unsigned __int64 sfs_hashW(const unsigned __int64 hash, const std::wstring& str);
unsigned __int64 longFN(unsigned __int64 i64Long, const std::wstring& str);
unsigned __int32 intFN(unsigned __int32 i32Int, const std::wstring& str);
void startWrapper(HINSTANCE hInstance);
void stopWrapper();
void sortList(float * pfPikoSeconds);
void createModsFolderList(float * pfPikoSeconds);
void createFilesFolderList(float * pfPikoSeconds);
int removeDuplicates(float * pfPikoSeconds);
void getCommandLineParams();
void linkIl2fbExe();
void stopWatchStart(float *pfPikoSeconds);
void stopWatchStop(float *pfPikoSeconds);
bool fileExists(const std::wstring& lpcFilename);
bool isServerExe();
bool directoryExists(const std::wstring& dirName_in);
void threadAttach();
void threadDetach();
std::wstring getCurrentExecutablePathName();
std::wstring getDirectoryWithCurrentExecutable();
bool iequals(const std::wstring& a, const std::wstring& b);
std::wstring trim(const std::wstring& str, std::wstring whitespace = L" \t");
std::wstring toLower(std::wstring data);
std::wstring toUpper(std::wstring data);
