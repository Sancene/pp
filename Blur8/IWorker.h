#pragma once
#include <windows.h>
#include <iostream>
#include "ITask.h"
#include <vector>

extern CRITICAL_SECTION CriticalSection;

const int MODE0 = 0;
const int MODE1 = 1;

class IWorker
{
private:
	std::vector<HANDLE*> _handles;
	bool _bisy = false;
	bool _end = false;

	int _mode;
	int _maxThreads;

public:
	IWorker(int mode, int maxAmountOfThreads = 1):
		_mode(mode), _maxThreads(maxAmountOfThreads)
	{
		InitializeCriticalSectionAndSpinCount(&CriticalSection, 0x00000400);
	}

	~IWorker()
	{
		DeleteCriticalSection(&CriticalSection);
	}

	bool CreateTask(ITask* taskToRun);
	void ResumeTasks();

	bool IsBusy();
};

