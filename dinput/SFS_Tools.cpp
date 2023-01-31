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
#include "SFS_Tools.h"

unsigned __int64 LongFN(unsigned __int64 hash, LPCTSTR buf) {
	int paramLen = _tcslen(buf);
	unsigned __int32 c;
	unsigned __int32 a = (unsigned)(hash & 0xFFFFFFFF);
	unsigned __int32 b = (unsigned)(hash >> 32 & 0xFFFFFFFF);
	for (int i = 0; i < paramLen; i++) {
		c = (unsigned __int32)buf[i];
		if ((c > 96) && (c < 123))
			c &= 223;
		else if (c == 47)
			c = 92;
		a = (a << 8 | c) ^ FPaTable[a >> 24];
		b = (b << 8 | c) ^ FPbTable[b >> 24];
	}
	return ((unsigned __int64)a & 0xFFFFFFFF) | ((unsigned __int64)b << 32);
}

unsigned __int64 LongFN(const unsigned __int64 hash, LPCSTR buf)
{
	int len = strlen(buf);
	unsigned __int32 c;
	unsigned __int32 a = (unsigned __int32)(hash & 0xFFFFFFFF);
	unsigned __int32 b = (unsigned __int32)(hash >> 32 & 0xFFFFFFFF);

	for (int i = 0; i < len; i++) {
		c = (unsigned __int32)buf[i];
		if ((c > 96) && (c < 123))
			c &= 223;
		else if (c == 47)
			c = 92;
		a = (a << 8 | c) ^ FPaTable[a >> 24];
		b = (b << 8 | c) ^ FPbTable[b >> 24];
	}

	return (unsigned __int64)a & 0xFFFFFFFF | (unsigned __int64)b << 32;
}

unsigned __int32 IntFN(unsigned __int32 hash, LPCTSTR buf) {
	int paramLen = _tcslen(buf);
	unsigned __int32 c;
	unsigned __int32 b;
	unsigned __int32 a = hash;
	for (int i = 0; i < paramLen; i++) {
		c = (unsigned __int32)buf[i];
		b = c & 0xFF;
		a = (a << 8 | b) ^ FPaTable[a >> 24];
		b = c >> 8 & 0xFF;
		a = (a << 8 | b) ^ FPaTable[a >> 24];
	}
	return (unsigned __int32)a;
}

unsigned __int32 IntFN(unsigned __int32 hash, LPCSTR buf) {
	int len = strlen(buf);
	unsigned __int32 c;
	unsigned __int32 b;
	unsigned __int32 a = hash;
	for (int i = 0; i < len; i++) {
		c = (unsigned __int32)buf[i];
		b = c & 0xFF;
		a = (a << 8 | b) ^ FPaTable[a >> 24];
		b = c >> 8 & 0xFF;
		a = (a << 8 | b) ^ FPaTable[a >> 24];
	}
	return (unsigned __int32)a;
}

unsigned __int64 SFS_hash(const unsigned __int64 hash, const void *buf, const int len)
{
	unsigned char c;
	unsigned a = (unsigned)(hash & 0xFFFFFFFF);
	unsigned b = (unsigned)(hash >> 32 & 0xFFFFFFFF);

	for (int i = 0; i < len; i++) {
		c = ((unsigned char *)buf)[i];
		a = (a << 8 | c) ^ FPaTable[a >> 24];
		b = (b << 8 | c) ^ FPbTable[b >> 24];
	}

	return (unsigned __int64)a & 0xFFFFFFFF | (unsigned __int64)b << 32;
}

