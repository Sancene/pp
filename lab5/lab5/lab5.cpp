#include <windows.h>
#include <string>
#include <iostream>

CRITICAL_SECTION CriticalSection;

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	EnterCriticalSection(&CriticalSection);

	int* working_variable = (int*)lpParam;
	int variable = *working_variable;

	for (int i = 0; i < 5; i++) {
		int j = *working_variable;
		int k = variable;
		int a = rand() % 10;
		*working_variable = j + a;
		variable = k + a;
	}

	std::string out = std::to_string(variable) + " " + std::to_string(*working_variable) + "\n";
	std::cout << out;

	LeaveCriticalSection(&CriticalSection);

	ExitThread(0);
}


int main(int argc)
{
	int working_variable = 0;

	if (!InitializeCriticalSectionAndSpinCount(&CriticalSection,
		0x00000400))
		return 0;

	HANDLE* handles = new HANDLE[2];

	for (int i = 0; i < 2; i++) {
		handles[i] = CreateThread(NULL, i, &ThreadProc, &working_variable, CREATE_SUSPENDED, NULL);
	}

	for (int i = 0; i < 2; i++) {
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(2, handles, true, INFINITE);

	std::cout << working_variable << std::endl;

	DeleteCriticalSection(&CriticalSection);

	return 0;
}