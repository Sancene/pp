#include "IWorker.h"

extern CRITICAL_SECTION CriticalSection;

struct IWorkerStruct
{
	ITask& task;
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	IWorkerStruct* refinedData = (IWorkerStruct*)lpParam;
	ITask& task = refinedData->task;

	EnterCriticalSection(&CriticalSection);
	task.Execute();
	LeaveCriticalSection(&CriticalSection);

	ExitThread(0);
}

bool IWorker::CreateTask(ITask* taskToRun)
{
	if (_end)
	{
		return false;
	}

	IWorkerStruct* dataToSend = new IWorkerStruct{ *taskToRun };
	HANDLE* newHandle = new HANDLE(CreateThread(NULL, 0, &ThreadProc, (LPVOID)(dataToSend), CREATE_SUSPENDED, NULL));

	_handles.push_back(newHandle);

	return true;
}

void IWorker::ResumeTasks()
{
	int freeThreads = 0;

	for (auto i = _handles.begin(); i != _handles.end(); i++)
	{
		if (_mode == MODE0)
		{
			ResumeThread(*i[0]);
			freeThreads++;
		}
		else if (_mode == MODE1)
		{
			if (freeThreads <= _maxThreads)
			{
				freeThreads++;
				ResumeThread(*i[0]);
			}
			else
			{
				auto waitForFreeThread = i;
				for (int j = 0; j < _maxThreads; j++) waitForFreeThread--;
				WaitForMultipleObjects(1, *waitForFreeThread, true, INFINITE);
				ResumeThread(*i[0]);

			}
		}
	}

	for (int i = 0; i < freeThreads; i++)
	{
		auto waitForThisHandle = _handles.end();
		for (int j = 0; j <= i; j++) waitForThisHandle--;
		WaitForMultipleObjects(1, *waitForThisHandle, true, INFINITE);
	}
}