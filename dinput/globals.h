//*****************************************************************
// DINPUT.dll - JVM Parameter parser and il2fb.exe modifier
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of DINPUT.dll.
//
// DINPUT.dll is free software.
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

#include "StdAfx.h"
#include <set>
#include <string>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include "jni.h"
#include "dinput.h"

// Suppress compiler warnings about unsafe functions
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

#define RunWorker(ThreadStartRoutine) 	CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadStartRoutine,NULL,0,NULL))

//*************************************************************************
// Type definitions
//*************************************************************************
typedef HRESULT(WINAPI * DIRECTINPUTCREATEA)(HINSTANCE, DWORD, LPDIRECTINPUTA *, LPUNKNOWN);
typedef HRESULT(WINAPI * DIRECTINPUTCREATEW)(HINSTANCE, DWORD, LPDIRECTINPUTW *, LPUNKNOWN);
typedef HRESULT(WINAPI * DIRECTINPUTCREATEEX)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
typedef HRESULT(WINAPI * DLLREGISTERSERVER)();
typedef HRESULT(WINAPI * DLLUNREGISTERSERVER)();
typedef BOOL(WINAPI * IMMDISABLEIME)(__in  DWORD);
typedef jint(JNICALL* JVM_CreateJavaVM)(JavaVM **p_vm, void **p_env, void *vm_args);
typedef jclass(JNICALL* JVM_DefineClass)(JNIEnv * env, const char * name, jobject loader, const jbyte * buf, jsize bufLen);
typedef void(WINAPI * IL2GE_Init)();
typedef int(__cdecl * IL2_SFS_mount)(const char *sfs, int i, char c);
//typedef int(__cdecl* IL2_SFS_mount)(const char* sfs, int i, int j);
typedef int(__stdcall* IL2_SFS_mount_cpp)(const char* sfs, int i, int j);
typedef int(__stdcall* DT_GetCpuID)();
typedef int(__stdcall* DT_PrintCpuInfo)();

struct jvm_options_compare {
	bool operator() (const std::string& lhs, const std::string& rhs) const {
		return stricmp(lhs.c_str(), rhs.c_str()) < 0;
	}
};

//*************************************************************************
// Globals
//*************************************************************************
extern BOOL bDisableMutex;
extern BOOL g_bDumpFiles;
extern BOOL g_bDumpSFSAccess;
extern BOOL g_bInstantDump;
extern BOOL g_bIsOpenGL;
extern BOOL g_bProcessAttached;
extern BOOL g_bEnablePipeLogger;
extern BOOL g_bIs415;
extern BOOL g_bIsJava8;
extern int g_bCpuInfo;
extern int g_iModType;
extern BOOL isMain;
extern BYTE g_oldMem[9];
extern CHAR cBuffer[MAX_PATH];
extern CHAR cBuf[MAX_PATH];
extern char cIniFile[MAX_PATH];
extern CHAR g_cDumpSeparator;
extern const TCHAR *	c_szMsgWndClass;
extern DIRECTINPUTCREATEA		SysDirectInputCreateA;
extern DIRECTINPUTCREATEEX		SysDirectInputCreateEx;
extern DIRECTINPUTCREATEW		SysDirectInputCreateW;
extern DLLREGISTERSERVER		SysDllRegisterServer;
extern DLLUNREGISTERSERVER		SysDllUnregisterServer;
extern HANDLE g_hJobObject;
extern HMODULE g_hIL2GE;
extern HMODULE hDInput;
extern HMODULE hDInputSelf;
extern HMODULE hImm32;
extern HMODULE hJvm;
extern IL2GE_Init				Il2geInit;
extern IMMDISABLEIME			SysImmDisableIme;
extern int g_iDebugMode;
extern int g_iSplashScreenMode;
extern int g_isServer;
extern JVM_CreateJavaVM		JniCreateJavaVM;
extern JVM_DefineClass		    JniDefineClass;
extern IL2_SFS_mount           Il2fbSFS_mount;
extern IL2_SFS_mount_cpp       Il2fbSFS_mount_cpp;
extern DT_GetCpuID       GetCpuID;
extern DT_PrintCpuInfo       PrintCpuInfo;
extern std::condition_variable cond_var;
extern std::mutex m;
extern std::set<std::string, jvm_options_compare> JvmOptions;
extern TCHAR LogFileName[MAX_PATH];
extern TCHAR szBuffer[MAX_PATH];
extern TCHAR szBuffer2[MAX_PATH];
extern TCHAR szConfIniFile[MAX_PATH];
extern TCHAR szCurDir[MAX_PATH];
extern CHAR cCurDir[MAX_PATH];
extern TCHAR szDirectInputPath[MAX_PATH];
extern TCHAR szImm32Path[MAX_PATH];
extern TCHAR szIniFile[MAX_PATH];
extern TCHAR szJvmPath[MAX_PATH];
extern char g_cFilesSFS[MAX_PATH];
extern TCHAR g_szSplashImage[MAX_PATH];
extern int g_iLoadedClasses;
extern jbyteArray g_bStartupClasses[5];