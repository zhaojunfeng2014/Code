#pragma once

#include <string>
#include <time.h>
using namespace std;

#include <Windows.h>
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")

const int g_iQueryTimes    = 100;

const string g_strDataDir   = "E:\\TempCode\\IndexCompare\\data\\";
const string g_strDataset   = "C02L";

const string g_strSpaWorkDir   = "E:\\TempCode\\IndexCompare\\temp\\Spatial\\";
const string g_strAttWorkDir   = "E:\\TempCode\\IndexCompare\\temp\\Attrib\\";

const string g_strSpaLog   = "E:\\TempCode\\IndexCompare\\temp\\Spatial\\timer.csv";
const string g_strAttLog   = "E:\\TempCode\\IndexCompare\\temp\\Attrib\\timer.csv";

extern FILE*    g_pLog_Spa;
extern FILE*    g_pLog_Att;

// ���ݰ��������ֶ�
// INTΨһֵ�ֶΣ�FEATUREID
// INTö���ֶΣ�TYPE
// DBL��ֵ�ֶΣ�LENGTH
// STRö���ֶΣ�CODE
const string g_strField_FeatureID   = "FEATUREID";
const string g_strField_Type        = "TYPE";
const string g_strField_Length      = "LENGTH";
const string g_strField_Code        = "CODE";

const double g_x_min = 465701.923;
const double g_x_max = 469994.306;
const double g_y_min = 3852567.173;
const double g_y_max = 3855717.229;

string getCurTime();

