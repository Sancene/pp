#pragma once
#include <windows.h>
#include <iostream>
#include "ITask.h"
#include <vector>

extern CRITICAL_SECTION CriticalSection;

class IWorker
{
private:
	HANDLE m_thread;
	bool _bisy = false;
	bool _end = false;

public:
	IWorker()
	{
	}

	~IWorker()
	{
		WaitForSingleObject(m_thread, INFINITE);
	}

	bool ExecuteTask(ITask* taskToRun);
	bool IsBusy();
};

