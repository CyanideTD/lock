#ifndef TSHP_HU_STATISTIC_H
#define TSHP_HU_STATISTIC_H

#include "std_header.h"
#include "conf.h"

const TUINT32 kMaxSendMessageSize = 256;

#pragma pack(1)
struct StatisticInfo
{
    TUINT32     search_total_count;
    TUINT32     hs_search_count;
    TUINT32     hs_updt_count;
    TUINT32     mysql_search_count;
	TUINT32		error_count;

    TUINT64     search_max_time_us;
    TUINT64     search_total_time_us;
    TUINT32     search_time_0_10_count;
    TUINT32     search_time_10_50_count;
    TUINT32     search_time_50_100_count;
    TUINT32     search_time_100_500_count;
    TUINT32     search_time_500_1000_count;
    TUINT32     search_time_1000_up_count;

	TUINT64		hs_read_time_us;
	TUINT64		hs_updt_time_us;
	TUINT64		mysql_read_time_us;

    StatisticInfo()
    {
        memset(this, 0, sizeof(StatisticInfo));
    }

    void Reset()
    {
        memset(this, 0, sizeof(StatisticInfo));
    }
};
#pragma pack()

class CStatistic
{
public :
    static CStatistic *Instance();
    ~CStatistic();
    int Init(CTseLogger *stat_log, CConf *pconf);
    static void *Start(void *param);

    int Clear();
    int OutputResult();

    /*
     * ����Ϊ�����ɹ�,��������,����ʧ��,������Ϊ���д�,����������(Ԥ��)
     */
    int AddHsSearch(TUINT64 search_time);
    int AddHsUpdate(TUINT64 search_time);
    int AddMysqlSearch(TUINT64 search_time);



    int AddError(TUINT64 search_time);

    int AddTimeStat(TUINT64 search_time);

    int GetStatInfo(StatisticInfo *stat_info);

	int CheckSendMessage();

private :
    CStatistic();
    static CStatistic *m_stat_tool;

    // ��־����
    CTseLogger      *m_stat_log;
    // ͳ�Ƽ��
    TUINT32         m_stat_interval_time;
    // ͳ����Ϣ
    StatisticInfo   m_stat_info;
	// ����
	CConf			*m_conf;
	char            m_message[kMaxSendMessageSize];
};

#endif

