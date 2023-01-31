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

#include "stdafx.h"
#include "Objidl.h"
#include "Wincodec.h"
#pragma comment(lib, "windowscodecs.lib")

#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H
#define RunWorker(ThreadStartRoutine) 	CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadStartRoutine,NULL,0,NULL))
#define SPLASH_SCREEN_VISIBLE		1
#define SPLASH_SCREEN_TOPMOST		2

struct SplashParams {
    HINSTANCE hInstance;
    LPCTSTR lpIconName;
    LPCTSTR lpszClassName;
    LPCTSTR lpszSplashTitle;
    LPCTSTR lpSplashImageName;
    LPCTSTR lpSplashImageType;
    SplashParams(HINSTANCE theInstance, LPCTSTR theIconName, LPCTSTR theClassName, LPCTSTR theSplashTitle, LPCTSTR theSplashImageName, LPCTSTR theSplashImageType) {
        hInstance = theInstance;
        lpIconName = theIconName;
        lpszClassName = theClassName;
        lpszSplashTitle = theSplashTitle;
        lpSplashImageName = theSplashImageName;
        lpSplashImageType = theSplashImageType;
    }
};

IStream * CreateStreamOnResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);
IWICBitmapSource * LoadBitmapFromStream(IStream * ipImageStream);
HBITMAP CreateHBITMAP(IWICBitmapSource * ipBitmap);
HBITMAP LoadSplashImage(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);
void RegisterWindowClass(HINSTANCE hInstance, LPCTSTR lpIconName, LPCTSTR lpszClassName);
HWND CreateSplashWindow(HINSTANCE hInstance, LPCTSTR lpszClassName);
void SetSplashImage(HWND hwndSplash, HBITMAP hbmpSplash);
void ShowSplash(HINSTANCE hInstance, LPCTSTR lpIconName, LPCTSTR lpszClassName, LPCTSTR lpszSplashTitle, LPCTSTR lpSplashImageName, LPCTSTR lpSplashImageType);
void CloseSplash();
UINT StartSplashThread(LPVOID pParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

#endif