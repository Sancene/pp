#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include "BMPFILE.h"
#include "BMPHelper.h"
#include "LogBuffer.h"

struct Params
{
	clock_t startTime;
	int threadNumber;
	std::ofstream* fout;
	BMPFILE* bmp;
	BMPFILE* blured;
	LogBuffer* logbuffer;
	int start;
	int end;
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	struct Params* params = (struct Params*)lpParam;
	BMPHelper::BlurByWidth(&*params->bmp, &*params->blured, params->start, params->end, &*params->fout, params->startTime, params->threadNumber, params->logbuffer);
	ExitThread(0);
}

void WriteHowTo()
{
	std::cout << "Example: "
		<< "lw4.exe" << " "
		<< "input.bmp" << " "
		<< "output.bmp" << " "
		<< "threadCount" << " "
		<< "coreCount" << " ";
}

int main(int argc, char* argv[])
{
	clock_t start = clock();

	std::string bmpFileName = "../image.bmp";
	std::string bluredFileName = "../imagedone.bmp";

	std::vector<std::ofstream> outFiles;

	BMPFILE bmp = BMPHelper::ReadFromFile(bmpFileName);
	LogBuffer logBuffer;

	if (bmp.GetPixels().size() == 0)
	{
		return -1;
	}

	BMPFILE blured = BMPFILE(bmp);

	int threadsCount = 3;
	int coreCount = 2;
	uint32_t width = (bmp.GetWidth() - 1) / threadsCount;
	std::vector<int> priority;
	for (int i = 0; i < threadsCount; i++)
	{
		outFiles.push_back(std::ofstream("output" + std::to_string(i) + ".txt"));
	}

	std::vector<Params> paramsToThread;

	int minWidth = 0;

	//THREAD_PRIORITY_ABOVE_NORMAL THREAD_PRIORITY_BELOW_NORMAL THREAD_PRIORITY_HIGHEST THREAD_PRIORITY_IDLE THREAD_PRIORITY_LOWEST THREAD_PRIORITY_NORMAL THREAD_PRIORITY_TIME_CRITICAL

	std::vector<int> prio = { THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_BELOW_NORMAL };

	for (int i = 0; i < threadsCount; i++)
	{
		Params params;
		params.startTime = start;
		params.threadNumber = i;
		params.fout = &outFiles[i];
		params.bmp = &bmp;
		params.blured = &blured;
		params.start = minWidth;
		params.end = i == threadsCount - 1 ? bmp.GetWidth() - 1 : minWidth + width;
		params.logbuffer = &logBuffer;
		paramsToThread.push_back(params);
		minWidth += width;
	}

	HANDLE* handles = new HANDLE[threadsCount];

	for (size_t i = 0; i < threadsCount; i++)
	{
		handles[i] = CreateThread(NULL, i, &ThreadProc, &paramsToThread[i], CREATE_SUSPENDED, NULL);
		SetThreadAffinityMask(handles[i], (1 << coreCount) - 1);
		SetThreadPriority(handles[i], prio[i]);
	}

	for (int i = 0; i < threadsCount; i++)
	{
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(threadsCount, handles, true, INFINITE);

	BMPHelper::WriteBMPFile(&blured, bluredFileName);
}