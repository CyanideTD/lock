#include  <sys/time.h>
#include "statistic.h"

CStatistic *CStatistic::m_stat_tool = NULL;

CStatistic::CStatistic()
{
}

CStatistic::~CStatistic()
{
}

CStatistic * CStatistic::Instance()
{
    if (NULL == m_stat_tool)
    {
        m_stat_tool = new CStatistic();
    }

    return m_stat_tool;
}

int CStatistic::Init(CTseLogger *stat_log, CConf *pconf)
{
    if (NULL == stat_log || pconf->m_stat_interval_time == 0)
        return -1;

    // 日志对象
    m_stat_log = stat_log;
    // 统计间隔
    m_stat_interval_time = pconf->m_stat_interval_time;
    // 统计信息初始化
    m_stat_info.Reset();

	m_conf = pconf;

    return 0;
}

void * CStatistic::Start(void *param)
{
    CStatistic *stat_tool = (CStatistic *)param;
    while (1)
    {
        stat_tool->OutputResult();
		stat_tool->CheckSendMessage();
        stat_tool->Clear();
        sleep(stat_tool->m_stat_interval_time);
    }

    return NULL;
}

int CStatistic::Clear()
{
    m_stat_info.Reset();

    return 0;
}

int CStatistic::OutputResult()
{
	TUINT32 total = m_stat_info.search_total_count;

    TSE_LOG_INFO(m_stat_log, ("req_stat: [req=%u](%u/s) "
            "[hs_read=%u](%u/s,%.2fms) [hs_updt=%u](%u/s,%.2fms) [mysql_read=%u](%u/s,%.2fms) [error=%u](%u/s)",
            m_stat_info.search_total_count,
            m_stat_info.search_total_count / m_stat_interval_time,
            m_stat_info.hs_search_count,
            m_stat_info.hs_search_count / m_stat_interval_time,
			m_stat_info.hs_search_count > 0 ? m_stat_info.hs_read_time_us/m_stat_info.hs_search_count/1000.0 : 0,
            m_stat_info.hs_updt_count,
            m_stat_info.hs_updt_count / m_stat_interval_time,
			m_stat_info.hs_updt_count > 0 ? m_stat_info.hs_updt_time_us/m_stat_info.hs_updt_count/1000.0 : 0,
            m_stat_info.mysql_search_count,
            m_stat_info.mysql_search_count / m_stat_interval_time,
			m_stat_info.mysql_search_count > 0 ? m_stat_info.mysql_read_time_us/m_stat_info.mysql_search_count/1000.0 : 0,
            m_stat_info.error_count,
            m_stat_info.error_count / m_stat_interval_time));

    
    TSE_LOG_INFO(m_stat_log, ("time_stat: [avg=%.2fms] [max=%.2fms] "
            "[0-10ms=%u%%] [10-50ms=%u%%] [50-100ms=%u%%] [100-500ms=%u%%] "
            "[500-1000ms=%u%%] [>1000ms=%u%%]",
            total > 0 ? m_stat_info.search_total_time_us / total / 1000.0 : 0,
            total > 0 ? m_stat_info.search_max_time_us / 1000.0 : 0 ,
            total > 0 ? m_stat_info.search_time_0_10_count * 100 / total : 0,
            total > 0 ? m_stat_info.search_time_10_50_count * 100 / total : 0,
            total > 0 ? m_stat_info.search_time_50_100_count * 100 / total : 0,
            total > 0 ? m_stat_info.search_time_100_500_count * 100 / total : 0,
            total > 0 ? m_stat_info.search_time_500_1000_count * 100 / total : 0,
            total > 0 ? m_stat_info.search_time_1000_up_count * 100 / total : 0
            ));

    return 0;
}

int CStatistic::CheckSendMessage()
{
	if (!m_conf->m_need_send_message)
		return 0;

	// 当错误数大于阈值并且错误比例大于阈值的时候才报警
	if ((m_stat_info.error_count >= (TUINT32)m_conf->m_error_num_threshold) &&
		((float)m_stat_info.error_count / m_stat_info.search_total_count >=
		(float)m_conf->m_error_rate_threshold / 100))
	{
		snprintf(m_message, kMaxSendMessageSize,
			"./sendmessage.sh \"%s-"
			"Error:%u/Total:%u in %u sec![%s:%u]\" ",
			m_conf->m_project_name,
			m_stat_info.error_count,
			m_stat_info.search_total_count,
			m_conf->m_stat_interval_time,
			m_conf->m_szServIp,
			m_conf->m_uwQueryPort);
		system(m_message);
		TSE_LOG_INFO(m_stat_log, ("Send error message: ErrorNum[%u]"
			"/TotalNum[%u] in %u seconds!",
			m_stat_info.error_count, m_stat_info.search_total_count, m_conf->m_stat_interval_time));
	}

	return 0;
}

int CStatistic::AddHsSearch(TUINT64 search_time)
{
    AddTimeStat(search_time);
    m_stat_info.hs_search_count++;
    m_stat_info.search_total_count++;

	m_stat_info.hs_read_time_us += search_time;

    return 0;
}

int CStatistic::AddHsUpdate(TUINT64 search_time)
{
    AddTimeStat(search_time);
    m_stat_info.hs_updt_count++;
    m_stat_info.search_total_count++;

	m_stat_info.hs_updt_time_us += search_time;

    return 0;
}

int CStatistic::AddMysqlSearch(TUINT64 search_time)
{
    AddTimeStat(search_time);
    m_stat_info.mysql_search_count++;
    m_stat_info.search_total_count++;

	m_stat_info.mysql_read_time_us += search_time;

    return 0;
}

int CStatistic::AddError(TUINT64 search_time)
{
    AddTimeStat(search_time);
    m_stat_info.error_count++;
    m_stat_info.search_total_count++;

    return 0;
}

int CStatistic::AddTimeStat(TUINT64 search_time)
{
    if (search_time > m_stat_info.search_max_time_us)
        m_stat_info.search_max_time_us = search_time;

    m_stat_info.search_total_time_us += search_time;

    if (search_time < 10 * 1000)
        m_stat_info.search_time_0_10_count++;
    else if (search_time < 50 * 1000)
        m_stat_info.search_time_10_50_count++;
    else if (search_time < 100 * 1000)
        m_stat_info.search_time_50_100_count++;
    else if (search_time < 500 * 1000)
        m_stat_info.search_time_100_500_count++;
    else if (search_time < 1000 * 1000)
        m_stat_info.search_time_500_1000_count++;
    else // >1s
        m_stat_info.search_time_1000_up_count++;

    return 0;
}

int CStatistic::GetStatInfo(StatisticInfo *stat_info)
{
    *stat_info = m_stat_info;
    return 0;
}

