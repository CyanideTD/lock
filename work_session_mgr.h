#ifndef _KEY_VALUE_WORK_SESSION_MGR__
#define _KEY_VALUE_WORK_SESSION_MGR__

#include "queue_t.h"
#include "session.h"
#include "conf.h"
#include "base/log/wtselogger.h"
using namespace wtse::log;

/*************************************************************************************
    Session ����
    1.����ģʽ
    2.��ҵ��so��ʼ��ҵ��
    3.��ҵ��so���غͳ�ʼ��Session,����װ��SessionWrapper
    4.�ṩȡSessionWrapper���ͻ���SessionWrapper�Ĺ���
    5.�ṩ����ʱ�滻ҵ��so�Ĺ���
**************************************************************************************/
class CWorkSessionMgr
{
private :
    static CWorkSessionMgr                * m_poCSessionMgr;
    CWorkSessionMgr();
public :
    ~CWorkSessionMgr();

    /*
     *    ��ȡSessionMgr����
     *    \return
     *        S_OK �ɹ���S_FAIL ʧ��
     */
    static CWorkSessionMgr    * Instance();

    /*
     *    ��ʼ��
     *    \return
     *        S_OK �ɹ���S_FAIL ʧ��
     */
    HRESULT    Init( CConf * poConf, CTseLogger *poLog);

    /*
     *    ����ʼ�����ͷ���Դ
     *    \return
     *        S_OK �ɹ���S_FAIL ʧ��
     */
    HRESULT    Uninit();

    /*
     *    ȡSessionWrapper�����޵ȴ�
     *    \return
     *        S_OK �ɹ���S_FAIL ʧ��
     */
    HRESULT    WaitTillSession(CSessionWrapper **ppoSessionWrapper);

    /*
     *    ȡSessionWrapper���г�ʱ����
     *    \return
     *        S_OK �ɹ���S_FAIL ʧ��
     */
    HRESULT    WaitTimeSession(CSessionWrapper **ppoSessionWrapper);

    /*
     *    ����SessionWrapper
     *    \return
     *        S_OK �ɹ���S_FAIL ʧ��
     */
    HRESULT    ReleaseSesion(CSessionWrapper *poSessionWrapper);

private :

    CQueueT<CSessionWrapper *>        *m_poMemQue;                        // �������
    CTseLogger                        *m_poLog;                            // ��־����

    TUINT32                            m_udwSessionNum;                    // Session�ܸ���
    CSessionWrapper                    *m_apoSessionWrapprList;            // ��װ���ڴ��б�
    TBOOL volatile                    m_bReady;                            // �Ƿ�����ṩ�������
};

#endif ///< _KEY_VALUE_WORK_SESSION_MGR__

