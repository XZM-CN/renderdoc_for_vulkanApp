#include "FPSUtil.h"
#include <time.h>
#include <chrono>
#include <thread>
#include <windows.h>
#include <winbase.h>
int FPSUtil::FPSCount;
long long FPSUtil::startTime;
float FPSUtil::currFPS;
long long FPSUtil::beforeTime;
long long currentTimeMillis()
{
    long long time = GetTickCount();
    return time;
}
void FPSUtil::init()
{
    FPSCount  = 0;
    startTime = currentTimeMillis();
}
void FPSUtil::calFPS()
{
    FPSCount++;
    if (FPSCount == 100)
    {
        FPSCount          = 0;
        long long endTime = currentTimeMillis();
        currFPS           = (float)(1000.0 / ((endTime - startTime) / 100.0));
        startTime         = endTime;
        printf("FPS: %f\n", FPSUtil::currFPS);
    }
}
void FPSUtil::before()
{
    beforeTime = currentTimeMillis();
}
void FPSUtil::after(int dstFPS)
{
    int dstSpan    = (int)(1000 / dstFPS) + 1;
    long long span = currentTimeMillis() - beforeTime;
    if (span < dstSpan)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(dstSpan - span));
    }
}
