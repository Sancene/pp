#include <windows.h>
#include <string>
#include <iostream>
#include <filesystem>
#include "ITask.h"
#include "MyTask.h"
#include "IWorker.h"

CRITICAL_SECTION CriticalSection;

int main(int argc, char* argv[])
{
    clock_t currentTime;
    currentTime = clock();
    int mode = atoi(argv[1]);
    int blocksCount = atoi(argv[2]);
    std::string in_directory = argv[3];
    std::string out_directory = argv[4];
    int threadsInPoolCount = atoi(argv[5]);

    IWorker worker(mode, threadsInPoolCount);

    using directory_iterator = std::filesystem::directory_iterator;

    std::vector<bitmap*> allOuts;

    for (const auto& dirEntry : directory_iterator(in_directory))
    {
        std::cout << (dirEntry.path().string()).c_str() << std::endl;
        bitmap* out = new bitmap((out_directory + "\\" + dirEntry.path().filename().string()).c_str());
        out = new bitmap((dirEntry.path().string()).c_str());
        out->save((out_directory + "\\" + dirEntry.path().filename().string()).c_str());
        allOuts.push_back(out);

        for (int i = 0; i < blocksCount; i++)
        {
            ITask* my_class = new MyTask((dirEntry.path().string()).c_str(), (out_directory + "\\" + dirEntry.path().filename().string()).c_str(), i + 1, blocksCount, out);
            worker.CreateTask(my_class);
        }
    }
    
    worker.ResumeTasks();
    std::cout << clock() - currentTime << "\n";

    return 0;
}
