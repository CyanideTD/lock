#include "global_serv.h"
#include <unistd.h>

CGlobalServ    * CGlobalServ::g_poGlobalServ = NULL;
CTseLogger * CGlobalServ:: g_poServLog = NULL;    

//===============================================================================

CGlobalServ * CGlobalServ::Instance()
{
    if (NULL == g_poGlobalServ)
    {
        g_poGlobalServ            = new CGlobalServ;
        if(NULL == g_poGlobalServ)
            assert(0);
    }
    return g_poGlobalServ;
}

HRESULT CGlobalServ::Init(const TCHAR *pszConfFile, const TCHAR *pszRegCbuFile)
{
    CGlobalServ::Instance();
    
    TUINT32    udwI    = 0;
    // 1> log
    INIT_LOG_MODULE("../conf/serv_log.conf");
    assert(config_ret == 0); 
    DEFINE_LOG(serv_Log, "serv_log");
    g_poServLog                            = serv_Log;
    DEFINE_LOG(reg_log, "reg_log");
    g_poGlobalServ->m_poRegLog            = reg_log;
    DEFINE_LOG(stat_log, "stat_log");
    g_poGlobalServ->m_poStatLog        = stat_log;

    g_poGlobalServ->m_poStatistic = CStatistic::Instance();

    // 2> conf
    g_poGlobalServ->m_poConf = new CConf;
    if (S_OK != g_poGlobalServ->m_poConf->Init("../conf/serv_info.conf", "../conf/module.conf"))
    {
        TSE_LOG_ERROR(g_poServLog, ("GlobalServ Init conf failed, conf file[%s]", pszConfFile));
        return S_FAIL;
    }
    TSE_LOG_INFO(g_poServLog, ("GlobalServ Init conf succ"));
    g_poGlobalServ->m_poStatistic->Init(stat_log, g_poGlobalServ->m_poConf);

	// wavewang@20130511: lock hash
	g_poGlobalServ->m_poLockHash = new CLockGroup;
	if(0 != g_poGlobalServ->m_poLockHash->Init(g_poGlobalServ->m_poConf->m_udwHashTimeoutMs))
	{
		TSE_LOG_ERROR(g_poServLog, ("GlobalServ Init lock_hash failed"));
		return S_FAIL;
	}

    // 3> work queue
    g_poGlobalServ->m_poWorkQueue = new CTaskQueue;
    if (0 != g_poGlobalServ->m_poWorkQueue->Init(g_poGlobalServ->m_poConf->m_udwWorkQueSize))
    {
        TSE_LOG_ERROR(g_poServLog, ("GlobalServ Init work queue failed with queue_size=%u", g_poGlobalServ->m_poConf->m_udwWorkQueSize));
        return S_FAIL;
    }
    TSE_LOG_INFO(g_poServLog, ("GlobalServ Init work queue succ"));

    // 4>检索网络IO
    g_poGlobalServ->m_poQueryNetIO = new CQueryNetIO;
    if (S_OK != g_poGlobalServ->m_poQueryNetIO->Init(g_poGlobalServ->m_poConf->m_szServIp,g_poGlobalServ->m_poConf->m_uwQueryPort,
        g_poGlobalServ->m_poConf, g_poServLog, g_poGlobalServ->m_poWorkQueue))
    {
        TSE_LOG_ERROR(g_poServLog, ("GlobalServ Init query netio failed with [ip=%s] [port=%u]",
            g_poGlobalServ->m_poConf->m_szServIp, g_poGlobalServ->m_poConf->m_uwQueryPort));
        return S_FAIL;
    }
    TSE_LOG_INFO(g_poServLog, ("GlobalServ Init query net_io succ"));

    // 6>work process
    g_poGlobalServ->m_paoWorkProcessList = new CWorkProcess[g_poGlobalServ->m_poConf->m_udwWorkThreadNum];
    for(udwI = 0; udwI < g_poGlobalServ->m_poConf->m_udwWorkThreadNum; ++udwI)
    {
        if(S_OK!=g_poGlobalServ->m_paoWorkProcessList[udwI].Init(g_poGlobalServ->m_poConf,g_poGlobalServ->g_poServLog,
			g_poGlobalServ->m_poWorkQueue, g_poGlobalServ->m_poQueryNetIO->m_poLongConn,
			g_poGlobalServ->m_poQueryNetIO->m_poLongConn, g_poGlobalServ->m_poStatistic))
        {
            TSE_LOG_ERROR(g_poServLog, ("GlobalServ Init work process failed"));
            return S_FAIL;
        }
    }
    TSE_LOG_INFO(g_poServLog, ("GlobalServ Init update process succ"));
    
    // 7>work_session_mgr
    if (S_OK != CWorkSessionMgr::Instance()->Init(g_poGlobalServ->m_poConf, g_poServLog))
    {
        TSE_LOG_ERROR(g_poServLog, ("GlobalServ Init work session mgr failed"));
        return S_FAIL;
    }
    TSE_LOG_INFO(g_poServLog, ("GlobalServ Init work session mgr succ"));

    TSE_LOG_INFO(g_poServLog, ("GlobalServ Init succ!!"));

    return S_OK;
}

HRESULT CGlobalServ::Start()
{
    TUINT32                udwThr                    = 0;
    pthread_t            tThrId                    = 0;

    //1.启动工作线程
    for(udwThr = 0; udwThr < g_poGlobalServ->m_poConf->m_udwWorkThreadNum; ++udwThr)
    {
        if (0 != pthread_create(&tThrId, NULL, CWorkProcess::Start, &(g_poGlobalServ->m_paoWorkProcessList[udwThr])))
        {
            TSE_LOG_ERROR(g_poServLog, ("Create QueryProcess thread failed, idx[%u], err msg[%s]", udwThr, strerror(errno)));
            return S_FAIL;
        }
    }

    //2.启动检索netio
    if (0 != pthread_create(&tThrId, NULL, CQueryNetIO::RoutineNetIO,g_poGlobalServ->m_poQueryNetIO ))
    {
        TSE_LOG_ERROR(g_poServLog, ("Create query netio thread failed, err msg[%s]",  strerror(errno)));
        return S_FAIL;
    }

    if (0 != pthread_create(&tThrId, NULL, CStatistic::Start, g_poGlobalServ->m_poStatistic))
    {
        TSE_LOG_ERROR(g_poServLog, ("Create Statistic thread failed, err msg[%s]", strerror(errno)));
        return S_FAIL;
    }

    return S_OK;
}

HRESULT CGlobalServ::StopProcess()
{
    // stop越快越好

	return 0;
}
