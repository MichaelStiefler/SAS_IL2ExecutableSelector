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
#include "SAS IL-2 Wrapper.h"
#include "trace.h"

extern HINSTANCE g_hInstance;


//************************************
// Method:    DllMain
// FullName:  DllMain
// Access:    public 
// Returns:   BOOL APIENTRY
// Qualifier:
// Parameter: HMODULE hModule
// Parameter: DWORD ul_reason_for_call
// Parameter: LPVOID lpReserved
//************************************
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                     )
{
    switch(ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        StartWrapper(hModule);
        break;

    case DLL_THREAD_ATTACH:
        ThreadAttach();
        break;

    case DLL_THREAD_DETACH:
        ThreadDetach();
        break;

    case DLL_PROCESS_DETACH:
        StopWrapper();
        break;
    }

    return TRUE;
}

