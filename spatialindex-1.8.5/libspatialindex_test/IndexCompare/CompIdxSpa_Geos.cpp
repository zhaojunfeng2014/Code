#include "stdafx.h"
#include "CompIndex_Spatial.h"

#include "geos.h"
#include "geos/index/SpatialIndex.h"
#include "geos/index/ItemVisitor.h"
#include "geos/index/strtree/STRtree.h"
#include "geos/index/quadtree/Quadtree.h"
using namespace geos;
using namespace geos::index;
using namespace geos::index::strtree;
using namespace geos::index::quadtree;

#include "shapefil.h"

int     g_nodeCapacity  = 64;

class GeoVisitor : public ItemVisitor
{
public:
    GeoVisitor()
    {
        m_iCount = 0;
    }

public:
    int     GetQueryCount(){return m_iCount;}
public:
    virtual void visitItem(void *item)
    {
        ++m_iCount;
    }

private:
    int     m_iCount;
};


// 内存模式:STRree:创建索引+查询数据
void TestGeos_1()
{
    // 0.要写日志的内容及初始化
    string strTime(""),strMethod(""),strMode("");
    long lInsert(0),lQuery(0);
    double dMemory_MB(0.0);
    int iQueryCnt(0);
    strTime = getCurTime();
    strMethod = "Geos-STRTree";
    strMode = "Memory";
    clock_t tBeg,tEnd;
    SIZE_T mBeg,mEnd;
    HANDLE hCurrent = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;

    // 1.打开数据
    string strShpPath = g_strDataDir + g_strDataset + ".shp";
    SHPHandle hSHP = SHPOpen(strShpPath.c_str(), "rb");
    SHPSetFastModeReadObject(hSHP, true);
    int nEntities(0),nShapeType(0);
    double padfMinBound[4],padfMaxBound[4];
    SHPGetInfo(hSHP, &nEntities, &nShapeType, padfMinBound, padfMaxBound);

    // 2.创建索引
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mBeg = pmc.PagefileUsage;
    tBeg = clock();

    list<Envelope*> lstEnvelopes;
    Envelope *pEnvelope = NULL;
    STRtree tree(g_nodeCapacity);
    for (int i=0; i<nEntities; ++i)
    {
        SHPObject *pObj = SHPReadObject(hSHP, i);

        pEnvelope = new Envelope(pObj->dfXMin,pObj->dfXMax, pObj->dfYMin, pObj->dfYMax);
        tree.insert(pEnvelope, NULL);
        lstEnvelopes.push_back(pEnvelope);

        SHPDestroyObject(pObj);
    }

    tEnd = clock();
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mEnd = pmc.PagefileUsage;
    lInsert = tEnd-tBeg;
    dMemory_MB = (mEnd-mBeg)/1024.0/1024.0;

    // 3.查询
    tBeg = clock();
    for (int iVal=0; iVal<g_iQueryTimes; ++iVal)
    {
        Envelope queryEvnlp(g_x_min,g_x_max, g_y_min,g_y_max);

        GeoVisitor visitor;
        tree.query(&queryEvnlp, visitor);
        iQueryCnt = visitor.GetQueryCount();
    }
    tEnd = clock();
    lQuery = tEnd-tBeg;

    // 4.清理
    for (list<Envelope*>::iterator iter=lstEnvelopes.begin(); iter!=lstEnvelopes.end(); ++iter)
    {
        delete (*iter);
        (*iter) = NULL;
    }
    lstEnvelopes.clear();

    // 5.日志
    fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
        strTime.c_str(), strMethod.c_str(), strMode.c_str(),
        lInsert, dMemory_MB, lQuery, iQueryCnt);
    fflush(g_pLog_Spa);
}

// 内存模式:QuadTree:创建索引+查询数据
void TestGeos_2()
{
    // 0.要写日志的内容及初始化
    string strTime(""),strMethod(""),strMode("");
    long lInsert(0),lQuery(0);
    double dMemory_MB(0.0);
    int iQueryCnt(0);
    strTime = getCurTime();
    strMethod = "Geos-QuadTree";
    strMode = "Memory";
    clock_t tBeg,tEnd;
    SIZE_T mBeg,mEnd;
    HANDLE hCurrent = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;

    // 1.打开数据
    string strShpPath = g_strDataDir + g_strDataset + ".shp";
    SHPHandle hSHP = SHPOpen(strShpPath.c_str(), "rb");
    SHPSetFastModeReadObject(hSHP, true);
    int nEntities(0),nShapeType(0);
    double padfMinBound[4],padfMaxBound[4];
    SHPGetInfo(hSHP, &nEntities, &nShapeType, padfMinBound, padfMaxBound);

    // 2.创建索引
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mBeg = pmc.PagefileUsage;
    tBeg = clock();

    list<Envelope*> lstEnvelopes;
    Envelope *pEnvelope = NULL;
    Quadtree tree;
    for (int i=0; i<nEntities; ++i)
    {
        SHPObject *pObj = SHPReadObject(hSHP, i);

        pEnvelope = new Envelope(pObj->dfXMin,pObj->dfXMax, pObj->dfYMin, pObj->dfYMax);
        tree.insert(pEnvelope, NULL);
        lstEnvelopes.push_back(pEnvelope);

        SHPDestroyObject(pObj);
    }

    tEnd = clock();
    GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
    mEnd = pmc.PagefileUsage;
    lInsert = tEnd-tBeg;
    dMemory_MB = (mEnd-mBeg)/1024.0/1024.0;

    // 3.查询
    tBeg = clock();
    for (int iVal=0; iVal<g_iQueryTimes; ++iVal)
    {
        Envelope queryEvnlp(g_x_min,g_x_max, g_y_min,g_y_max);

        GeoVisitor visitor;
        tree.query(&queryEvnlp, visitor);
        iQueryCnt = visitor.GetQueryCount();
    }
    tEnd = clock();
    lQuery = tEnd-tBeg;

    // 4.清理
    for (list<Envelope*>::iterator iter=lstEnvelopes.begin(); iter!=lstEnvelopes.end(); ++iter)
    {
        delete (*iter);
        (*iter) = NULL;
    }
    lstEnvelopes.clear();

    // 5.日志
    fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
        strTime.c_str(), strMethod.c_str(), strMode.c_str(),
        lInsert, dMemory_MB, lQuery, iQueryCnt);
    fflush(g_pLog_Spa);
}
