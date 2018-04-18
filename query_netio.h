#ifndef _KEY_VALUE_QUERY_NETIO_HH
#define _KEY_VALUE_QUERY_NETIO_HH

#include "std_header.h"
#include "session.h"
#include "conf.h"

using namespace wtse::log;

/*************************************************************************************
    前端请求网络IO
    1.接收请求数据
    2.获取SessionWrapper，放入工作队列
**************************************************************************************/
class CQueryNetIO : public ITasksGroupCallBack
{
public :
    CQueryNetIO();
    ~CQueryNetIO();

    /*
     *    初始化函数
     */
    HRESULT Init(TCHAR * pszIp, TUINT16 uwPort,CConf * poConf, CTseLogger *poLog, CTaskQueue *poWorkQueue);

    /*
     *    反初始化函数
     */
    HRESULT Uninit();

public :

    /*
     *    网络IO线程启动函数
     */
    static TVOID * RoutineNetIO(TVOID *pParam);

    /*
     *    开始网络服务
     */
    TVOID    StartNetServ();

    /*
     *    停止网络服务
     */
    TVOID    StopNetServ();

    
    /*
     *    网络消息回调
     */
    virtual TVOID OnTasksFinishedCallBack(LTasksGroup *pstTasksGrp);

    /*
     *    网络消息请求
     */
    virtual TVOID OnUserRequest(LongConnHandle stHandle, const TUCHAR *pszData, TUINT32 udwDataLen, TINT32 &dwWillResponse);

     /*
     *  关闭长连接，特殊时候用
     *    主要可用于想继续提供检索服务，但不想支持更新，
     *   而不想重启程序时用
     */
    TVOID StopLongConn();

     /*
         *   特殊时候用
     *  重新初始化长链接,调用此函数时，需要重开一个线程，运行RoutineNetIO
     */  
    HRESULT RestartLongConn();

private :
    /*
     *    处理请求
     */
    HRESULT    OnRequest(LongConnHandle stHandle, TUINT32 udwSeqNum, 
                            const TUCHAR *pszData, TUINT32 udwDataLen, TUINT16 uwServType);


public :
    /*
     *    创建监听socket
     */
    SOCKET CreateListenSocket(TCHAR* pszListenHost,    TUINT16 uwPort);

    /*
     *    关闭监听socket
     */
    HRESULT    CloseListenSocket();

    /*
     *    获取IP，端口
     */
    HRESULT    GetIp2PortByHandle(LongConnHandle stHandle, TUINT16 *puwPort, TCHAR **ppszIp);

    /*
     *    向上游返回错误
     */
    HRESULT SendBackErr(LongConnHandle stHandle, TUINT16 uwSrvType, TUINT32 udwSeqNum, TINT32 dwErrorCode);

public :
    ILongConn                    *m_poLongConn;                                    //长连接
    CTseLogger                    *m_poLog;                                        // Log
    CTaskQueue                    *m_poWorkQueue;                                // work queue
    TINT32                        m_hListenSock;                                    // 监听socket
    TBOOL                        m_bServNew;                                    // 是否继续新的服务
    TBOOL                        m_bRelease;                                    //看是否能释放长链接资源
    static TBOOL                m_bRunLongConn;                                //是否关掉长链接
    CBaseProtocolUnpack            *m_poUnpack;                                    // 解包器
    CBaseProtocolPack            *m_poPack;                                        // 打包器
    CConf                       *m_poConf;                                      //conf
    TCHAR                       m_szIp[32];
    TUINT16                     m_uwPort;
};

#endif ///< _KEY_VALUE_QUERY_NETIO_HH

