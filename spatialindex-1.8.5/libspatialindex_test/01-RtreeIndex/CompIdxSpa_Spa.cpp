//#include "stdafx.h"
#include <windows.h>
#include "CompIndex_Spatial.h"

#include "spatialindex/SpatialIndex.h"
using namespace SpatialIndex;
#include <math.h>

#include "shapefil.h"
#include <io.h>


uint32_t get_pagesize()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwPageSize;
}

uint32_t get_nodesize()
{
	//cout << sizeof(RTree::Data) << endl; //96
	return (uint32_t)powf(2.0f, int(logf(sizeof(RTree::Data)) / logf(2.0f) + 1));
}

uint32_t get_nodecapa()
{
	return get_pagesize() / get_nodesize();
}


long        g_lQueryCnt = 0;

double      g_fillFactor = 0.7;
uint32_t    g_pageSize = get_pagesize();
uint32_t    g_randomCapacity = 10;
uint32_t    g_indexCapacity = get_nodecapa();
uint32_t    g_leafCapacity = get_nodecapa();

class SpaVisitor : public IVisitor
{
public:
	virtual void visitNode(const INode& n) {}

	virtual void visitData(const IData& d)
	{
		++g_lQueryCnt;
	}

	virtual void visitData(std::vector<const IData*>& v) {}
};


// 内存模式:RTree:创建索引+查询数据
void TestSpa_1()
{
	// 0.要写日志的内容及初始化
	string strTime(""), strMethod(""), strMode("");
	long lInsert(0), lQuery(0);
	double dMemory_MB(0.0);
	int iQueryCnt(0);
	strTime = getCurTime();
	strMethod = "SpatialIndex-RTree";
	strMode = "Memory";
	clock_t tBeg, tEnd;
	SIZE_T mBeg, mEnd;
	HANDLE hCurrent = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;

	// 1.打开数据
	string strShpPath = g_strDataDir + g_strDataset + ".shp";
	SHPHandle hSHP = SHPOpen(strShpPath.c_str(), "rb");
	SHPSetFastModeReadObject(hSHP, true);
	int nEntities(0), nShapeType(0);
	double padfMinBound[4], padfMaxBound[4];
	SHPGetInfo(hSHP, &nEntities, &nShapeType, padfMinBound, padfMaxBound);

	// 2.创建索引
	GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
	mBeg = pmc.PagefileUsage;
	tBeg = clock();

	id_type indexIdentifier;
	IStorageManager* store = StorageManager::createNewMemoryStorageManager();
	ISpatialIndex* tree = RTree::createNewRTree(*store, g_fillFactor, g_indexCapacity, g_leafCapacity, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);

	id_type id;
	double low[2], high[2];
	for (int i = 0; i < nEntities; ++i)
	{
		SHPObject *pObj = SHPReadObject(hSHP, i);

		id = i;
		low[0] = pObj->dfXMin;
		low[1] = pObj->dfYMin;
		high[0] = pObj->dfXMax;
		high[1] = pObj->dfYMax;
		Region r = Region(low, high, 2);

		tree->insertData(0, NULL, r, id);

		SHPDestroyObject(pObj);
	}

	tEnd = clock();
	GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
	mEnd = pmc.PagefileUsage;
	lInsert = tEnd - tBeg;
	dMemory_MB = (mEnd - mBeg) / 1024.0 / 1024.0;

	// 3.查询
	low[0] = g_x_min;
	low[1] = g_y_min;
	high[0] = g_x_max;
	high[1] = g_y_max;
	tBeg = clock();
	for (int iVal = 0; iVal < g_iQueryTimes; ++iVal)
	{
		g_lQueryCnt = 0;
		Region r = Region(low, high, 2);
		SpaVisitor vis;
		tree->intersectsWithQuery(r, vis);
	}
	tEnd = clock();
	lQuery = tEnd - tBeg;
	iQueryCnt = g_lQueryCnt;

	// 4.清理
	delete tree;
	delete store;
	SHPClose(hSHP);

	// 5.日志
	fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
		strTime.c_str(), strMethod.c_str(), strMode.c_str(),
		lInsert, dMemory_MB, lQuery, iQueryCnt);
	fflush(g_pLog_Spa);
}


class ShpDataStream : public IDataStream
{
public:
	ShpDataStream(std::string shpPath) : m_pNext(0)
	{
		m_hSHP = SHPOpen(shpPath.c_str(), "rb");
		if (NULL == m_hSHP)
			throw Tools::IllegalArgumentException("Can not Open Shp!");
		SHPSetFastModeReadObject(m_hSHP, true);

		int nEntities(0), nShapeType(0);
		double padfMinBound[4], padfMaxBound[4];
		SHPGetInfo(m_hSHP, &nEntities, &nShapeType, padfMinBound, padfMaxBound);

		m_iTotal = nEntities;
		m_iCursor = 0;
		readNextEntry();
	}

	virtual ~ShpDataStream()
	{
		if (NULL != m_pNext)
			delete m_pNext;
		SHPClose(m_hSHP);
	}

	virtual IData* getNext()
	{
		if (NULL == m_pNext)
			return NULL;

		RTree::Data* ret = m_pNext;
		m_pNext = NULL;
		readNextEntry();
		return ret;
	}

	virtual bool hasNext()
	{
		return (NULL != m_pNext);
	}

	virtual uint32_t size()
	{
		return m_iTotal;
	}

	virtual void rewind()
	{
		if (NULL != m_pNext)
		{
			delete m_pNext;
			m_pNext = NULL;
		}

		m_iCursor = 0;
		readNextEntry();
	}

	void readNextEntry()
	{
		if (m_iCursor < m_iTotal)
		{
			SHPObject *pObj = SHPReadObject(m_hSHP, m_iCursor);
			double low[2], high[2];
			low[0] = pObj->dfXMin;
			low[1] = pObj->dfYMin;
			high[0] = pObj->dfXMax;
			high[1] = pObj->dfYMax;

			int nPointCnt = pObj->nVertices;
			double* pVertics = new double[3 * nPointCnt];
			for (int i = 0; i < nPointCnt; i++)
			{
				pVertics[3 * i] = pObj->padfX[i];
				pVertics[3 * i + 1] = pObj->padfY[i];
				pVertics[3 * i + 2] = pObj->padfY[i];
			}

			Region r(low, high, 2);

			//m_pNext = new RTree::Data(3 * nPointCnt * sizeof(double) + 1, (byte*)pVertics, r, m_iCursor); //会坐标数据复制数据
			//m_pNext = new RTree::Data(0, NULL, r, m_iCursor); //会坐标数据复制数据
			m_pNext = new RTree::Data(3 * nPointCnt * sizeof(double), (byte*)pVertics, r, m_iCursor); //会坐标数据复制数据


			delete[] pVertics;
			SHPDestroyObject(pObj);
		}
		++m_iCursor;
	}

	SHPHandle m_hSHP;
	RTree::Data* m_pNext;
	int m_iTotal;
	int m_iCursor;
};

// 内存模式:RTree-BulkLoad:创建索引+查询数据
void TestSpa_2()
{
	// 0.要写日志的内容及初始化
	string strTime(""), strMethod(""), strMode("");
	long lInsert(0), lQuery(0);
	double dMemory_MB(0.0);
	int iQueryCnt(0);
	strTime = getCurTime();
	strMethod = "SpatialIndex-RTree";
	strMode = "Memory-BulkLoad";
	clock_t tBeg, tEnd;
	SIZE_T mBeg, mEnd;
	HANDLE hCurrent = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;

	// 1.创建索引
	GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
	mBeg = pmc.PagefileUsage;
	tBeg = clock();

	string strShpPath = g_strDataDir + g_strDataset + ".shp";
	ShpDataStream stream(strShpPath);
	id_type indexIdentifier;
	IStorageManager* store = StorageManager::createNewMemoryStorageManager();
	ISpatialIndex* tree = RTree::createAndBulkLoadNewRTree(
		RTree::BLM_STR, stream, *store, g_fillFactor, g_indexCapacity, g_leafCapacity, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);

	tEnd = clock();
	GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
	mEnd = pmc.PagefileUsage;
	lInsert = tEnd - tBeg;
	dMemory_MB = (mEnd - mBeg) / 1024.0 / 1024.0;

	// 2.查询
	double low[2], high[2];
	low[0] = g_x_min;
	low[1] = g_y_min;
	high[0] = g_x_max;
	high[1] = g_y_max;
	tBeg = clock();
	for (int iVal = 0; iVal < g_iQueryTimes; ++iVal)
	{
		g_lQueryCnt = 0;
		Region r = Region(low, high, 2);
		SpaVisitor vis;
		tree->intersectsWithQuery(r, vis);
	}
	tEnd = clock();
	lQuery = tEnd - tBeg;
	iQueryCnt = g_lQueryCnt;

	// 3.清理
	delete tree;
	delete store;

	// 4.日志
	fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
		strTime.c_str(), strMethod.c_str(), strMode.c_str(),
		lInsert, dMemory_MB, lQuery, iQueryCnt);
	fflush(g_pLog_Spa);
}


// 硬盘模式:RTree-BulkLoad:创建索引+查询数据
void TestSpa_3()
{
	// 0.要写日志的内容及初始化
	string strTime(""), strMethod(""), strMode("");
	long lInsert(0), lQuery(0);
	double dMemory_MB(0.0);
	int iQueryCnt(0);
	strTime = getCurTime();
	strMethod = "SpatialIndex-RTree";
	strMode = "Disk-BulkLoad";
	clock_t tBeg, tEnd;
	SIZE_T mBeg, mEnd;
	HANDLE hCurrent = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;

	// 1.清理索引
	string strShpPath = g_strDataDir + g_strDataset + ".shp";
	string strTreePath = g_strSpaWorkDir + g_strDataset + ".sidx";
	{
		if (0 == _access(strTreePath.c_str(), 00))
			remove(strTreePath.c_str());
	}

	// 2.创建索引
	GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
	mBeg = pmc.PagefileUsage;
	tBeg = clock();

	ShpDataStream stream(strShpPath);
	id_type indexIdentifier;

	IStorageManager* store = StorageManager::createNewDiskStorageManager(strTreePath, g_pageSize);
	StorageManager::IBuffer* buffer = StorageManager::createNewRandomEvictionsBuffer(*store, g_randomCapacity, false);
	ISpatialIndex* tree = RTree::createAndBulkLoadNewRTree(
		RTree::BLM_STR, stream, *buffer, g_fillFactor, g_indexCapacity, g_leafCapacity, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);

	tEnd = clock();
	GetProcessMemoryInfo(hCurrent, &pmc, sizeof(pmc));
	mEnd = pmc.PagefileUsage;
	lInsert = tEnd - tBeg;
	dMemory_MB = (mEnd - mBeg) / 1024.0 / 1024.0;

	// 3.查询
	double low[2], high[2];
	low[0] = g_x_min;
	low[1] = g_y_min;
	high[0] = g_x_max;
	high[1] = g_y_max;
	tBeg = clock();
	for (int iVal = 0; iVal < g_iQueryTimes; ++iVal)
	{
		g_lQueryCnt = 0;
		Region r = Region(low, high, 2);
		SpaVisitor vis;
		tree->intersectsWithQuery(r, vis);
	}
	tEnd = clock();
	lQuery = tEnd - tBeg;
	iQueryCnt = g_lQueryCnt;

	// 4.清理
	delete tree;
	delete buffer;
	delete store;

	// 5.日志
	fprintf(g_pLog_Spa, "%s,%s,%s,%ld,%.06f,%ld,%d\n",
		strTime.c_str(), strMethod.c_str(), strMode.c_str(),
		lInsert, dMemory_MB, lQuery, iQueryCnt);
	fflush(g_pLog_Spa);
}

