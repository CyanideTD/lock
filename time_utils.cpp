#include "time_utils.h"

TUINT64 CTimeUtils::GetCurTimeUs()
{
	struct timeval  stV;
	struct timezone stZ;
	TUINT64 cur_time = 0;

	stZ.tz_minuteswest = 0;
	stZ.tz_dsttime     = 0;
	gettimeofday(&stV, &stZ);
	cur_time = stV.tv_sec * (TUINT64)1000000 + stV.tv_usec;
	return cur_time;
}

TUINT32 CTimeUtils::GetUnixTime()
{
	return time((time_t*)NULL);
}

TUINT64 CTimeUtils::GetCurTimeMs()
{
	return (GetCurTimeUs()/1000);
}
