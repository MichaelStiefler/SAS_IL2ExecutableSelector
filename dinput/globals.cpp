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

#include "globals.h"

// Suppress compiler warnings about unsafe functions
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

//*************************************************************************
// Globals
//*************************************************************************
BOOL bDisableMutex = FALSE;
BOOL g_bDumpFiles = FALSE;
BOOL g_bDumpSFSAccess = FALSE;
BOOL g_bInitialized = FALSE;
BOOL g_bInstantDump = FALSE;
BOOL g_bIsOpenGL = FALSE;
BOOL g_bProcessAttached = FALSE;
BOOL g_bEnablePipeLogger = TRUE;
BOOL g_bIs415 = FALSE;
BOOL g_bIsJava8 = FALSE;
int g_bCpuInfo = 0;
int g_iModType = 0;
BOOL isMain = TRUE;
BYTE g_oldMem[9];
CHAR cBuffer[MAX_PATH];
CHAR cBuf[MAX_PATH];
char cIniFile[MAX_PATH];
CHAR g_cDumpSeparator = ' ';
const TCHAR *	c_szMsgWndClass = _T("Il2SASWatchdogMsgWnd");
DIRECTINPUTCREATEA		SysDirectInputCreateA = NULL;
DIRECTINPUTCREATEEX		SysDirectInputCreateEx = NULL;
DIRECTINPUTCREATEW		SysDirectInputCreateW = NULL;
DLLREGISTERSERVER		SysDllRegisterServer = NULL;
DLLUNREGISTERSERVER		SysDllUnregisterServer = NULL;
HANDLE g_hJobObject = NULL;
HMODULE g_hIL2GE = NULL;
HMODULE hDInput = NULL;
HMODULE hDInputSelf = NULL;
HMODULE hImm32 = NULL;
HMODULE hJvm = NULL;
IL2GE_Init				Il2geInit = NULL;
IMMDISABLEIME			SysImmDisableIme = NULL;
int g_iDebugMode = 0;
int g_iSplashScreenMode = 0;
int g_isServer = -1;
JVM_CreateJavaVM		JniCreateJavaVM = NULL;
JVM_DefineClass		    JniDefineClass = NULL;
IL2_SFS_mount           Il2fbSFS_mount = NULL;
IL2_SFS_mount_cpp       Il2fbSFS_mount_cpp = NULL;
DT_GetCpuID             GetCpuID = NULL;
DT_PrintCpuInfo         PrintCpuInfo = NULL;
std::condition_variable cond_var;
std::mutex m;
std::set<std::string, jvm_options_compare> JvmOptions;
TCHAR LogFileName[MAX_PATH];
TCHAR szBuffer[MAX_PATH];
TCHAR szBuffer2[MAX_PATH];
TCHAR szConfIniFile[MAX_PATH];
TCHAR szCurDir[MAX_PATH];
CHAR cCurDir[MAX_PATH];
TCHAR szDirectInputPath[MAX_PATH];
TCHAR szImm32Path[MAX_PATH];
TCHAR szIniFile[MAX_PATH];
TCHAR szJvmPath[MAX_PATH];
char g_cFilesSFS[MAX_PATH];
TCHAR g_szSplashImage[MAX_PATH];
int g_iLoadedClasses = 0;
jbyteArray g_bStartupClasses[5];