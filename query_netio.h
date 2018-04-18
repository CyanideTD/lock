#ifndef _KEY_VALUE_QUERY_NETIO_HH
#define _KEY_VALUE_QUERY_NETIO_HH

#include "std_header.h"
#include "session.h"
#include "conf.h"

using namespace wtse::log;

/*************************************************************************************
    ǰ����������IO
    1.������������
    2.��ȡSessionWrapper�����빤������
**************************************************************************************/
class CQueryNetIO : public ITasksGroupCallBack
{
public :
    CQueryNetIO();
    ~CQueryNetIO();

    /*
     *    ��ʼ������
     */
    HRESULT Init(TCHAR * pszIp, TUINT16 uwPort,CConf * poConf, CTseLogger *poLog, CTaskQueue *poWorkQueue);

    /*
     *    ����ʼ������
     */
    HRESULT Uninit();

public :

    /*
     *    ����IO�߳���������
     */
    static TVOID * RoutineNetIO(TVOID *pParam);

    /*
     *    ��ʼ�������
     */
    TVOID    StartNetServ();

    /*
     *    ֹͣ�������
     */
    TVOID    StopNetServ();

    
    /*
     *    ������Ϣ�ص�
     */
    virtual TVOID OnTasksFinishedCallBack(LTasksGroup *pstTasksGrp);

    /*
     *    ������Ϣ����
     */
    virtual TVOID OnUserRequest(LongConnHandle stHandle, const TUCHAR *pszData, TUINT32 udwDataLen, TINT32 &dwWillResponse);

     /*
     *  �رճ����ӣ�����ʱ����
     *    ��Ҫ������������ṩ�������񣬵�����֧�ָ��£�
     *   ��������������ʱ��
     */
    TVOID StopLongConn();

     /*
         *   ����ʱ����
     *  ���³�ʼ��������,���ô˺���ʱ����Ҫ�ؿ�һ���̣߳�����RoutineNetIO
     */  
    HRESULT RestartLongConn();

private :
    /*
     *    ��������
     */
    HRESULT    OnRequest(LongConnHandle stHandle, TUINT32 udwSeqNum, 
                            const TUCHAR *pszData, TUINT32 udwDataLen, TUINT16 uwServType);


public :
    /*
     *    ��������socket
     */
    SOCKET CreateListenSocket(TCHAR* pszListenHost,    TUINT16 uwPort);

    /*
     *    �رռ���socket
     */
    HRESULT    CloseListenSocket();

    /*
     *    ��ȡIP���˿�
     */
    HRESULT    GetIp2PortByHandle(LongConnHandle stHandle, TUINT16 *puwPort, TCHAR **ppszIp);

    /*
     *    �����η��ش���
     */
    HRESULT SendBackErr(LongConnHandle stHandle, TUINT16 uwSrvType, TUINT32 udwSeqNum, TINT32 dwErrorCode);

public :
    ILongConn                    *m_poLongConn;                                    //������
    CTseLogger                    *m_poLog;                                        // Log
    CTaskQueue                    *m_poWorkQueue;                                // work queue
    TINT32                        m_hListenSock;                                    // ����socket
    TBOOL                        m_bServNew;                                    // �Ƿ�����µķ���
    TBOOL                        m_bRelease;                                    //���Ƿ����ͷų�������Դ
    static TBOOL                m_bRunLongConn;                                //�Ƿ�ص�������
    CBaseProtocolUnpack            *m_poUnpack;                                    // �����
    CBaseProtocolPack            *m_poPack;                                        // �����
    CConf                       *m_poConf;                                      //conf
    TCHAR                       m_szIp[32];
    TUINT16                     m_uwPort;
};

#endif ///< _KEY_VALUE_QUERY_NETIO_HH

