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
    Session��װ��
**************************************************************************************/
class CSessionWrapper
{
public :
    //����CBU��Ϣ
    LongConnHandle      m_stHandle;								// CBu�������ӹ�����handle
    TUINT32             m_udwClientSeq;							// CBU�����seqnum
	TUINT32				m_udwServiceType;						// �����������
    
    //���������Ϣ
    TUCHAR              m_szReqBuf[MAX_NET_IO_REQ_PACKAGE_LEN]; //CBU������������BUF
    TUINT32             m_udwReqBufLen;                         //CBU���͹���������BUF����

    //ʱ�����
    TUINT64             m_uddwReqTimeUs;						//����ʱ��
    TUINT64             m_uddwResTimeUs;						//���������ε�ʱ��

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


