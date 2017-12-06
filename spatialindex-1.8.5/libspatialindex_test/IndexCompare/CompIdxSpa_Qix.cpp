#include "stdafx.h"
#include "CompIndex_Spatial.h"

#include "shapefil.h"
#include <io.h>

// 内存模式:创建索引+查询数据
void TestQix_1()
{
    // 0.要写日志的内容及初始化
    string strTime(""),strMethod(""),strMode("");
    long lInsert(0),lQuery(0);
    double dMemory_MB(0.0);
    int iQueryCnt(0);
    strTime = getCurTime();
    strMethod = "Qix";
    strMode = "Memory";
    clock_t tBeg,tEnd;
    SIZE_T mBeg,mEnd;
    HANDLE hCurrent = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;

    // 1.打开数据
    string strShpPath = g_strDataDir + g_strDataset + ".shp";
    SHPHandle hSHP = SHPOpen(strShpPath.c_str(), "rb");
    SHPSetFastModeReadObject(hSHP, true);

    // 2.创建索引
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mBeg = pmc.PagefileUsage;
    tBeg = clock();
    SHPTree *hTree = SHPCreateTree(
        hSHP, 2, MAX_DEFAULT_TREE_DEPTH,
        hSHP->adBoundsMin, hSHP->adBoundsMax);
    tEnd = clock();
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mEnd = pmc.PagefileUsage;
    lInsert = tEnd-tBeg;
    dMemory_MB = (mEnd-mBeg)/1024.0/1024.0;

    // 3.查询
    double adfBoundsMin[4] = {g_x_min,g_y_min,0,0};
    double adfBoundsMax[4] = {g_x_max,g_y_max,0,0};
    tBeg = clock();
    for (int iTime=0; iTime<g_iQueryTimes; ++iTime)
    {
        int *piIndex = NULL;
        piIndex = SHPTreeFindLikelyShapes(hTree, adfBoundsMin, adfBoundsMax, &iQueryCnt);
        free(piIndex);
    }
    tEnd = clock();
    lQuery = tEnd-tBeg;

    // 4.清理
    SHPDestroyTree(hTree);
    SHPClose(hSHP);

    // 5.日志
    fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
        strTime.c_str(), strMethod.c_str(), strMode.c_str(),
        lInsert, dMemory_MB, lQuery, iQueryCnt);
    fflush(g_pLog_Spa);
}

// 硬盘模式:创建索引+查询数据
void TestQix_2()
{
    // 0.要写日志的内容及初始化
    string strTime(""),strMethod(""),strMode("");
    long lInsert(0),lQuery(0);
    double dMemory_MB(0.0);
    int iQueryCnt(0);
    strTime = getCurTime();
    strMethod = "Qix";
    strMode = "Disk";
    clock_t tBeg,tEnd;
    SIZE_T mBeg,mEnd;
    HANDLE hCurrent = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;

    string strShpPath = g_strDataDir + g_strDataset + ".shp";
    string strTreePath = g_strSpaWorkDir + g_strDataset + ".qix";
    // 1.清理索引
    {
        if (0 == access(strTreePath.c_str(), 00))
            remove(strTreePath.c_str());
    }

    // 2.创建索引
    SHPHandle hSHP = SHPOpen(strShpPath.c_str(), "rb");
    SHPSetFastModeReadObject(hSHP, true);

    tBeg = clock();
    SHPTree *hTree = SHPCreateTree(
        hSHP, 2, MAX_DEFAULT_TREE_DEPTH,
        hSHP->adBoundsMin, hSHP->adBoundsMax);
    SHPWriteTree(hTree, strTreePath.c_str());
    tEnd = clock();
    lInsert = tEnd-tBeg;

    SHPDestroyTree(hTree);
    SHPClose(hSHP);

    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mBeg = pmc.PagefileUsage;
    SHPTreeDiskHandle hQIX = SHPOpenDiskTree(strTreePath.c_str(), NULL);
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
        piIndex = SHPSearchDiskTreeEx(hQIX, adfBoundsMin, adfBoundsMax, &iQueryCnt);
        free(piIndex);
    }
    tEnd = clock();
    lQuery = tEnd-tBeg;

    // 4.清理
    SHPCloseDiskTree(hQIX);

    // 5.日志
    fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
        strTime.c_str(), strMethod.c_str(), strMode.c_str(),
        lInsert, dMemory_MB, lQuery, iQueryCnt);
    fflush(g_pLog_Spa);
}
