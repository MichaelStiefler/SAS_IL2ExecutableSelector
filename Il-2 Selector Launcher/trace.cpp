//*****************************************************************
// IL-2 Selector.exe - IL-2 Selector Launcher
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of IL-2 Selector.exe.
//
// IL-2 Selector.exe is free software.
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
#ifdef _DEBUG
#pragma warning( disable : 4996 )
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
void Proto(LPCTSTR szLine);

//************************************
// Method:    _trace
// FullName:  _trace
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: TCHAR * format
// Parameter: ...
//************************************
bool _trace(TCHAR *format, ...)
{
    TCHAR buffer[1000];
    va_list argptr;
    va_start(argptr, format);
    wvsprintf(buffer, format, argptr);
    va_end(argptr);
    _tprintf(buffer);
    OutputDebugString(buffer);
    return true;
}


#endif