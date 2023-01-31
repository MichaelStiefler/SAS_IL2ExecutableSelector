//*****************************************************************
// il2fb.exe - SAS IL-2 Executable Selector
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of il2fb.exe.
//
// il2fb.exe is free software.
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

#ifndef _CRC32STATIC_H_
#define _CRC32STATIC_H_

#include "Common.h"

class CCrc32Static
{
public:
    CCrc32Static();
    virtual ~CCrc32Static();

    static DWORD StringCrc32(LPCTSTR szString, DWORD &dwCrc32);
    static DWORD FileCrc32Streams(LPCTSTR szFilename, DWORD &dwCrc32);
    static DWORD FileCrc32Win32(LPCTSTR szFilename, DWORD &dwCrc32);
    static DWORD FileCrc32Filemap(LPCTSTR szFilename, DWORD &dwCrc32);
    static DWORD FileCrc32Assembly(LPCTSTR szFilename, DWORD &dwCrc32);

protected:
    static bool GetFileSizeQW(const HANDLE hFile, QWORD &qwSize);
    static inline void CalcCrc32(const BYTE byte, DWORD &dwCrc32);

    static DWORD s_arrdwCrc32Table[256];
};

#endif
