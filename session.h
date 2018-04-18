#ifndef _KEY_VALUE_SESSION_HH
#define _KEY_VALUE_SESSION_HH

#include "std_header.h"
#include "queue_t.h"
#include "lock_group.h"

using namespace wtse::log;

#pragma pack(1)

enum EHashReqType
{
	EN_HASH_LOCK__GET = 0,
	EN_HASH_LOCK__UPDT,
};

struct SReqParam
{
	TUINT32 m_udwKeyNum;
	SLockNode m_auddwKey[MAX_LOCK_KEY_NUM_IN_ONE_REQ];
	TUINT64 m_uddwTimeOut;

	TVOID Reset()
	{
		memset((char*)this, 0, sizeof(*this));
	}
};


/*************************************************************************************
    Session包装器
**************************************************************************************/
class CSessionWrapper
{
public :
    //上游CBU信息
    LongConnHandle      m_stHandle;								// CBu请求连接过来的handle
    TUINT32             m_udwClientSeq;							// CBU请求的seqnum
	TUINT32				m_udwServiceType;						// 请求服务类型
    
    //请求相关信息
    TUCHAR              m_szReqBuf[MAX_NET_IO_REQ_PACKAGE_LEN]; //CBU发过来的请求BUF
    TUINT32             m_udwReqBufLen;                         //CBU发送过来的请求BUF长度

    //时间参数
    TUINT64             m_uddwReqTimeUs;						//请求时间
    TUINT64             m_uddwResTimeUs;						//返包给上游的时间

    TINT32              m_dwRetCode;
	TUINT32				m_udwReqTimes;

	SReqParam			m_stReqParam;
    
    TVOID    Init()
    {
        Reset();
    }

    TVOID    Reset()
    {
		m_udwClientSeq    = 0;
		m_udwServiceType = 0;

		m_udwReqBufLen = 0;

		m_uddwReqTimeUs = 0;
		m_uddwResTimeUs = 0;

		m_dwRetCode = 0;
		m_udwReqTimes = 0;

		m_stReqParam.Reset();
    }
};

typedef CQueueT<CSessionWrapper * >     CTaskQueue;

#pragma pack()

#endif ///< _KEY_VALUE_SESSION_HH


