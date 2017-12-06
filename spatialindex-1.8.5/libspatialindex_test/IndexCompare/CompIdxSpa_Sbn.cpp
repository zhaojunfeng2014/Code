#include "stdafx.h"
#include "CompIndex_Spatial.h"

#include "shapefil.h"

// 硬盘模式:查询数据
void TestSbn_1()
{
    // 0.要写日志的内容及初始化
    string strTime(""),strMethod(""),strMode("");
    long lInsert(0),lQuery(0);
    double dMemory_MB(0.0);
    int iQueryCnt(0);
    strTime = getCurTime();
    strMethod = "Sbn";
    strMode = "Disk";
    clock_t tBeg,tEnd;
    SIZE_T mBeg,mEnd;
    HANDLE hCurrent = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;

    string strShpPath = g_strDataDir + g_strDataset + ".shp";
    string strTreePath = g_strDataDir + g_strDataset + ".sbn";
    // 1.打开索引
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mBeg = pmc.PagefileUsage;
    SBNSearchHandle hSBN = SBNOpenDiskTree(strTreePath.c_str(),NULL);
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mEnd = pmc.PagefileUsage;
    dMemory_MB = (mEnd-mBeg)/1024.0/1024.0;

    // 3.查询
    double adfBoundsMin[4] = {g_x_min,g_y_min,0,0};
    double adfBoundsMax[4] = {g_x_max,g_y_max,0,0};
    tBeg = clock();
    for (int iTime=0; iTime<g_iQueryTimes; ++iTime)
    {
        int *piIndex = NULL;
        piIndex = SBNSearchDiskTree(hSBN, adfBoundsMin, adfBoundsMax, &iQueryCnt);
        SBNSearchFreeIds(piIndex);
    }
    tEnd = clock();
    lQuery = tEnd-tBeg;

    // 4.清理
    SBNCloseDiskTree(hSBN);

    // 5.日志
    fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
        strTime.c_str(), strMethod.c_str(), strMode.c_str(),
        lInsert, dMemory_MB, lQuery, iQueryCnt);
    fflush(g_pLog_Spa);
}
