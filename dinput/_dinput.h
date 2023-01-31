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

#ifdef DINPUT_EXPORTS
#define DINPUT_API extern "C" __declspec(dllexport)
#else
#define DINPUT_API __declspec(dllimport)
#endif

//*************************************************************************
// Includes
//*************************************************************************
#include "jni.h"
#include "dinput.h"
#include <string>

//*************************************************************************
// Definitions
//*************************************************************************
#define PROC_FULL_RIGHTS DELETE|READ_CONTROL|SYNCHRONIZE|WRITE_DAC|WRITE_OWNER|PROCESS_TERMINATE|PROCESS_CREATE_THREAD|PROCESS_SET_SESSIONID|PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_DUP_HANDLE|PROCESS_CREATE_PROCESS|PROCESS_SET_QUOTA|PROCESS_SET_INFORMATION|PROCESS_QUERY_INFORMATION|PROCESS_SUSPEND_RESUME
#define CLIENT_INI			L"il2fb.ini"
#define SERVER_INI			L"il2server.ini"
#define CLIENT_CONF_INI		L"conf.ini"
#define LOGFILE_NAME		L"initlog.lst"
#define CLASSES_RUNTIME_FILE_NAME		L"%sclasses_runtime_%04d-%02d-%02d.csv"
#define CLASSES_SORTED_FILE_NAME		L"%sclasses_sorted_%04d-%02d-%02d.csv"
#define CLASSES_SUMMARY_FILE_NAME		L"%sclasses_summary_%04d-%02d-%02d.txt"
#define IL2GE_PATHS			{L"il2ge.dll", L"il2ge\\lib\\il2ge.dll", L"bin\\selector\\basefiles\\il2ge.dll"}
#define IL2GE_PATHS_NUM		3
#define	XSS_DIVIDER			128
#define PERM_DIVIDER		8

#define MEM_STRATEGY_BALANCED		0
#define MEM_STRATEGY_CONSERVATIVE	1
#define MEM_STRATEGY_HEAPONLY		2

#define SELECTOR_INFO_FILE_VERSION		1
#define SELECTOR_INFO_PRODUCT_VERSION	2
#define SELECTOR_INFO_FILEVERSION		3
#define SELECTOR_INFO_PRODUCTVERSION	4
#define SELECTOR_INFO_SPECIAL_BUILD		5
#define SELECTOR_INFO_COPYRIGHT			6

//*************************************************************************
// Function Prototypes
//*************************************************************************
void AdjustJvmParams();
void ReadSelectorSettings();
void GetParams();
BOOL IsServerExe();
BOOL StartWatchdog();
BOOL StartPipeLogger();
BOOL FileExists(LPCTSTR szPath);
void ReadConfSettings();
void _tcstrim(LPTSTR str);

JNIEXPORT jstring JNICALL Java_com_maddox_sas1946_il2_util_BaseGameVersion_getSelectorInfo(JNIEnv *, jobject, jint);
JNIEXPORT jboolean JNICALL Java_com_maddox_rts_SFSInputStream_isDumpMode(JNIEnv *, jobject);
JNIEXPORT jbyteArray JNICALL Java_com_maddox_rts_SFSInputStream_getStartupClass(JNIEnv*, jobject, jint);
JNIEXPORT void JNICALL Java_com_maddox_rts_SFSInputStream_freeStartupClasses(JNIEnv*, jobject);
JNIEXPORT void JNICALL Java_com_maddox_rts_SFSInputStream_println(JNIEnv*, jobject, jstring);
