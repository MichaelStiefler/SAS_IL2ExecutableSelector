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

#include <tchar.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
//#include "jni.h"


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

#define DUMP_FILE			L"crt\\RtDump.bin"



#define IL2FB_INI_SECTION	L"Settings"
#define IL2FB_INI_MODTYPE	L"ModType"
#define IL2FB_INI_MODS		L"ModsFolder"
#define IL2FB_INI_FILES		L"FilesFolder"

typedef unsigned int __cdecl TSFS_open(char *filename, int flags);
typedef unsigned int __cdecl TSFS_openf(unsigned __int64 hash, int flags);
typedef unsigned int __cdecl TCalcCryptDump(int checkSecond2, BYTE* baseAddrData, int len);
//typedef unsigned int __stdcall TSFS_read(int filePointer, LPVOID lpBuffer, DWORD* nNumberOfBytesToRead);
//typedef unsigned int __stdcall TSFS_lseek(int filePointer, LONG lDistanceToMove, int moveMethod);
typedef unsigned int __stdcall TSFS_open_cpp(char* filename, int flags);
typedef unsigned int __stdcall TSFS_openf_cpp(unsigned __int64 hash, int flags);

struct MyFileListItem {
	unsigned __int64 hash;
	LPSTR filePath;
	DWORD	dwIndex;
};

//#ifdef __cplusplus
//extern "C" {
//#endif
//	JNIEXPORT int JNICALL Java_com_maddox_il2_net_NetServerParams_readDump(JNIEnv *, jobject, jint);
//
//#ifdef __cplusplus
//}
//#endif

//SASIL2WRAPPER_C_API void __cdecl ReadDump(void *buf, unsigned len);
//SASIL2WRAPPER_C_API int __stdcall __SFS_openf(const unsigned __int64 hash, const int flags);
#ifdef USE_415_CODE
SASIL2WRAPPER_C_API int __stdcall __SFS_openf(const unsigned __int64 hash, const int flags);
#else
SASIL2WRAPPER_C_API int __cdecl __SFS_openf(const unsigned __int64 hash, const int flags);
#endif
void ListFiles(LPCTSTR lpParent, LPCTSTR lpRoot, LPCTSTR lpAddFront);
unsigned __int64 SFS_hashW(const unsigned __int64 hash, LPCTSTR buf, const int len);
unsigned __int64 SFS_hash(const unsigned __int64 hash, const void *buf, const int len);
unsigned __int64 LongFN(unsigned __int64 paramLong, LPCTSTR paramString);
unsigned __int32 IntFN(unsigned __int32 paramInt, LPCTSTR paramString);
void StartWrapper(HINSTANCE hInstance);
void StopWrapper();
void SortList(float * pfPikoSeconds);
void CreateModsFolderList(float * pfPikoSeconds);
bool CreateCachedModsFolderList(float * pfPikoSeconds);
void CreateFilesFolderList(float * pfPikoSeconds);
bool CreateCachedFilesFolderList(float * pfPikoSeconds);
bool ReadCachedFileList(LPCTSTR pCachedFileListName);
int RemoveDuplicates(float * pfPikoSeconds);
void GetCommandLineParams();
void LinkIl2fbExe();
void StopWatchStart(float *pfPikoSeconds);
void StopWatchStop(float *pfPikoSeconds);
BOOL FileExists(LPCTSTR lpcFilename);
bool IsServerExe();
bool DirectoryExists(LPCTSTR dirName_in);
void ThreadAttach();
void ThreadDetach();
TCHAR * trim(TCHAR * c, TCHAR*space);
