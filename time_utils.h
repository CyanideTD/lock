#ifndef _TIME_UTILS_H_
#define _TIME_UTILS_H_

#include "base/common/wtse_std_header.h"
#include <time.h>
#include <sys/time.h>

class CTimeUtils
{
public:
	//��ȡ��������ȷ��΢��
	static TUINT64 GetCurTimeUs();

	//��ȡ��������ȷ����
	static TUINT32 GetUnixTime();

	//��ȡ������
	static TUINT64 GetCurTimeMs();
};

#endif