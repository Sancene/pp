#include <windows.h>
#include <string>
#include <iostream>
#include "ITask.h"
#include "MyTask.h"
#include "IWorker.h"

CRITICAL_SECTION CriticalSection;

int main(int argc, char* argv[])
{
    HANDLE hProcess = GetCurrentProcess();
    SetProcessAffinityMask(hProcess, 4);
    IWorker worker;
    const int number = 20;
    ITask* my_class[number];
    for (int i = 0; i < number; i++)
    {
        my_class[i] = new MyTask(i);
        Sleep(0.01);
        worker.ExecuteTask(my_class[i]);
    }

    return 0;
}
