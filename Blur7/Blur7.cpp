#include <windows.h>
#include <string>
#include <iostream>
#include "ITask.h"
#include "MyTask.h"
#include "IWorker.h"

CRITICAL_SECTION CriticalSection;

int main(int argc, char* argv[])
{
    IWorker worker;
    const int number = 10;
    ITask* my_class[number];

    for (int i = 0; i < number; i++)
    {
        my_class[i] = new MyTask(i);
        std::cout << worker.ExecuteTask(my_class[i]) << std::endl;
        //worker.~IWorker();
    }

    return 0;
}
