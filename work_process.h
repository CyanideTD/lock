#ifndef _UPDATE_PROCESS_H_
#define _UPDATE_PROCESS_H_

#include "std_header.h"
#include "work_session_mgr.h"
#include "conf.h"

/*************************************************************************************
    后台工作线程
**************************************************************************************/
class CWorkProcess
{
public :
    CWorkProcess();
    ~CWorkProcess();

    /*
     *    初始化函数
     */
	TINT32 Init(CConf *poConf, CTseLogger *poLog,  CTaskQueue *poUpdateQue, ILongConn *poUpdtLongConn, ILongConn *poQueryLongConn, CStatistic* stat);

    /*
     *    线程启动函数
     */
    static TVOID * Start(TVOID *pParam);

    /*
     *    工作循环
     */
    TINT32    WorkRoutine();

private:
	// 处理前端请求
	TINT32 ProcessReq(CSessionWrapper *poSession);
	TINT32 ParsePackage( CSessionWrapper *poSession, SReqParam *pstNode );
	TINT32 DoWork(CSessionWrapper *poSession, SReqParam *pstNode);
    TINT32 SendRes(CSessionWrapper *poSession);

private:
	ILongConn                    *m_poUpdtLongConn;			// 数据更新长连接
	ILongConn					 *m_poQueryLongConn;		//数据查询长连接
    CConf                        *m_poConf;				    //配置

    CTseLogger                    *m_poLog;					//Log

    CStatistic                    *m_poStat;
    
    CTaskQueue                    *m_poUpdateQueue;         //更新队列
    
    CBaseProtocolUnpack           *m_poUnpack;              // 解包工具
	CBaseProtocolPack			  *m_poPack;				// 打包工具

	// buf
	TUCHAR							m_szResBuf[MAX_NET_IO_RES_PACKAGE_LEN];
	TUINT32							m_udwResBufLen;
	TUINT32							m_udwCurResNum;
	TUINT32							m_udwTotalResNum;
};

#endif

