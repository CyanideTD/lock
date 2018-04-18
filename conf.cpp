#include <string.h>
#include <stdio.h>
#include "conf.h"
#include "base/conf/wtse_ini_configer.h"
using namespace wtse::conf;

CConf::CConf()
{
	// do nothing
}

CConf::~CConf()
{
	// do nothing
}

//初始化配置文件
TINT32 CConf::Init(const TCHAR *pszServFile, const TCHAR *pszModuleFile)
{
    CTseIniConfiger objConfig;
    TBOOL bConf            = FALSE;
	/*TCHAR szKey[128];
	TUINT32 idx = 0, idy = 0;*/

    bConf = objConfig.LoadConfig(pszServFile);
    assert(bConf == TRUE);

    bConf = objConfig.GetValue("SERV_INFO", "module_ip", m_szServIp);
    assert(bConf == TRUE);

    bConf = objConfig.LoadConfig(pszModuleFile);
    assert(bConf == TRUE);

    bConf = objConfig.GetValue("LOCK_SVR_INFO", "lock_svr_serv_port", m_uwQueryPort);
    assert(bConf == TRUE);

    bConf = objConfig.GetValue("LOCK_SVR_INFO", "lock_svr_WorkQueSize", m_udwWorkQueSize);
    assert(bConf == TRUE);
    bConf = objConfig.GetValue("LOCK_SVR_INFO", "lock_svr_WorkThreadNum", m_udwWorkThreadNum);
    assert(bConf == TRUE);

	// cache
	assert(bConf == TRUE);
	bConf = objConfig.GetValue("LOCK_SVR_INFO", "lock_svr_HashTimeoutMs", m_udwHashTimeoutMs);
	assert(bConf == TRUE);
	bConf = objConfig.GetValue("LOCK_SVR_INFO", "lock_svr_HashMaxGetTimes", m_udwHashMaxGetTimes);
	assert(bConf == TRUE);

	// stat 
    std::string tmp;
	bConf = objConfig.GetValue("PROJECT_INFO", "project", tmp);
	assert(bConf == true);
    tmp = tmp + "_lock_svr";
    memcpy(m_project_name, tmp.c_str(), strlen(tmp.c_str()) + 1);

	bConf = objConfig.GetValue("LOCK_SVR_INFO", "lock_svr_IntervalTime", m_stat_interval_time);
	assert(bConf == true);
	bConf = objConfig.GetValue("LOCK_SVR_INFO", "lock_svr_NeedSendMessage", m_need_send_message);
	assert(bConf == true);
	bConf = objConfig.GetValue("LOCK_SVR_INFO", "lock_svr_ErrNumThreshold", m_error_num_threshold);
	assert(bConf == true);
	bConf = objConfig.GetValue("LOCK_SVR_INFO", "lock_svr_ErrRateThreashold", m_error_rate_threshold);
	assert(bConf == true);

	

    return 0;
}
