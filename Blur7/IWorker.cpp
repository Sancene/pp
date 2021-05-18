#include "IWorker.h"

extern CRITICAL_SECTION CriticalSection;

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	//IWorkerStruct* refinedData = (IWorkerStruct*)lpParam;
	auto task = (ITask*)lpParam;

	//EnterCriticalSection(&CriticalSection);
	task->Execute();
	//LeaveCriticalSection(&CriticalSection);
	ExitThread(0);
}

bool IWorker::ExecuteTask(ITask* taskToRun)
{
	if (IsBusy())
	{
		std::cout << "busy\n";
		return false;
	}
	if(m_thread = CreateThread(NULL, 0, &ThreadProc, taskToRun, CREATE_SUSPENDED, NULL))
	{
		ResumeThread(m_thread);
		return false;
	}
	return false;
}

bool IWorker::IsBusy()
{
	DWORD exitcode;
	GetExitCodeThread(m_thread, &exitcode);
	return exitcode == STILL_ACTIVE;
}

