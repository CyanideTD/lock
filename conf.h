#ifndef _KEY_VALUE_CONF_HH
#define _KEY_VALUE_CONF_HH

#include "base/common/wtsetypedef.h"
#include "my_define.h"
#include <string>

#define DEFAULT_NAME_STR_LEN		(64)

#pragma pack(1)

/*************************************************************************************
    KEY_VALUE配置参数
**************************************************************************************/
class CConf
{
public:
    CConf();
    ~CConf();

    //初始化配置文件
    TINT32 Init(const TCHAR *pszServFile, const TCHAR *pszModuleFile);

    //服务ip
    TCHAR       m_szServIp[20];
    //服务端口
    TUINT16     m_uwQueryPort;

    //队列大小
    TUINT32        m_udwWorkQueSize;
    //工作线程个数
    TUINT32        m_udwWorkThreadNum;

	// cache
	TUINT32		m_udwHashTimeoutMs;
	TUINT32		m_udwHashMaxGetTimes;

	// 告警设置
	TCHAR	m_project_name[DEFAULT_NAME_STR_LEN];
	TUINT32 m_stat_interval_time;
	TUINT32 m_need_send_message;
	TUINT32 m_error_num_threshold;
	TUINT32 m_error_rate_threshold;
};

#pragma pack()

#endif ///< _KEY_VALUE_CONF_HH

