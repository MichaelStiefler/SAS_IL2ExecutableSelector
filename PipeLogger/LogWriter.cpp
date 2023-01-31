//*****************************************************************
// PipeLogger.exe - Enhanced Logging Tool for IL-2 1946
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of PipeLogger.exe.
//
// PipeLogger.exe is free software.
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
// Includes
//*************************************************************************
#include "stdafx.h"
#include "LogWriter.h"
#include "trace.h"
#include "globals.h"
#include <regex>

FILE* OpenLogFile(LPCTSTR logFileName) {
	FILE *retVal = NULL;
	while (true) {
		retVal = _tfsopen(logFileName, L"ab", _SH_DENYWR);

		if (retVal != NULL) {
			break;
		}

		Sleep(rand() % 100);
	}
	return retVal;
}

void CloseLogFile(FILE *logFile) {
	fflush(logFile);
	fclose(logFile);
}

FILE* FlushLogFile(FILE *logFile, LPCTSTR logFileName) {
	CloseLogFile(logFile);
	return OpenLogFile(logFileName);
}

BOOL flushQueue(FILE *logFile, TQueueConcurrent<std::string> *logQueue, unsigned long *bytesWritten, unsigned long *exceptions, unsigned long *errors) {
	BOOL retVal = FALSE;
	try {
		std::optional<std::string> queueElement = logQueue->pop_front();
		while (queueElement.has_value()) {
			std::regex rgxexception("java\\.(lang|util|io|net|time|security)\\.[a-zA-Z]+Exception");
			std::regex rgxerror("java\\.(lang|util|io|net|time|security)\\.[a-zA-Z]+Error");
			std::ptrdiff_t const match_count_exception(std::distance(
				std::sregex_iterator(queueElement.value().begin(), queueElement.value().end(), rgxexception),
				std::sregex_iterator()));
			std::ptrdiff_t const match_count_error(std::distance(
				std::sregex_iterator(queueElement.value().begin(), queueElement.value().end(), rgxerror),
				std::sregex_iterator()));
			*exceptions += (unsigned long)match_count_exception;
			*errors += (unsigned long)match_count_error;
			*bytesWritten += fprintf(logFile, queueElement->c_str());
			retVal = TRUE;
			queueElement = logQueue->pop_front();
		}
	}
	catch (...) {
		TRACE("Exception occured in flushQueue()\r\n");
	}
	return retVal;
}

DWORD WINAPI LogWriterThread(LPVOID lpvParam)
{
	LogWriterParams* pLogWriterParams =(LogWriterParams*)lpvParam;
	HANDLE hNewData;
	HANDLE hEndLogWriter;
	HANDLE hTerminatePipeLogger;
	BOOL bDuplicateHandles = DuplicateHandle(GetCurrentProcess(), pLogWriterParams->hNewData, GetCurrentProcess(), &hNewData, 0, FALSE, DUPLICATE_SAME_ACCESS)
							&& DuplicateHandle(GetCurrentProcess(), pLogWriterParams->hEndLogWriter, GetCurrentProcess(), &hEndLogWriter, 0, FALSE, DUPLICATE_SAME_ACCESS)
							&& DuplicateHandle(GetCurrentProcess(), g_hTerminatePipeLogger, GetCurrentProcess(), &hTerminatePipeLogger, 0, FALSE, DUPLICATE_SAME_ACCESS);
	if (!bDuplicateHandles) {
		TRACE("Log Writer No.%d could not duplicate required Wait Handles!\r\n", pLogWriterParams->dwIndex);
		return -1;
	}
	HANDLE waitHandles[] = { hNewData, hEndLogWriter, hTerminatePipeLogger };
	unsigned long totalBytesWritten = 0;
	unsigned long totalExceptions = 0;
	unsigned long totalErrors = 0;

	TCHAR* pchLogFile = (TCHAR*)calloc(_tcslen(pLogWriterParams->pchLogFile) + 1, sizeof(TCHAR));
	_tcscpy(pchLogFile, pLogWriterParams->pchLogFile);

	DWORD dwIndex = pLogWriterParams->dwIndex, dwFlushTimeout = pLogWriterParams->dwFlushTimeout;
	TQueueConcurrent<std::string> *logQueue = pLogWriterParams->logQueue;

	g_aiLogWriters++;
	TRACE("Log Writer No.%d running\r\n", dwIndex);
	DWORD lastFlush = GetTickCount();
	BOOL bInstantFlush = FALSE;
	if (dwFlushTimeout == 0) {
		dwFlushTimeout = INFINITE;
		bInstantFlush = TRUE;
	}
	FILE *logFile = OpenLogFile(pchLogFile);
	BOOL bFlushPending = FALSE;
	BOOL bContinue = TRUE;
	try {
		while (bContinue)
		{
			switch (WaitForMultipleObjects(3, waitHandles, FALSE, dwFlushTimeout)) {
			case WAIT_OBJECT_0:
				bFlushPending = flushQueue(logFile, logQueue, &totalBytesWritten, &totalExceptions, &totalErrors);
				if (bInstantFlush) {
					if (bFlushPending) {
						logFile = FlushLogFile(logFile, pchLogFile);
						bFlushPending = FALSE;
					}
				}
				else if (dwFlushTimeout != INFINITE && GetTickCount() - lastFlush > dwFlushTimeout) {
					logFile = FlushLogFile(logFile, pchLogFile);
					lastFlush = GetTickCount();
					bFlushPending = FALSE;
				}
				break;
			case WAIT_OBJECT_0 + 1:
			case WAIT_OBJECT_0 + 2:
				flushQueue(logFile, logQueue, &totalBytesWritten, &totalExceptions, &totalErrors);
				bContinue = FALSE;
				break;
			case WAIT_TIMEOUT:
				logFile = FlushLogFile(logFile, pchLogFile);
				lastFlush = GetTickCount();
				bFlushPending = FALSE;
				break;
			default:
				break;
			}
		}
	}
	catch (...) { }
	unsigned long size = ftell(logFile);
	if (totalBytesWritten < size) totalBytesWritten = size;
	CloseLogFile(logFile);
	g_aiLogWriters--;
	CloseHandle(hNewData);
	CloseHandle(hEndLogWriter);
	CloseHandle(hTerminatePipeLogger);
	TCHAR buf2[32];
	TCHAR buf3[32];
	TCHAR buf4[32];
	memset(buf2, 0, sizeof(buf2));
	memset(buf3, 0, sizeof(buf3));
	memset(buf4, 0, sizeof(buf4));
	format_commas(totalBytesWritten, buf2);
	format_commas(totalExceptions, buf3);
	format_commas(totalErrors, buf4);
	TRACE(L"Log Writer No.%d terminated. %s bytes written, %s exceptions, %s errors.\r\n", dwIndex, buf2, buf3, buf4);

	if (g_bShowLogWarnings && (totalBytesWritten > g_ulLogSizeThreshold || totalExceptions > g_ulExceptionThreshold || totalErrors > g_ulErrorThreshold)) {
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		TCHAR buf[1024];
		memset(buf, 0, sizeof(buf));
		_tsplitpath(pchLogFile, drive, dir, fname, ext);
		_stprintf(buf, TEXT("Your logfile %s%s seems to contain issues that might need to be addressed.\r\n"
			"These are the key figures of your logfile %s%s:\r\n"
			"Size: %s Bytes\r\n"
			"Exceptions: %s\r\n"
			"Errors: %s\r\n"
			"\r\n"
			"Please choose whether you want to save a backup of this logfile for later reference!"),
			fname, ext,
			fname, ext,
			buf2,
			buf3,
			buf4);
		g_aiPendingMessages++;
		if (IDYES == MessageBox(NULL,
			buf,
			TEXT("The Logfile needs your attention!"),
			MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON1 | MB_SETFOREGROUND | MB_TOPMOST)) {
			SYSTEMTIME stSystemTime;
			GetLocalTime(&stSystemTime);
			_stprintf(buf, TEXT("%s%s%s (%04d-%02d-%02d %02d·%02d·%02d.%03d)%s"),
				drive, dir, fname,
				stSystemTime.wYear,
				stSystemTime.wMonth,
				stSystemTime.wDay,
				stSystemTime.wHour,
				stSystemTime.wMinute,
				stSystemTime.wSecond,
				stSystemTime.wMilliseconds,
				ext);
			if (CopyFile(pchLogFile, buf, FALSE)) {
				TRACE(L"Backing up logfile \"%s\" to \"%s\" succeeded.\r\n", pchLogFile, buf);
			} else {
				TRACE(L"Backing up logfile \"%s\" to \"%s\" failed.\r\n", pchLogFile, buf);
				_stprintf(buf, TEXT("Unable to save a backup of your logfile.\r\n"
					"Please make sure to backup the logfile %s%s yourself!"),
					fname, ext
				);
				MessageBox(NULL,
					buf,
					TEXT("Failed to save a backup of your logfile!\r\n"),
					MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
			}
		}
		else {
			TRACE(L"User chose not to backup logfile \"%s\".\r\n", pchLogFile);
		}
		g_aiPendingMessages--;
	}
	free(pchLogFile);
	return 0;
}

