//*****************************************************************
// IL-2 Watchdog.exe - il2fb.exe Watchdog
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of IL-2 Watchdog.exe.
//
// IL-2 Watchdog.exe is free software.
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

#ifndef MAX_COPY_LENGTH
#define MAX_COPY_LENGTH 128
#endif

class CopyPacket
{
public:
    GUID Signature;
    BYTE data[MAX_COPY_LENGTH];
public:
    BOOL Same(const GUID & s) {
        return memcmp(&Signature, &s, sizeof(GUID)) == 0;
    }
};


class CopyData
{
public:
    CopyData(UINT id, GUID s) {
        packet.Signature = s;
        cds.dwData = id;
        cds.cbData = 0;
        cds.lpData = &packet;
    }
    UINT SetLength(UINT n) {
        cds.cbData = sizeof(packet.Signature) + n;
        return cds.cbData;
    }
    void SetData(LPCVOID src, size_t length) {
        CopyMemory(packet.data, src, length);
    }
    LRESULT Send(HWND target, HWND sender) {
        return SendMessage(target, WM_COPYDATA, (WPARAM)sender, (LPARAM)&cds);
    }
    static UINT GetMinimumLength() {
        return sizeof(GUID);
    }
protected:
    COPYDATASTRUCT cds;
    CopyPacket packet;
};