#ifndef KEY_VALUE_GLOBAL_SERV_HH
#define KEY_VALUE_GLOBAL_SERV_HH
#include <unistd.h>
#include "conf.h"
#include "query_netio.h"
#include "session.h"
#include "work_session_mgr.h"
#include "work_process.h"
#include "base/common/wtsetypedef.h"
#include "base/log/wtselogger.h"
#include "std_header.h"
#include "statistic.h"
#include "lock_group.h"

using namespace wtse::log;

/*************************************************************************************
    �����������У�
        1.��̬ȫ����
        2.������־
        3.ע����־
        4.������־
        5.������Ϣ
        6.��������
        7.�ظ�����
        8.����buf����
        9.��������IO
        10.��������IO
        11.�����߳�
        12.�ظ��߳�
        13.�����߳�
        14.ע����

    ����
        1.��ʼ����Դ
        2.�����߳�

**************************************************************************************/
class CGlobalServ
{
private:
public :
    static  CGlobalServ		*g_poGlobalServ; 
    static CTseLogger		*g_poServLog;				// log
    
    CTseLogger				*m_poStatLog;				// old stat log

    CStatistic              *m_poStatistic;

	CLockGroup				*m_poLockHash;				// lock svr;

private :
    CTseLogger				*m_poRegLog;				/// reg log
    
    CConf					*m_poConf;                  /// �����ļ��������

    CTaskQueue				*m_poWorkQueue;              /// ��������

    CQueryNetIO				*m_poQueryNetIO;             /// ����IO
    CQueryNetIO				*m_poUpdateNetIO;            /// ����IO
    
    CWorkProcess			*m_paoWorkProcessList;       /// �����߳��б�

public:
    
    /*
     *    new һ��g_poGlobalServ
     */    
    static CGlobalServ * Instance();
    
    /*
     *    ��ʼ������
     *     
     *      pszRegCbuFile:ע����Ϣ�ļ�
     *       pszConfFile: ������Ϣ
     */
    static TBOOL Init(const TCHAR *pszConfFile, const TCHAR * pszRegCbuFile);

    /*
     *    ���������߳�
     */
    TBOOL Start();

       /*
     *    ͣ���з���
     */     
    TBOOL StopProcess();
    
};


#endif ///< KEY_VALUE_GLOBAL_SERV_HH





