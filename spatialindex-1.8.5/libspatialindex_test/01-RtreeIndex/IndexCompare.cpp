#include "CompIndex_Spatial.h"
#include "CompIndex_Attribute.h"

FILE*    g_pLog_Spa = NULL;
FILE*    g_pLog_Att = NULL;

#define MULTITEST(code, times)              \
    for (int iTime=0; iTime<times; ++iTime) \
    {                                       \
        code;                               \
    }                                       \
                                            \

int main(int argc, char* argv[])
{
	g_pLog_Spa = fopen(g_strSpaLog.c_str(), "a");
	if (NULL == g_pLog_Spa)
		return 1;
	g_pLog_Att = fopen(g_strAttLog.c_str(), "a");
	if (NULL == g_pLog_Att)
		return 1;

	//MULTITEST(TestLoop_1(),5);
	//MULTITEST(TestLoop_2(),5);
	//MULTITEST(TestLoop_3(),5);    // 查询100次非常慢，建议改为10次，把结果再乘10

	//MULTITEST(TestQix_1(),5);
	//MULTITEST(TestQix_2(),5);

	//MULTITEST(TestSbn_1(),5);

	//MULTITEST(TestGeos_1(),5);
	//MULTITEST(TestGeos_2(),5);

	//MULTITEST(TestSpa_1(),5);
	MULTITEST(TestSpa_2(), 5);
	//MULTITEST(TestSpa_3(),5);

	fclose(g_pLog_Spa);
	fclose(g_pLog_Att);
	return 0;
}