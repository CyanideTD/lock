#ifndef _UPDATE_PROCESS_H_
#define _UPDATE_PROCESS_H_

#include "std_header.h"
#include "work_session_mgr.h"
#include "conf.h"

/*************************************************************************************
    ��̨�����߳�
**************************************************************************************/
class CWorkProcess
{
public :
    CWorkProcess();
    ~CWorkProcess();

    /*
     *    ��ʼ������
     */
	TINT32 Init(CConf *poConf, CTseLogger *poLog,  CTaskQueue *poUpdateQue, ILongConn *poUpdtLongConn, ILongConn *poQueryLongConn, CStatistic* stat);

    /*
     *    �߳���������
     */
    static TVOID * Start(TVOID *pParam);

    /*
     *    ����ѭ��
     */
    TINT32    WorkRoutine();

private:
	// ����ǰ������
	TINT32 ProcessReq(CSessionWrapper *poSession);
	TINT32 ParsePackage( CSessionWrapper *poSession, SReqParam *pstNode );
	TINT32 DoWork(CSessionWrapper *poSession, SReqParam *pstNode);
    TINT32 SendRes(CSessionWrapper *poSession);

private:
	ILongConn                    *m_poUpdtLongConn;			// ���ݸ��³�����
	ILongConn					 *m_poQueryLongConn;		//���ݲ�ѯ������
    CConf                        *m_poConf;				    //����

    CTseLogger                    *m_poLog;					//Log

    CStatistic                    *m_poStat;
    
    CTaskQueue                    *m_poUpdateQueue;         //���¶���
    
    CBaseProtocolUnpack           *m_poUnpack;              // �������
	CBaseProtocolPack			  *m_poPack;				// �������

	// buf
	TUCHAR							m_szResBuf[MAX_NET_IO_RES_PACKAGE_LEN];
	TUINT32							m_udwResBufLen;
	TUINT32							m_udwCurResNum;
	TUINT32							m_udwTotalResNum;
};

#endif

