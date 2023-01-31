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

//*************************************************************************
// Definitions
//*************************************************************************
#define _IMM_ // Define _IMM_ in order to avoid forward declaration of ImmDisableIME() in Imm.h

//*************************************************************************
// Includes
//*************************************************************************
#include "_dinput.h"
#include <tchar.h>
#include <vector>
#include <set>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <psapi.h>
#include "jni.h"
#include "trace.h"
#include "SFS_Tools.h"
#include "globals.h"
#include "../global/version.h"
#include "../detours/include/detours.h"
#include "resource.h"
#include "../FileCrypter/Output/resource_files.h"

#pragma comment(lib, "../detours/lib.X86/detours.lib")
#pragma comment(lib, "psapi.lib")

// Suppress compiler warnings about unsafe functions
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )
#pragma warning( disable : 4099 )

const unsigned char* initClasses[5] = { LDRCallBack, LDR, Finger, SFSInputStream, RTS };
const size_t initClassesSize[5] = { LDRCallBack_size, LDR_size, Finger_size, SFSInputStream_size, RTS_size };

//************************************
// Method:    DirectInputCreateA
// FullName:  DirectInputCreateA
// Access:    public
// Returns:   DINPUT_API HRESULT __stdcall
// Qualifier:
// Parameter: HINSTANCE hinst
// Parameter: DWORD dwVersion
// Parameter: LPDIRECTINPUTA * ppDI
// Parameter: LPUNKNOWN punkOuter
//************************************
DINPUT_API HRESULT __stdcall DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* ppDI, LPUNKNOWN punkOuter)
{
	// forward any DirectInputCreateA function call to windows native dinput.dll
	if (hDInput == NULL) {
		LPCTSTR lpEnvPath = _wgetenv(L"SystemRoot");
		TCHAR szDirectInputPath[MAX_PATH];
		_stprintf(szDirectInputPath, L"%s\\system32\\dinput.dll", lpEnvPath);
		hDInput = LoadLibraryW(szDirectInputPath);
	}
	if (hDInput == NULL) {
		return E_NOINTERFACE;
	}

	if (SysDirectInputCreateA == NULL) {
		SysDirectInputCreateA = (DIRECTINPUTCREATEA)GetProcAddress(hDInput, "DirectInputCreateA");
	}

	return SysDirectInputCreateA(hinst, dwVersion, ppDI, punkOuter);
}

//************************************
// Method:    DirectInputCreateEx
// FullName:  DirectInputCreateEx
// Access:    public
// Returns:   DINPUT_API HRESULT __stdcall
// Qualifier:
// Parameter: HINSTANCE hinst
// Parameter: DWORD dwVersion
// Parameter: REFIID refIID
// Parameter: LPVOID * ppDI
// Parameter: LPUNKNOWN punkOuter
//************************************
DINPUT_API HRESULT __stdcall DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID refIID, LPVOID* ppDI, LPUNKNOWN punkOuter)
{
	// forward any DirectInputCreateEx function call to windows native dinput.dll
	if (hDInput == NULL) {
		LPCTSTR lpEnvPath = _wgetenv(L"SystemRoot");
		TCHAR szDirectInputPath[MAX_PATH];
		_stprintf(szDirectInputPath, L"%s\\system32\\dinput.dll", lpEnvPath);
		hDInput = LoadLibraryW(szDirectInputPath);
	}
	if (hDInput == NULL) {
		return E_NOINTERFACE;
	}

	if (SysDirectInputCreateEx == NULL) {
		SysDirectInputCreateEx = (DIRECTINPUTCREATEEX)GetProcAddress(hDInput, "DirectInputCreateEx");
	}

	return SysDirectInputCreateEx(hinst, dwVersion, refIID, ppDI, punkOuter);
}

//************************************
// Method:    DirectInputCreateW
// FullName:  DirectInputCreateW
// Access:    public
// Returns:   DINPUT_API HRESULT __stdcall
// Qualifier:
// Parameter: HINSTANCE hinst
// Parameter: DWORD dwVersion
// Parameter: LPDIRECTINPUTW * ppDI
// Parameter: LPUNKNOWN punkOuter
//************************************
DINPUT_API HRESULT __stdcall DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW* ppDI, LPUNKNOWN punkOuter)
{
	// forward any DirectInputCreateW function call to windows native dinput.dll
	if (hDInput == NULL) {
		LPCTSTR lpEnvPath = _wgetenv(L"SystemRoot");
		TCHAR szDirectInputPath[MAX_PATH];
		_stprintf(szDirectInputPath, L"%s\\system32\\dinput.dll", lpEnvPath);
		hDInput = LoadLibraryW(szDirectInputPath);
	}
	if (hDInput == NULL) {
		return E_NOINTERFACE;
	}

	if (SysDirectInputCreateW == NULL) {
		SysDirectInputCreateW = (DIRECTINPUTCREATEW)GetProcAddress(hDInput, "DirectInputCreateW");
	}

	return SysDirectInputCreateW(hinst, dwVersion, ppDI, punkOuter);
}

//************************************
// Method:    ImmDisableIME
// FullName:  ImmDisableIME
// Access:    public
// Returns:   DINPUT_API BOOL __stdcall
// Qualifier:
// Parameter: __in DWORD idThread
//************************************
DINPUT_API BOOL __stdcall ImmDisableIME(__in  DWORD idThread)
{
	// forward any ImmDisableIME function call to windows native imm32.dll
	if (hImm32 == NULL) {
		LPCTSTR lpEnvPath = _wgetenv(L"SystemRoot");
		TCHAR szImm32Path[MAX_PATH];
		_stprintf(szImm32Path, L"%s\\system32\\imm32.dll", lpEnvPath);
		hImm32 = LoadLibraryW(szImm32Path);
	}
	if (hImm32 == NULL) {
		return E_NOINTERFACE;
	}

	if (SysImmDisableIme == NULL) {
		SysImmDisableIme = (IMMDISABLEIME)GetProcAddress(hImm32, "ImmDisableIME");
	}

	return SysImmDisableIme(idThread);
}

//************************************
// Method:    DllRegisterServer
// FullName:  DllRegisterServer
// Access:    public
// Returns:   DINPUT_API HRESULT __stdcall
// Qualifier:
// Parameter: void
//************************************
DINPUT_API HRESULT __stdcall DllRegisterServer(void)
{
	// forward any DllRegisterServer function call to windows native dinput.dll
	if (hDInput == NULL) {
		LPCTSTR lpEnvPath = _wgetenv(L"SystemRoot");
		TCHAR szDirectInputPath[MAX_PATH];
		_stprintf(szDirectInputPath, L"%s\\system32\\dinput.dll", lpEnvPath);
		hDInput = LoadLibraryW(szDirectInputPath);
	}
	if (hDInput == NULL) {
		return E_NOINTERFACE;
	}

	if (SysDllRegisterServer == NULL) {
		SysDllRegisterServer = (DLLREGISTERSERVER)GetProcAddress(hDInput, "DllRegisterServer");
	}

	return SysDllRegisterServer();
}

//************************************
// Method:    DllUnregisterServer
// FullName:  DllUnregisterServer
// Access:    public
// Returns:   DINPUT_API HRESULT __stdcall
// Qualifier:
// Parameter: void
//************************************
DINPUT_API HRESULT __stdcall DllUnregisterServer(void)
{
	// forward any DllUnregisterServer function call to windows native dinput.dll
	if (hDInput == NULL) {
		LPCTSTR lpEnvPath = _wgetenv(L"SystemRoot");
		TCHAR szDirectInputPath[MAX_PATH];
		_stprintf(szDirectInputPath, L"%s\\system32\\dinput.dll", lpEnvPath);
		hDInput = LoadLibraryW(szDirectInputPath);
	}
	if (hDInput == NULL) {
		return E_NOINTERFACE;
	}

	if (SysDllUnregisterServer == NULL) {
		SysDllUnregisterServer = (DLLUNREGISTERSERVER)GetProcAddress(hDInput, "DllUnregisterServer");
	}

	return SysDllUnregisterServer();
}

//************************************
// Method:    SAS_DefineClass
// FullName:  SAS_DefineClass
// Access:    public
// Returns:   jclass __stdcall
// Qualifier:
// Parameter: JNIEnv* env
// Parameter: const char* name
// Parameter: jobject loader
// Parameter: const jbyte* buf
// Parameter: jsize len
//************************************
jclass __stdcall SAS_DefineClass(JNIEnv* env, const char* name, jobject loader, const jbyte* buf, jsize len) {
	jclass result;

	//if (g_iLoadedClasses >= -1000) {
	//	result = JniDefineClass(env, name, loader, buf, len);
	//	++g_iLoadedClasses;
	//	return result;
	//}

	// First five classes are to be loaded from embedded resources, afterwards we proceed with normal class loading behaviour
	if (g_iLoadedClasses >= 5)
	{
		result = JniDefineClass(env, name, loader, buf, len);
		++g_iLoadedClasses;
	}
	else
	{
		// These are the first five classes that get loaded.
		// It's essential to use the resource-embedded classes shipped with IL-2 Selector here.
		g_bStartupClasses[g_iLoadedClasses] = env->NewByteArray(len);
		env->SetByteArrayRegion(g_bStartupClasses[g_iLoadedClasses], 0, len, buf);
		//HRSRC myResource = ::FindResource(hDInputSelf, MAKEINTRESOURCE(101 + g_iLoadedClasses), RT_RCDATA);
		//unsigned int myResourceSize = ::SizeofResource(hDInputSelf, myResource);
		//HGLOBAL myResourceData = ::LoadResource(hDInputSelf, myResource);
		//jbyte* pMyBinaryData = (jbyte*)::LockResource(myResourceData);
		//result = JniDefineClass(env, name, loader, pMyBinaryData, myResourceSize);

		unsigned char* data = (unsigned char*)malloc(initClassesSize[g_iLoadedClasses]);
		CopyMemory(data, initClasses[g_iLoadedClasses], initClassesSize[g_iLoadedClasses]);
		size_t saltPos = 0;
		size_t saltLen = strlen(G_FILE_VERSION);
		for (size_t i = 0; i < initClassesSize[g_iLoadedClasses]; i++) {
			unsigned char* x = data + i;
			unsigned char salt = G_FILE_VERSION[saltPos++];
			if (saltPos >= saltLen) saltPos = 0;
			*x ^= salt;
		}
		jbyte* pMyBinaryData = (jbyte*)data;
		result = JniDefineClass(env, name, loader, pMyBinaryData, initClassesSize[g_iLoadedClasses]);
		++g_iLoadedClasses;
	}

	return result;
}


//const unsigned char* initClasses[5] = { LDRCallBack, LDR, Finger, SFSInputStream, RTS };
//const size_t initClassesSize[5] = { LDRCallBack_size, LDR_size, Finger_size, SFSInputStream_size, RTS_size };

//************************************
// Method:    isDumpMode
// FullName:  Java_com_maddox_rts_SFSInputStream_isDumpMode
// Access:    public
// Returns:   JNIEXPORT jboolean JNICALL
// Qualifier:
// Parameter: JNIEnv* env
// Parameter: jobject thisObject
//************************************
JNIEXPORT jboolean JNICALL Java_com_maddox_rts_SFSInputStream_isDumpMode(JNIEnv* env, jobject thisObject)
{
	return g_bDumpFiles?JNI_TRUE:JNI_FALSE;
}

//************************************
// Method:    getStartupClass
// FullName:  Java_com_maddox_rts_SFSInputStream_getStartupClass
// Access:    public
// Returns:   JNIEXPORT jbyteArray JNICALL
// Qualifier:
// Parameter: JNIEnv* env
// Parameter: jobject thisObject
// Parameter: jint index
//************************************
JNIEXPORT jbyteArray JNICALL Java_com_maddox_rts_SFSInputStream_getStartupClass(JNIEnv* env, jobject thisObject, jint index)
{
	return g_bStartupClasses[index];
}

//************************************
// Method:    freeStartupClasses
// FullName:  Java_com_maddox_rts_SFSInputStream_freeStartupClasses
// Access:    public
// Returns:   JNIEXPORT void JNICALL
// Qualifier:
// Parameter: JNIEnv* env
// Parameter: jobject thisObject
//************************************
JNIEXPORT void JNICALL Java_com_maddox_rts_SFSInputStream_freeStartupClasses(JNIEnv* env, jobject thisObject)
{
//	if (g_bDumpFiles) for (int i = 0; i < 5; i++)  env->DeleteLocalRef(g_bStartupClasses[i]);
	return;
}

//************************************
// Method:    println
// FullName:  Java_com_maddox_rts_SFSInputStream_println
// Access:    public
// Returns:   JNIEXPORT void JNICALL
// Qualifier:
// Parameter: JNIEnv* env
// Parameter: jobject thisObject
// Parameter: jstring line
//************************************
JNIEXPORT void JNICALL Java_com_maddox_rts_SFSInputStream_println(JNIEnv* env, jobject thisObject, jstring line)
{
	const char* nativeString = env->GetStringUTFChars(line, 0);
	printf(nativeString);
	printf("\r\n");
	env->ReleaseStringUTFChars(line, nativeString);
}

//************************************
// Method:    SAS_CreateJavaVM
// FullName:  SAS_CreateJavaVM
// Access:    public
// Returns:   jint JNICALL
// Qualifier:
// Parameter: JavaVM * * p_vm
// Parameter: void * * p_env
// Parameter: void * vm_args
//************************************
jint JNICALL SAS_CreateJavaVM(JavaVM** p_vm, void** p_env, void* vm_args)
{
	// This function is being called when the JMP command has been successfully
	// injected into JVM.dll and an application calls JNI_CreateJavaVM().
	TRACE("Hooked \"SAS_CreateJavaVM\" function activated\r\n");
	if (g_hIL2GE == NULL && !IsServerExe()) {
		if (g_bIsOpenGL) {
			TCHAR szIL2GEFile[MAX_PATH];
			TRACE("OpenGL Mode detected, checking IL-2 Graphics Extender Availability...\r\n");
			LPCTSTR il2gepaths[] = IL2GE_PATHS;
			for (int i = 0; i < IL2GE_PATHS_NUM; i++) {
				ZeroMemory(szIL2GEFile, sizeof(szIL2GEFile));
				_tcscpy(szIL2GEFile, szCurDir);
				_tcscat(szIL2GEFile, il2gepaths[i]);
				TRACE(L"Trying %s\r\n", szIL2GEFile);
				if (FileExists(szIL2GEFile)) {
					TRACE(L"il2ge.dll found, loading library.\r\n", szIL2GEFile);
					g_hIL2GE = LoadLibrary(szIL2GEFile);
					if (g_hIL2GE != NULL) {
						TRACE("IL-2 Graphics Extender loaded successfully, trying to call Init() method...\r\n");
						Il2geInit = (IL2GE_Init)GetProcAddress(g_hIL2GE, "Init");
						if (Il2geInit == NULL) {
							TRACE("No Init() method available in IL-2 Graphics Extender.\r\n");
							break;
						}
						TRACE("Init() method found, calling it now.\r\n");
						Il2geInit();
						break;
					}
				}
			}
			if (g_hIL2GE == NULL) {
				TRACE("IL-2 Graphics Extender not available.\r\n");
				g_hIL2GE = (HMODULE)INVALID_HANDLE_VALUE;
			}
		}
		else {
			TRACE("DirectX Mode detected, skipping IL-2 Graphics Extender routines.\r\n");
		}
	}
	TRACE("Injecting JVM Parameters\r\n");

	// get access to JVM arguments being passed to the JNI_CreateJavaVM() function.
	JavaVMInitArgs* vmargs = (JavaVMInitArgs*)vm_args;
	// create a new set of  JVM arguments which holds our modified JVM parameters
	JavaVMInitArgs newArgs;
	// newArgs.ignoreUnrecognized = JNI_FALSE; // never ignore unknown JVM options and parameters
	newArgs.ignoreUnrecognized = JNI_TRUE; // never ignore unknown JVM options and parameters
	newArgs.version = vmargs->version;
	// allocate memory for our new JVM parameters
	JavaVMOption* newOptions = (JavaVMOption*)malloc((JvmOptions.size()) * sizeof(JavaVMOption));
	ZeroMemory(newOptions, sizeof(newOptions));

	// fill new JVM arguments array.
	try {
		int iCurOption = 0;

//		newOptions[iCurOption++].optionString = "-XX:+UnlockDiagnosticVMOptions";

		for (const std::string& option : JvmOptions) {
			newOptions[iCurOption++].optionString = (char*)option.c_str();
			TRACE("Final JVM Option: %s\r\n", option.c_str());
		}

		newArgs.options = newOptions;
		newArgs.nOptions = iCurOption;
	}
	catch (...) {
		TRACE("Error in SAS_CreateJavaVM while generating new JavaVMInitArgs!\r\n");
		return JNI_ERR;
	}

	if (JniCreateJavaVM == NULL) {
		TRACE("Error in SAS_CreateJavaVM: Couldn't find original JNI_CreateJavaVM entry point!\r\n");
		return JNI_ERR;
	}

	// call back into original function JNI_CreateJavaVM() (with restored header)
	jint jRet = JniCreateJavaVM(p_vm, p_env, &newArgs);

	// Modify JVM's function table and re-route the "defineClass" method
	// Note that this is not the JVM's exported "defineClass" method address, but rather
	// the method pointer table entry in the JVM's Environment header.
	// This ensure that our hook gets called before any "legal" hook and be processed.

//	if (!IsServerExe()) {
		jclass(JNICALL * oldDefineClassAddress)(JNIEnv*, const char*, jobject, const jbyte*, jsize);
		DWORD flOldProtect;

		int jvmEnvironmentAddress = **((int**)p_env);
		DWORD dwCurProcId = GetCurrentProcessId();
		HANDLE hCurProc = OpenProcess(0x1F0FFFu, 0, dwCurProcId);
		VirtualProtectEx(hCurProc, (LPVOID)jvmEnvironmentAddress, 0x18u, 0x40u, &flOldProtect);
		oldDefineClassAddress = *(jclass(JNICALL**)(JNIEnv*, const char*, jobject, const jbyte*, jsize))(jvmEnvironmentAddress + 20);
		*(DWORD*)(jvmEnvironmentAddress + 20) = (DWORD)SAS_DefineClass;
		JniDefineClass = oldDefineClassAddress;
		VirtualProtectEx(hCurProc, (LPVOID)jvmEnvironmentAddress, 0x18u, flOldProtect, &flOldProtect);
		CloseHandle(hCurProc);
//	}

	// ---

	if (jRet == JNI_OK) {
		TRACE("Java Virtual Machine Initialization with additional parameters successful!\r\n");
		if (IsServerExe()) {
			printf("#\r\n");
			printf("# Java VM successfully initialized with addon parameters!\r\n");
			printf("# JVM is up and running, starting IL-2 Server now...\r\n");
			printf("#\r\n");
			printf("#########################################################\r\n");
		}
	}
	else {
		TRACE("Error in SAS_CreateJavaVM: JNI_CreateJavaVM return code = %d\r\n", jRet);
	}
	free(newOptions);
	return jRet;
}

//************************************
// Method:    SAS_SFS_mount
// FullName:  SAS_SFS_mount
// Access:    public
// Returns:   int __cdecl
// Qualifier:
// Parameter: const char* sfs
// Parameter: int i
// Parameter: char c
//************************************
//int __cdecl SAS_SFS_mount(const char* sfs, int i, char c) {
int __cdecl SAS_SFS_mount(const char* sfs, int i, int j) {
		// In case we are being asked to mount files.sfs, check whether il2fb.ini references an alternative files.sfs instead and if so, mount that one.
	if ((IsServerExe() && stricmp(sfs, "filesserver.sfs") == 0) || stricmp(sfs, "files.sfs") == 0) {
		int retVal = 0;
		char* pch = strtok(g_cFilesSFS, ",");
		while (pch != NULL) {
			retVal = (g_bIs415 || IsServerExe())?Il2fbSFS_mount_cpp(pch, i, j):Il2fbSFS_mount(pch, i, j);
			if (g_bDumpSFSAccess) TRACE("SFS_mount(%s, %d, %d) = %d\r\n", pch, i, j, retVal);
			pch = strtok(NULL, ",");
		}
		return retVal;
	}
	int retVal = g_bIs415?Il2fbSFS_mount_cpp(sfs, i, j):Il2fbSFS_mount(sfs, i, j);
	if (g_bDumpSFSAccess) TRACE("SFS_mount(%s, %d, %d) = %d\r\n", sfs, i, j, retVal);
	return retVal;
}

//************************************
// Method:    ApplyMutexSetting
// FullName:  ApplyMutexSetting
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void ApplyMutexSetting()
{
	// This function is called by modified entry point header from Stock il2fb.exe
	// This is necessary since stock il2fb.exe is UPX-compressed, so we have to wait
	// until the executable has been decompressed in memory and step in before it's
	// being executed then.
	DWORD dwOldProtect = 0;
	DWORD dwNumRead = 0;
	DWORD dwProcRights = PROC_FULL_RIGHTS;
	DWORD dwCurProc = GetCurrentProcessId();
	HANDLE hCurProc = OpenProcess(dwProcRights, FALSE, dwCurProc);

	if (hCurProc != NULL) {
		int iIl2fbMutex = 0x00422068;

		try {
			if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2fbMutex, 15, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
				ZeroMemory((LPVOID)iIl2fbMutex, 15); // disable Mutex
				VirtualProtectEx(hCurProc, (LPVOID)iIl2fbMutex, 9, dwOldProtect, NULL);
			}
			else {
				TRACE("Error in ApplyMutexSetting: Couldn't get access to mutex string memory area!\r\n");
			}
		}
		catch (...) {
			TRACE("Error in ApplyMutexSetting while overwriting mutex string!\r\n");
		}

		CloseHandle(hCurProc);
	}
	else {
		TRACE("Error in ApplyMutexSetting: Couldn't aquire current process handle!\r\n");
	}

	__asm { // jump back to original entry point at process address 0x4128E9
		mov eax, 0x004128E9
		jmp eax
	}
	return;
}

//************************************
// Method:    IsServerExe
// FullName:  IsServerExe
// Access:    public
// Returns:   BOOL
// Qualifier:
//************************************
BOOL IsServerExe()
{
	if (g_isServer == 0)
		return FALSE;
	else if (g_isServer == 1)
		return TRUE;
	// Check if the process executable is running IL-2 Server or Client
	DWORD dwOldProtect = 0;
	DWORD dwNumRead = 0;
	DWORD dwProcRights = DELETE | READ_CONTROL | SYNCHRONIZE | WRITE_DAC | WRITE_OWNER
		| PROCESS_TERMINATE | PROCESS_CREATE_THREAD | PROCESS_SET_SESSIONID | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_DUP_HANDLE | PROCESS_CREATE_PROCESS | PROCESS_SET_QUOTA | PROCESS_SET_INFORMATION | PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME;
	DWORD dwCurProc = GetCurrentProcessId();
	HANDLE hCurProc = OpenProcess(dwProcRights, FALSE, dwCurProc);
	BOOL bIsServer = FALSE;
	g_isServer = 0;

	try {
		int iIl2ServerID = 0x0041EBA8;
		char* cFilesServer = (char*)"filesserver.sfs"; // this file reference is present in IL-2 Server executable only.

		if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2ServerID, 15, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
			if (memcmp((LPVOID)iIl2ServerID, cFilesServer, 15) == 0) {
				bIsServer = TRUE;
				g_isServer = 1;
			}

			VirtualProtectEx(hCurProc, (LPVOID)iIl2ServerID, 9, dwOldProtect, NULL);
		}
		if (!bIsServer) {
			int iIl2ServerID = 0x00408d8d; // il2server.exe 4.15
			if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2ServerID, 15, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
				if (memcmp((LPVOID)iIl2ServerID, cFilesServer, 15) == 0) {
					bIsServer = TRUE;
					g_isServer = 1;
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

//************************************
// Method:    file_exists
// FullName:  file_exists
// Access:    public
// Returns:   BOOL
// Qualifier:
//************************************
BOOL file_exists(TCHAR* filePath)
{
	//This will get the file attributes bitlist of the file
	DWORD fileAtt = GetFileAttributes(filePath);

	//If an error occurred it will equal to INVALID_FILE_ATTRIBUTES
	if (fileAtt == INVALID_FILE_ATTRIBUTES)
		//So lets throw an exception when an error has occurred
		//throw GetLastError();
		return FALSE;

	//If the path referers to a directory it should also not exists.
	return ((fileAtt & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

//************************************
// Method:    AddJvmOption
// FullName:  AddJvmOption
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: LPCSTR lpOptionString
//************************************
void AddJvmOption(LPCSTR lpOptionString)
{
	std::string newOption(lpOptionString);
	std::pair optionInsert = JvmOptions.emplace(newOption);
	if (!optionInsert.second) {
		JvmOptions.erase(optionInsert.first);
		JvmOptions.emplace(newOption);
	}

	TRACE("Added JVM Option: %s\r\n", newOption.c_str());
}

//************************************
// Method:    IsInJvmOptions
// FullName:  IsInJvmOptions
// Access:    public
// Returns:   int
// Qualifier:
// Parameter: LPCSTR lpSearch
// Parameter: unsigned int iStartIndex
//************************************
bool IsInJvmOptions(LPCSTR lpSearch)
{
	std::string searchString(lpSearch);
	std::transform(searchString.begin(), searchString.end(), searchString.begin(), ::tolower);
	size_t searchStringLen = searchString.length();
	for (const std::string& jvmOption : JvmOptions) {
		std::string jvmOptionLower(jvmOption);
		std::transform(jvmOptionLower.begin(), jvmOptionLower.end(), jvmOptionLower.begin(), ::tolower);
		if (searchStringLen > jvmOption.length()) continue;
		if (jvmOptionLower._Starts_with(searchString)) {
			return true;
		}
	}
	return false;
}

//************************************
// Method:    AdjustJvmParams
// FullName:  AdjustJvmParams
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void AdjustJvmParams()
{
	// This function is called when our DINPUT.dll is getting linked.
	// The process of IL-2 exists at this time so we can hijack a set of memory
	// areas and inject code, overwrite memory areas or shift pointers around
	GetParams();

	if (!IsServerExe()) {
		Sleep(0);
		if (!StartWatchdog()) {
			TRACE("Watchdog Start failed.");
		}
	}
	if (g_bEnablePipeLogger) {
		Sleep(0);
		StartPipeLogger();
	}

	// Get original function pointer of Java VM's JNI_CreateJavaVM() function.
	if (hJvm == NULL) {
		TCHAR szJvmPath[MAX_PATH];
//		_stprintf(szJvmPath, L"%sbin\\hotspot\\jvm.dll", szCurDir);
		g_bIsJava8 = TRUE;
		_stprintf(szJvmPath, L"%sbin\\client\\jvm.dll", szCurDir);
		if (!file_exists(szJvmPath)) {
			g_bIsJava8 = FALSE;
			_stprintf(szJvmPath, L"%sbin\\hotspot\\jvm.dll", szCurDir);
		}
		hJvm = LoadLibraryW(szJvmPath);
	}
	JniCreateJavaVM = (JVM_CreateJavaVM)GetProcAddress(hJvm, "JNI_CreateJavaVM");

	if (JniCreateJavaVM == NULL) {
		TRACE("Error in AdjustJvmParams: Couldn't find original JNI_CreateJavaVM entry point!\r\n");
		return;
	}

	TCHAR szDtDllPath[MAX_PATH];
	_stprintf(szDtDllPath, L"%sDT.dll", szCurDir);
	BOOL dtDllExists = file_exists(szDtDllPath);

	if (IsServerExe()) {
		//if (g_bIs415) {
		//	HINSTANCE hinst = LoadLibrary(L"DT.dll");
		//	Il2fbSFS_mount_cpp = (IL2_SFS_mount_cpp)GetProcAddress(hinst, "_SFS_Mount@12");
		//}
		//else {
		//	HINSTANCE hinst = LoadLibrary(L"rts.dll");
		//	Il2fbSFS_mount_cpp = (IL2_SFS_mount_cpp)GetProcAddress(hinst, "_SFS_Mount@12");
		//}
		if (dtDllExists) {
			g_bIs415 = TRUE;
			HINSTANCE hinst = LoadLibrary(L"DT.dll");
			Il2fbSFS_mount_cpp = (IL2_SFS_mount_cpp)GetProcAddress(hinst, "_SFS_Mount@12");
		}
		if (Il2fbSFS_mount_cpp == NULL) {
			g_bIs415 = FALSE;
			HINSTANCE hinst = LoadLibrary(L"rts.dll");
			Il2fbSFS_mount_cpp = (IL2_SFS_mount_cpp)GetProcAddress(hinst, "_SFS_Mount@12");
		}
	}
	else {
		//if (g_bIs415) {
		//	HINSTANCE hinst = LoadLibrary(L"DT.dll");
		//	Il2fbSFS_mount_cpp = (IL2_SFS_mount_cpp)GetProcAddress(hinst, "_SFS_Mount@12");
		//}
		//else {
		//	Il2fbSFS_mount = (IL2_SFS_mount)GetProcAddress(GetModuleHandle(NULL), "SFS_mount");
		//}
		if (dtDllExists) {
			HINSTANCE hinst = LoadLibrary(L"DT.dll");
			Il2fbSFS_mount_cpp = (IL2_SFS_mount_cpp)GetProcAddress(hinst, "_SFS_Mount@12");
			g_bIs415 = Il2fbSFS_mount_cpp != NULL;
		}
		Il2fbSFS_mount = (IL2_SFS_mount)GetProcAddress(GetModuleHandle(NULL), "SFS_mount");
		//if (Il2fbSFS_mount == NULL) {
		//	HINSTANCE hinst = LoadLibrary(L"il2fb_sas.exe");
		//	Il2fbSFS_mount = (IL2_SFS_mount)GetProcAddress(hinst, "SFS_mount");
		//}
	}

	if (g_bIs415) {
		HINSTANCE hinst = LoadLibrary(L"DT.dll");
		PrintCpuInfo = (DT_PrintCpuInfo)GetProcAddress(hinst, "_PrintCpuInfo@0");
		int cpuInfo = PrintCpuInfo();
		GetCpuID = (DT_GetCpuID)GetProcAddress(hinst, "_GetCpuID@0");
		g_bCpuInfo = GetCpuID();
		TRACE("CPU ID = %08X\r\n", g_bCpuInfo);
		if (g_bCpuInfo & 0x30000) { // SSE4
			if (!IsInJvmOptions("-XX:UseSSE=4")) {
				AddJvmOption("-XX:UseSSE=4");
			}
			if (!IsInJvmOptions("-XX:+UseSSE42Intrinsics")) {
				AddJvmOption("-XX:+UseSSE42Intrinsics");
			}
		}
		else if (g_bCpuInfo & 0xC0) { // SSE3
			if (!IsInJvmOptions("-XX:UseSSE=3")) {
				AddJvmOption("-XX:UseSSE=3");
			}
			if (!IsInJvmOptions("-XX:-UseSSE42Intrinsics")) {
				AddJvmOption("-XX:-UseSSE42Intrinsics");
			}
		}
		else if (g_bCpuInfo & 0x20) { // SSE2
			if (!IsInJvmOptions("-XX:UseSSE=2")) {
				AddJvmOption("-XX:UseSSE=2");
			}
			if (!IsInJvmOptions("-XX:-UseSSE42Intrinsics")) {
				AddJvmOption("-XX:-UseSSE42Intrinsics");
			}
		}
		else if (g_bCpuInfo & 0x4) { // SSE1
			if (!IsInJvmOptions("-XX:UseSSE=1")) {
				AddJvmOption("-XX:UseSSE=1");
			}
			if (!IsInJvmOptions("-XX:-UseSSE42Intrinsics")) {
				AddJvmOption("-XX:-UseSSE42Intrinsics");
			}
		}
	}

	// When running an IL-2 server, show some output string on command line
	if (IsServerExe()) {
		printf("#########################################################\r\n");
		printf("#\r\n");
		printf("# IL-2 Server Executable %s %s\r\n", G_PRODUCT_VERSION, G_COPYRIGHT);
		printf("#\r\n");
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	LONG lRet = DetourAttach(&(PVOID&)JniCreateJavaVM, SAS_CreateJavaVM);
	if (lRet == NO_ERROR) {
		TRACE("Attach SAS_CreateJavaVM = OK\r\n");
	} else if (lRet == ERROR_INVALID_BLOCK) {
		TRACE("Attach SAS_CreateJavaVM = ERROR_INVALID_BLOCK\r\n");
	}
	else if (lRet == ERROR_INVALID_HANDLE) {
		TRACE("Attach SAS_CreateJavaVM = ERROR_INVALID_HANDLE\r\n");
	}
	else if (lRet == ERROR_INVALID_OPERATION) {
		TRACE("Attach SAS_CreateJavaVM = ERROR_INVALID_OPERATION\r\n");
	}
	else if (lRet == ERROR_NOT_ENOUGH_MEMORY) {
		TRACE("Attach SAS_CreateJavaVM = ERROR_NOT_ENOUGH_MEMORY\r\n");
	}
	else {
		TRACE("Attach SAS_CreateJavaVM = %d\r\n", lRet);
	}
		//	if (!IsServerExe()) {
		lRet = IsServerExe()||g_bIs415?DetourAttach(&(PVOID&)Il2fbSFS_mount_cpp, SAS_SFS_mount):DetourAttach(&(PVOID&)Il2fbSFS_mount, SAS_SFS_mount);
		if (lRet == NO_ERROR) {
			TRACE("Attach SAS_SFS_mount = OK\r\n");
		}
		else if (lRet == ERROR_INVALID_BLOCK) {
			TRACE("Attach SAS_SFS_mount = ERROR_INVALID_BLOCK\r\n");
		}
		else if (lRet == ERROR_INVALID_HANDLE) {
			TRACE("Attach SAS_SFS_mount = ERROR_INVALID_HANDLE\r\n");
		}
		else if (lRet == ERROR_INVALID_OPERATION) {
			TRACE("Attach SAS_SFS_mount = ERROR_INVALID_OPERATION\r\n");
		}
		else if (lRet == ERROR_NOT_ENOUGH_MEMORY) {
			TRACE("Attach SAS_SFS_mount = ERROR_NOT_ENOUGH_MEMORY\r\n");
		}
		else {
			TRACE("Attach SAS_SFS_mount = %d\r\n", lRet);
		}
		//	}
	DetourTransactionCommit();

	DWORD dwOldProtect = 0;
	DWORD dwProcRights = DELETE | READ_CONTROL | SYNCHRONIZE | WRITE_DAC | WRITE_OWNER
		| PROCESS_TERMINATE | PROCESS_CREATE_THREAD | PROCESS_SET_SESSIONID | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_DUP_HANDLE | PROCESS_CREATE_PROCESS | PROCESS_SET_QUOTA | PROCESS_SET_INFORMATION | PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME;
	DWORD dwCurProc = GetCurrentProcessId();
	HANDLE hCurProc = OpenProcess(dwProcRights, FALSE, dwCurProc);
	BOOL bIsStockExe = FALSE;
	BYTE checkStock[] = { 0x32, 0x2E, 0x30, 0x30, 0x00 }; // UPX 2.00 identification string in Stock il2fb.exe
	int iIl2fbUpxCheck = 0x004003DB; // Address of UPX 2.00 ID string prior to decompression
	try {
		if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2fbUpxCheck, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
			if (memcmp((LPVOID)iIl2fbUpxCheck, checkStock, 5) == 0) {
				bIsStockExe = TRUE;
			}

			VirtualProtectEx(hCurProc, (LPVOID)iIl2fbUpxCheck, 5, dwOldProtect, NULL);
		}
		else {
			TRACE("Error in AdjustJvmParams: Couldn't get access to il2fb.exe Version Check memory area!\r\n");
		}
	}
	catch (...) {
		TRACE("Error in AdjustJvmParams: Couldn't check il2fb.exe Version!\r\n");
	}

	if (bDisableMutex) {  // strip mutex string from process

		try {

			if (bIsStockExe) {  // disable Mutex for Stock il2fb.exe
				try {
					int iIl2fbUpxDecompressEnd = 0x01EA16EB;

					if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2fbUpxDecompressEnd, 9, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
						*(DWORD*)iIl2fbUpxDecompressEnd = 0x90909090; // fill with NOPs
						*(DWORD*)(iIl2fbUpxDecompressEnd + 4) = 0x90909090; // fill with NOPs
						*(BYTE*)(iIl2fbUpxDecompressEnd + 8) = 0x90; // fill with NOPs
						*(BYTE*)iIl2fbUpxDecompressEnd = 0xE9; // near JMP
						*(DWORD*)(iIl2fbUpxDecompressEnd + 1) = (DWORD)ApplyMutexSetting - 0x01EA16EB - 5; // run ApplyMutexSetting() after decompressing
						VirtualProtectEx(hCurProc, (LPVOID)iIl2fbUpxDecompressEnd, 9, dwOldProtect, NULL);
					}
					else {
						TRACE("Error in AdjustJvmParams: Couldn't get access to Stock il2fb.exe entry point memory area!\r\n");
					}
				}
				catch (...) {
					TRACE("Error in AdjustJvmParams: Couldn't inject code into Stock il2fb.exe entry point memory area!\r\n");
				}
			}
			else { // disable Mutex for mod enabled il2fb.exe
				if (!IsServerExe()) {
					try {
						int iIl2fbMutex = 0x0042206C;

						if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2fbMutex, 15, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
							ZeroMemory((LPVOID)iIl2fbMutex, 15); // disable Mutex
							VirtualProtectEx(hCurProc, (LPVOID)iIl2fbMutex, 9, dwOldProtect, NULL);
						}
						else {
							int iIl2fbMutex = 0x004085AC; // il2fb.exe 4.15
							if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2fbMutex, 15, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
								ZeroMemory((LPVOID)iIl2fbMutex, 15); // disable Mutex
								VirtualProtectEx(hCurProc, (LPVOID)iIl2fbMutex, 9, dwOldProtect, NULL);
							}
							else {
								TRACE("Error in AdjustJvmParams: Couldn't get access to modded il2fb.exe mutex string memory area!\r\n");
							}
						}
					}
					catch (...) {
						TRACE("Error in AdjustJvmParams: Couldn't erase modded il2fb.exe mutex string!\r\n");
					}
				}
			}
		}
		catch (...) {
			TRACE("Error in AdjustJvmParams: Couldn't check il2fb.exe Version!\r\n");
		}




	}
	if (g_bIs415 && !bIsStockExe && g_iModType != 0) {
		//	BYTE checkModEnabled[] = { 0xEB, 0x16 }; // IL-2 4.15 executable, jmp to -mods flag check
		//	BYTE skipCheckModEnabled[] = { 0xEB, 0x2D }; // IL-2 4.15 executable, jmp to -mods flag check
		//	int iIl2fbModsCheck = 0x0040744E; // Address of IL-2 4.15 executable, jmp to -mods flag check
			//BYTE checkModEnabled[] = { 0x74, 0x1D }; // IL-2 4.15 executable, jmp to -mods flag check
			//BYTE skipCheckModEnabled[] = { 0xEB, 0x1D }; // IL-2 4.15 executable, jmp to -mods flag check
			//int iIl2fbModsCheck = 0x0040745E; // Address of IL-2 4.15 executable, jmp to -mods flag check
			//dwOldProtect = 0;
			//if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2fbModsCheck, 2, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
			//	if (memcmp((LPVOID)iIl2fbModsCheck, checkModEnabled, 2) == 0) {
			//		memcpy((LPVOID)iIl2fbModsCheck, skipCheckModEnabled, 2);
			//	}

			//	VirtualProtectEx(hCurProc, (LPVOID)iIl2fbModsCheck, 2, dwOldProtect, NULL);
			//}
		BYTE modEnabledFlag[] = { 0x01 };
		int iIl2fbModsFlag = 0x0040D025;
		dwOldProtect = 0;
		if (NULL != VirtualProtectEx(hCurProc, (LPVOID)iIl2fbModsFlag, 1, PAGE_READWRITE, &dwOldProtect)) {
			memcpy((LPVOID)iIl2fbModsFlag, modEnabledFlag, 1);
			VirtualProtectEx(hCurProc, (LPVOID)iIl2fbModsFlag, 1, dwOldProtect, NULL);
			TRACE("AdjustJvmParams: Mod Flag set!\r\n");
		}
	}
	else {
		TRACE("AdjustJvmParams: Mod Flag NOT set!\r\n");
	}
	CloseHandle(hCurProc);
}

//************************************
// Method:    ReadSelectorSettings
// FullName:  ReadSelectorSettings
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void ReadSelectorSettings()
{
	bDisableMutex = (GetPrivateProfileInt(L"Settings", L"MultipleInstances", 0, szIniFile) != 0);
	int iSelectorRamSize = GetPrivateProfileInt(L"Settings", L"RamSize", 0, szIniFile);
	int iSelectorMemStrategy = GetPrivateProfileInt(L"Settings", L"MemoryStrategy", 0, szIniFile);
	g_iSplashScreenMode = GetPrivateProfileInt(L"Settings", L"SplashScreenMode", 0, szIniFile);
	int iDumpMode = GetPrivateProfileInt(L"Settings", L"DumpMode", 0, szIniFile);
	if (iDumpMode & 0x01) g_bDumpSFSAccess = TRUE;
	if (iDumpMode & 0x02) g_bDumpFiles = TRUE;
	g_bInstantDump = GetPrivateProfileInt(L"Settings", L"InstantDump", 0, szIniFile) != 0;
	ZeroMemory(szBuffer, sizeof(szBuffer));
	if (GetPrivateProfileString(L"Settings", L"DumpSeparatorChar", L" ", szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szIniFile) > 0) {
		if (_tcslen(szBuffer) == 1) {
			ZeroMemory(cBuffer, sizeof(cBuffer));
			wcstombs(cBuffer, szBuffer, sizeof(cBuffer));
			g_cDumpSeparator = cBuffer[0];
		}
	}
	else {
		g_cDumpSeparator = ' ';
	}
	g_bEnablePipeLogger = GetPrivateProfileInt(L"Settings", L"PipeLogger", 1, szIniFile) != 0;

	g_iDebugMode = GetPrivateProfileInt(L"Settings", L"DebugMode", 0, szIniFile);

	if (iSelectorRamSize != 0) {

		TRACE("Applying JVM Memory Settings from IL-2 Selector...\r\n");
		int iStackSize = (iSelectorRamSize * 1024) / XSS_DIVIDER;
		int iPermSize = iSelectorRamSize / PERM_DIVIDER;

		switch (iSelectorMemStrategy) {
		case MEM_STRATEGY_CONSERVATIVE:
			iStackSize /= 2;
			iPermSize /= 2;
			break;

		case MEM_STRATEGY_HEAPONLY:
			iStackSize = 0;
			iPermSize = 0;
			break;

		default:
			break;
		}

		int iDynamicSize = iSelectorRamSize - (iStackSize / 1024) - iPermSize;
		ZeroMemory(cBuffer, sizeof(cBuffer));
		sprintf(cBuffer, "-Xms%dM", iDynamicSize);
		AddJvmOption(cBuffer);
		//ZeroMemory(cBuffer, sizeof(cBuffer));
		//sprintf(cBuffer, "-XX:InitialHeapSize=%dM", iDynamicSize);
		//AddJvmOption(cBuffer);
		ZeroMemory(cBuffer, sizeof(cBuffer));
		sprintf(cBuffer, "-Xmx%dM", iDynamicSize);
		AddJvmOption(cBuffer);
		//ZeroMemory(cBuffer, sizeof(cBuffer));
		//sprintf(cBuffer, "-XX:MaxHeapSize=%dM", iDynamicSize);
		//AddJvmOption(cBuffer);

		if (iSelectorMemStrategy == MEM_STRATEGY_HEAPONLY) {
			// TRACE("ReadSelectorSettings()---1\r\n");
			return;
		}

		ZeroMemory(cBuffer, sizeof(cBuffer));
		if (g_bIs415 && iStackSize > 1024) iStackSize = 1024;
		sprintf(cBuffer, "-Xss%dK", iStackSize);
		AddJvmOption(cBuffer);
		//ZeroMemory(cBuffer, sizeof(cBuffer));
		//sprintf(cBuffer, "-XX:ThreadStackSize=%d", iStackSize);
		//AddJvmOption(cBuffer);
		 
		if (!g_bIs415) {
			ZeroMemory(cBuffer, sizeof(cBuffer));
			sprintf(cBuffer, "-XX:PermSize=%dM", iPermSize);
			AddJvmOption(cBuffer);
			ZeroMemory(cBuffer, sizeof(cBuffer));
			sprintf(cBuffer, "-XX:MaxPermSize=%dM", iPermSize);
			AddJvmOption(cBuffer);
		}
	}

	g_iModType = GetPrivateProfileInt(L"Settings", L"ModType", 0, szIniFile);
	memset(szBuffer2, 0, sizeof(szBuffer2));
	_stprintf(szBuffer2, L"Modtype_%02d", g_iModType);
	BOOL defaultFilesSfs = TRUE;
	if (IsServerExe()) {
		if (GetPrivateProfileString(szBuffer2, L"FilesServer.SFS", L"filesserver.sfs", szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szIniFile) > 0) {
			if (_tcslen(szBuffer) > 0) {
				ZeroMemory(g_cFilesSFS, sizeof(g_cFilesSFS));
				wcstombs(g_cFilesSFS, szBuffer, sizeof(g_cFilesSFS));
				defaultFilesSfs = FALSE;
			}
		}
		if (defaultFilesSfs) {
			strcpy(g_cFilesSFS, "filesserver.sfs");
		}
	}
	else {
		if (GetPrivateProfileString(szBuffer2, L"Files.SFS", L"files.sfs", szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szIniFile) > 0) {
			if (_tcslen(szBuffer) > 0) {
				ZeroMemory(g_cFilesSFS, sizeof(g_cFilesSFS));
				wcstombs(g_cFilesSFS, szBuffer, sizeof(g_cFilesSFS));
				defaultFilesSfs = FALSE;
			}
		}
		if (defaultFilesSfs) {
			strcpy(g_cFilesSFS, "files.sfs");
		}
	}
	memset(g_szSplashImage, 0, sizeof(g_szSplashImage));
	GetPrivateProfileString(szBuffer2, L"Splash", L"", g_szSplashImage, sizeof(g_szSplashImage) / sizeof(TCHAR), szIniFile);
}

//************************************
// Method:    ReadConfSettings
// FullName:  ReadConfSettings
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void ReadConfSettings()
{
	if (GetPrivateProfileString(L"GLPROVIDER", L"GL", L"dx8wrap.dll", szBuffer, MAX_PATH, szConfIniFile)) {
		LPTSTR szGlProvider = szBuffer;
		_tcstrim(szGlProvider);
		if (_tcsicmp(szGlProvider, L"opengl32.dll") == 0) g_bIsOpenGL = TRUE;
	}
}

//************************************
// Method:    ReadJvmOptions
// FullName:  ReadJvmOptions
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void ReadJvmOptions()
{
	TRACE("ReadJvmOptions()\r\n");
	TCHAR szJvmOptionsInFile[0xFFFF];
	ZeroMemory(szJvmOptionsInFile, sizeof(szJvmOptionsInFile));
	TCHAR szJvmOptions[0xFFFF];
	ZeroMemory(szJvmOptions, sizeof(szJvmOptions));
	char cBuf[MAX_PATH];
	GetPrivateProfileSection(L"JVM", szJvmOptionsInFile, 0xFFFF, szIniFile);
	LPTSTR lpJvmOptionToken = szJvmOptionsInFile;

	while (_tcslen(lpJvmOptionToken) != 0) {
		ZeroMemory(cBuf, sizeof(cBuf));
		wcstombs(cBuf, lpJvmOptionToken, MAX_PATH);
		AddJvmOption(cBuf);
		lpJvmOptionToken += _tcslen(lpJvmOptionToken) + 1;
		TRACE("Adding JVM Option from Ini: %s\r\n", cBuf);
	}
}

//************************************
// Method:    AddMandatoryJvmOptions
// FullName:  AddMandatoryJvmOptions
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void AddMandatoryJvmOptions()
{
	TRACE("Checking mandatory JVM Options...\r\n");
	if (g_bIs415) {
		if (g_iDebugMode == 1) {
			TRACE("DebugMode=1\r\n");
			AddJvmOption("-Djava.class.path=.");
			AddJvmOption("-Xverify:remote");
			AddJvmOption("-Xcomp");
		}
		else {
			if (!IsInJvmOptions("-Djava.class.path")) {
				AddJvmOption("-Djava.class.path=.");
			}

			if (!IsInJvmOptions("-Xverify")) {
				AddJvmOption("-Xverify:remote");
			}

			if (!IsInJvmOptions("-Xcomp")) {
				AddJvmOption("-Xcomp");
			}
		}
		if (!IsInJvmOptions("-XX:MaxGCPauseMillis")) {
			AddJvmOption("-XX:MaxGCPauseMillis=1000");
		}
		if (!IsInJvmOptions("-XX:+PrintCommandLineFlags")) {
			AddJvmOption("-XX:+PrintCommandLineFlags");
		}
		if (!IsInJvmOptions("-XX:+PrintVMOptions")) {
			AddJvmOption("-XX:+PrintVMOptions");
		}
		if (!IsInJvmOptions("-XX:+UseSerialGC") && !IsInJvmOptions("-XX:+UseConcMarkSweepGC") && !IsInJvmOptions("-XX:+UseParallelGC") && !IsInJvmOptions("-XX:+UseG1GC")) {
			AddJvmOption("-XX:+UseG1GC");
		}
		if (!IsInJvmOptions("-XX:+UseStringDeduplication")) {
			AddJvmOption("-XX:+UseStringDeduplication");
		}
		if (!IsInJvmOptions("-XX:ReservedCodeCacheSize")) {
			AddJvmOption("-XX:ReservedCodeCacheSize=128m");
		}
		if (!IsInJvmOptions("-XX:+PrintCodeCache")) {
			AddJvmOption("-XX:+PrintCodeCache");
		}
		if (!IsInJvmOptions("-XX:+CITime")) {
			AddJvmOption("-XX:+CITime");
		}
	}
	else {
		if (g_iDebugMode == 1) {
			TRACE("DebugMode=1\r\n");
			AddJvmOption("-Djava.class.path=.");
			AddJvmOption("-Xverify:none");
			AddJvmOption("-Xcomp");
		}
		else {
			if (!IsInJvmOptions("-Djava.class.path")) {
				AddJvmOption("-Djava.class.path=.");
			}

			if (!IsInJvmOptions("-Xverify")) {
				AddJvmOption("-Xverify:none");
			}

			if (!IsInJvmOptions("-Xcomp")) {
				AddJvmOption("-Xcomp");
			}
		}
	}
}

//************************************
// Method:    GetParams
// FullName:  GetParams
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void GetParams()
{
	JvmOptions.clear();
	ReadSelectorSettings();
	ReadConfSettings();
	ReadJvmOptions();
	AddMandatoryJvmOptions();
}

//************************************
// Method:    StartWatchdog
// FullName:  StartWatchdog
// Access:    public 
// Returns:   BOOL
// Qualifier:
//************************************
BOOL StartWatchdog()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR szWatchdogFile[MAX_PATH];
	TCHAR szWatchdogParams[MAX_PATH];
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(szWatchdogFile, sizeof(szWatchdogFile));
	ZeroMemory(szWatchdogParams, sizeof(szWatchdogParams));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	_stprintf(szWatchdogParams, L"%d \"%s\"", g_iSplashScreenMode, g_szSplashImage);

	_tcscpy(szWatchdogFile, szCurDir);
	_tcscat(szWatchdogFile, L"bin\\selector\\basefiles\\IL-2 Watchdog.exe");
	TRACE("IL-2 Process ID = %08X\r\n", GetCurrentProcessId());
	TRACE(L"Starting Watchdog at %s %s\r\n", szWatchdogFile, szWatchdogParams);

	if (!CreateProcess(szWatchdogFile, szWatchdogParams, NULL,
		NULL, FALSE,
		CREATE_NEW_CONSOLE | CREATE_BREAKAWAY_FROM_JOB | CREATE_SUSPENDED,
		NULL, szCurDir, &si, &pi)) {
		TRACE("Watchdog Process not created, last Error: %08X\r\n", GetLastError());
		return FALSE;
	}

	if (g_hJobObject == NULL) g_hJobObject = CreateJobObject(NULL, NULL);
	if (g_hJobObject == NULL) {
		TRACE("Job Object not created\r\n");
		TerminateProcess(pi.hProcess, -1);
		return FALSE;
	}
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	if (SetInformationJobObject(g_hJobObject, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)) == FALSE) {
		TRACE("Job Object extended limit information not set\r\n");
		TerminateProcess(pi.hProcess, -1);
		return FALSE;
	}

	if (AssignProcessToJobObject(g_hJobObject, pi.hProcess) == FALSE) {
		TRACE("Watchdog Process not assigned to Job Object\r\n");
		TerminateProcess(pi.hProcess, -1);
		return FALSE;
	}

	ResumeThread(pi.hThread);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	TRACE("Watchdog process started.\r\n");
	Sleep(0);
	return TRUE;
}

//************************************
// Method:    StartPipeLogger
// FullName:  StartPipeLogger
// Access:    public 
// Returns:   BOOL
// Qualifier:
//************************************
BOOL StartPipeLogger()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	TCHAR szPipeLoggerFile[MAX_PATH];
	ZeroMemory(szPipeLoggerFile, sizeof(szPipeLoggerFile));
	TCHAR szPipeLoggerArgs[MAX_PATH];
	ZeroMemory(szPipeLoggerArgs, sizeof(szPipeLoggerArgs));
	_stprintf(szPipeLoggerFile, L"%s%s", szCurDir, L"bin\\selector\\basefiles\\PipeLogger.exe");
	_stprintf(szPipeLoggerArgs, L"\"%s\" \"%s\"", szPipeLoggerFile, szIniFile);

	TRACE(L"Starting Pipe Logger at %s\r\n", szPipeLoggerFile);
	if (!CreateProcess(szPipeLoggerFile, szPipeLoggerArgs, NULL,
		NULL, TRUE,
		CREATE_NEW_CONSOLE,
		NULL, szCurDir, &si, &pi)) {
		TRACE("PipeLogger Process not created, last Error: %08X\r\n", GetLastError());
		return FALSE;
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	TRACE("PipeLogger process started.\r\n");
	Sleep(0);
	return TRUE;
}

//************************************
// Method:    versionString
// FullName:  versionString
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: char* buffer
// Parameter: int numToken
// Parameter: ...
//************************************
void versionString(char* buffer, int numToken, ...) {
	va_list argptr;
	va_start(argptr, numToken);
	sprintf(buffer, "%d.%d.%d.%d", va_arg(argptr, int), va_arg(argptr, int), va_arg(argptr, int), va_arg(argptr, int));
	va_end(argptr);
}

//************************************
// Method:    FileExists
// FullName:  FileExists
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR szPath
//************************************
BOOL FileExists(LPCTSTR szPath) {
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//************************************
// Method:    _tcstrim
// FullName:  _tcstrim
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: LPTSTR str
//************************************
void _tcstrim(LPTSTR str) {
	int start = 0; // number of leading spaces
	LPTSTR buffer = str;
	while (*str && *str++ == L' ') ++start;
	while (*str++); // move to end of string
	int end = str - buffer - 1;
	while (end > 0 && buffer[end - 1] == L' ') --end; // backup over trailing spaces
	buffer[end] = 0; // remove trailing spaces
	if (end <= start || start == 0) return; // exit if no leading spaces or string is now empty
	str = buffer + start;
	while ((*buffer++ = *str++));  // remove leading spaces: K&R
}

//************************************
// Method:    getSelectorInfo
// FullName:  Java_com_maddox_sas1946_il2_util_BaseGameVersion_getSelectorInfo
// Access:    public 
// Returns:   JNIEXPORT jstring JNICALL
// Qualifier:
// Parameter: JNIEnv* env
// Parameter: jobject obj
// Parameter: jint infoRequested
//************************************
JNIEXPORT jstring JNICALL Java_com_maddox_sas1946_il2_util_BaseGameVersion_getSelectorInfo(JNIEnv* env, jobject obj, jint infoRequested) {
	char buffer[128];
	ZeroMemory(buffer, sizeof(buffer));
	switch (infoRequested) {
	case SELECTOR_INFO_FILE_VERSION:
		sprintf(buffer, G_FILE_VERSION);
		break;
	case SELECTOR_INFO_PRODUCT_VERSION:
		sprintf(buffer, G_PRODUCT_VERSION);
		break;
	case SELECTOR_INFO_FILEVERSION:
		versionString(buffer, G_FILEVERSION);
		break;
	case SELECTOR_INFO_PRODUCTVERSION:
		versionString(buffer, G_PRODUCTVERSION);
		break;
	case SELECTOR_INFO_SPECIAL_BUILD:
		sprintf(buffer, G_SPECIAL_BUILD);
		break;
	case SELECTOR_INFO_COPYRIGHT:
		sprintf(buffer, G_COPYRIGHT);
		break;
	default:
		sprintf(buffer, "unknown Selector Info requested");
		break;
	}
	return env->NewStringUTF(buffer);
}

