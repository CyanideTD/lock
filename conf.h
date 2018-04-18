#ifndef _KEY_VALUE_CONF_HH
#define _KEY_VALUE_CONF_HH

#include "base/common/wtsetypedef.h"
#include "my_define.h"
#include <string>

#define DEFAULT_NAME_STR_LEN		(64)

#pragma pack(1)

/*************************************************************************************
    KEY_VALUE���ò���
**************************************************************************************/
class CConf
{
public:
    CConf();
    ~CConf();

    //��ʼ�������ļ�
    TINT32 Init(const TCHAR *pszServFile, const TCHAR *pszModuleFile);

    //����ip
    TCHAR       m_szServIp[20];
    //����˿�
    TUINT16     m_uwQueryPort;

    //���д�С
    TUINT32        m_udwWorkQueSize;
    //�����̸߳���
    TUINT32        m_udwWorkThreadNum;

	// cache
	TUINT32		m_udwHashTimeoutMs;
	TUINT32		m_udwHashMaxGetTimes;

	// �澯����
	TCHAR	m_project_name[DEFAULT_NAME_STR_LEN];
	TUINT32 m_stat_interval_time;
	TUINT32 m_need_send_message;
	TUINT32 m_error_num_threshold;
	TUINT32 m_error_rate_threshold;
};

#pragma pack()

#endif ///< _KEY_VALUE_CONF_HH

