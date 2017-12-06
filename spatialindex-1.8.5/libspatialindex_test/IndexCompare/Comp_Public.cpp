#include "stdafx.h"
#include "Comp_Public.h"

string getCurTime()
{
    time_t t = time(NULL);
    tm *pTM = localtime(&t);

    char szTime[20] = {0};
    sprintf_s(szTime, 20, "%04d-%02d-%02d %02d:%02d:%02d",
        pTM->tm_year+1900, pTM->tm_mon+1, pTM->tm_mday,
        pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
    return string(szTime);
}
