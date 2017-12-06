#include "stdafx.h"
#include "CompIndex_Spatial.h"

#include "shapefil.h"

#include <vector>
#include <list>
using namespace std;

struct sEnvelope
{
    sEnvelope(size_t i, double x1, double x2, double y1, double y2)
    {
        id = i;
        x_min = x1;
        x_max = x2;
        y_min = y1;
        y_max = y2;
    }
    size_t id;
    double x_min;
    double x_max;
    double y_min;
    double y_max;
};

// 内存模式:vector:循环所有数据
void TestLoop_1()
{
    typedef vector<sEnvelope*>  Envelopes;
    Envelopes envlps;

    // 0.要写日志的内容及初始化
    string strTime(""),strMethod(""),strMode("");
    long lInsert(0),lQuery(0);
    double dMemory_MB(0.0);
    int iQueryCnt(0);
    strTime = getCurTime();
    strMethod = "Loop-vector";
    strMode = "Memory";
    clock_t tBeg,tEnd;
    SIZE_T mBeg,mEnd;
    HANDLE hCurrent = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;

    // 1.内存初始化
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mBeg = pmc.PagefileUsage;
    tBeg = clock();

    string strShpPath = g_strDataDir + g_strDataset + ".shp";
    SHPHandle hSHP = SHPOpen(strShpPath.c_str(), "rb");
    SHPSetFastModeReadObject(hSHP, true);
    int nEntities(0),nShapeType(0);
    double padfMinBound[4],padfMaxBound[4];
    SHPGetInfo(hSHP, &nEntities, &nShapeType, padfMinBound, padfMaxBound);
    for (int i=0; i<nEntities; ++i)
    {
        SHPObject *pObj = SHPReadObject(hSHP, i);

        sEnvelope *pEnvlp = new sEnvelope(i, pObj->dfXMin,pObj->dfXMax, pObj->dfYMin,pObj->dfYMax);
        envlps.push_back(pEnvlp);

        SHPDestroyObject(pObj);
    }
    SHPClose(hSHP);

    tEnd = clock();
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mEnd = pmc.PagefileUsage;
    lInsert = tEnd-tBeg;
    dMemory_MB = (mEnd-mBeg)/1024.0/1024.0;

    // 2.查询
    tBeg = clock();
    for (int iTime=0; iTime<g_iQueryTimes; ++iTime)
    {
        iQueryCnt = 0;

        for (Envelopes::iterator iter=envlps.begin(); iter!=envlps.end(); ++iter)
        {
            if ((*iter)->x_min <= g_x_max &&
                (*iter)->x_max >= g_x_min &&
                (*iter)->y_min <= g_y_max &&
                (*iter)->y_max >= g_y_min)
            {
                ++iQueryCnt;
            }
        }
    }
    tEnd = clock();
    lQuery = tEnd-tBeg;

    // 3.清理
    for (Envelopes::iterator iter=envlps.begin(); iter!=envlps.end(); ++iter)
    {
        delete (*iter);
        (*iter) = NULL;
    }
    envlps.clear();

    // 4.日志
    fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
        strTime.c_str(), strMethod.c_str(), strMode.c_str(),
        lInsert, dMemory_MB, lQuery, iQueryCnt);
    fflush(g_pLog_Spa);
}

// 内存模式:list:循环所有数据
void TestLoop_2()
{
    typedef list<sEnvelope*>  Envelopes;
    Envelopes envlps;

    // 0.要写日志的内容及初始化
    string strTime(""),strMethod(""),strMode("");
    long lInsert(0),lQuery(0);
    double dMemory_MB(0.0);
    int iQueryCnt(0);
    strTime = getCurTime();
    strMethod = "Loop-list";
    strMode = "Memory";
    clock_t tBeg,tEnd;
    SIZE_T mBeg,mEnd;
    HANDLE hCurrent = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;

    // 1.内存初始化
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mBeg = pmc.PagefileUsage;
    tBeg = clock();

    string strShpPath = g_strDataDir + g_strDataset + ".shp";
    SHPHandle hSHP = SHPOpen(strShpPath.c_str(), "rb");
    SHPSetFastModeReadObject(hSHP, true);
    int nEntities(0),nShapeType(0);
    double padfMinBound[4],padfMaxBound[4];
    SHPGetInfo(hSHP, &nEntities, &nShapeType, padfMinBound, padfMaxBound);
    for (int i=0; i<nEntities; ++i)
    {
        SHPObject *pObj = SHPReadObject(hSHP, i);

        sEnvelope *pEnvlp = new sEnvelope(i, pObj->dfXMin,pObj->dfXMax, pObj->dfYMin,pObj->dfYMax);
        envlps.push_back(pEnvlp);

        SHPDestroyObject(pObj);
    }
    SHPClose(hSHP);

    tEnd = clock();
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mEnd = pmc.PagefileUsage;
    lInsert = tEnd-tBeg;
    dMemory_MB = (mEnd-mBeg)/1024.0/1024.0;

    // 2.查询
    tBeg = clock();
    for (int iTime=0; iTime<g_iQueryTimes; ++iTime)
    {
        iQueryCnt = 0;

        for (Envelopes::iterator iter=envlps.begin(); iter!=envlps.end(); ++iter)
        {
            if ((*iter)->x_min <= g_x_max &&
                (*iter)->x_max >= g_x_min &&
                (*iter)->y_min <= g_y_max &&
                (*iter)->y_max >= g_y_min)
            {
                ++iQueryCnt;
            }
        }
    }
    tEnd = clock();
    lQuery = tEnd-tBeg;

    // 3.清理
    for (Envelopes::iterator iter=envlps.begin(); iter!=envlps.end(); ++iter)
    {
        delete (*iter);
        (*iter) = NULL;
    }
    envlps.clear();

    // 4.日志
    fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
        strTime.c_str(), strMethod.c_str(), strMode.c_str(),
        lInsert, dMemory_MB, lQuery, iQueryCnt);
    fflush(g_pLog_Spa);
}

// 硬盘模式:循环所有数据
void TestLoop_3()
{
    // 0.要写日志的内容及初始化
    string strTime(""),strMethod(""),strMode("");
    long lInsert(0),lQuery(0);
    double dMemory_MB(0.0);
    int iQueryCnt(0);
    strTime = getCurTime();
    strMethod = "Loop";
    strMode = "Disk";
    clock_t tBeg,tEnd;
    //SIZE_T mBeg,mEnd;
    //HANDLE hCurrent = GetCurrentProcess();
    //PROCESS_MEMORY_COUNTERS pmc;

    // 1.打开文件
    string strShpPath = g_strDataDir + g_strDataset + ".shp";
    SHPHandle hSHP = SHPOpen(strShpPath.c_str(), "rb");
    SHPSetFastModeReadObject(hSHP, true);
    int nEntities(0),nShapeType(0);
    double padfMinBound[4],padfMaxBound[4];
    SHPGetInfo(hSHP, &nEntities, &nShapeType, padfMinBound, padfMaxBound);

    // 2.查询
    tBeg = clock();
    for (int iTime=0; iTime<g_iQueryTimes; ++iTime)
    {
        iQueryCnt = 0;

        for (int i=0; i<nEntities; ++i)
        {
            SHPObject *pObj = SHPReadObject(hSHP, i);
            if (pObj->dfXMin <= g_x_max &&
                pObj->dfXMax >= g_x_min &&
                pObj->dfYMin <= g_y_max &&
                pObj->dfYMax >= g_y_min)
            {
                ++iQueryCnt;
            }
            SHPDestroyObject(pObj);
        }
    }
    tEnd = clock();
    lQuery = tEnd-tBeg;

    // 3.清理
    SHPClose(hSHP);

    // 4.日志
    fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
        strTime.c_str(), strMethod.c_str(), strMode.c_str(),
        lInsert, dMemory_MB, lQuery, iQueryCnt);
    fflush(g_pLog_Spa);
}
