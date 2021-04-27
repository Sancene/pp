#pragma once

#include <fstream>
#include "List.h"
#include "LogFileWriter.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#undef min
#undef max

class LogBuffer {
private:
	List list{};
	LogFileWriter log_file_writer{};
	CRITICAL_SECTION criticals_section;
	HANDLE thread;
	HANDLE event;

public:
	LogBuffer() {
		InitializeCriticalSectionAndSpinCount(&criticals_section, 0x00000400);

		event = CreateEvent(NULL, TRUE, FALSE, TEXT("Event"));
		thread = CreateThread(NULL, 0, &LogSizeMonitoringThread, (void*)this, 0, NULL);
	}

	~LogBuffer() {
		DeleteCriticalSection(&criticals_section);
	}

	void Log(int value) {
		EnterCriticalSection(&criticals_section);

		if (list.CountNode() >= 50) {
			SetEvent(event);

			DWORD wait = WaitForSingleObject(thread, INFINITE);
			if (wait == WAIT_OBJECT_0) {
				ResetEvent(event);

				thread = CreateThread(NULL, 0, &LogSizeMonitoringThread, (void*)this, 0, NULL);
			}
		}
		list.AddNode(value);

		LeaveCriticalSection(&criticals_section);
	}

	static DWORD WINAPI LogSizeMonitoringThread(CONST LPVOID lp_param) {
		LogBuffer* param = (LogBuffer*)lp_param;

		DWORD wait = WaitForSingleObject(param->event, INFINITE);

		if (wait == WAIT_OBJECT_0) {
			param->log_file_writer.Log(&param->list);
		}

		ExitThread(EXIT_SUCCESS);
	}
};