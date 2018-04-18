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
    包含的数据有：
        1.静态全局类
        2.服务日志
        3.注册日志
        4.更新日志
        5.配置信息
        6.工作队列
        7.回复队列
        8.更新buf队列
        9.更新网络IO
        10.检索网络IO
        11.检索线程
        12.回复线程
        13.更新线程
        14.注册类

    负责：
        1.初始化资源
        2.启动线程

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
    
    CConf					*m_poConf;                  /// 配置文件解析结果

    CTaskQueue				*m_poWorkQueue;              /// 工作队列

    CQueryNetIO				*m_poQueryNetIO;             /// 网络IO
    CQueryNetIO				*m_poUpdateNetIO;            /// 网络IO
    
    CWorkProcess			*m_paoWorkProcessList;       /// 工作线程列表

public:
    
    /*
     *    new 一个g_poGlobalServ
     */    
    static CGlobalServ * Instance();
    
    /*
     *    初始化函数
     *     
     *      pszRegCbuFile:注册信息文件
     *       pszConfFile: 其它信息
     */
    static TBOOL Init(const TCHAR *pszConfFile, const TCHAR * pszRegCbuFile);

    /*
     *    启动所有线程
     */
    TBOOL Start();

       /*
     *    停所有服务
     */     
    TBOOL StopProcess();
    
};


#endif ///< KEY_VALUE_GLOBAL_SERV_HH





