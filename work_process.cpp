#include "work_process.h"
#include "base_def.h"
#include "session.h"
#include "statistic.h"
#include "global_serv.h"

using namespace wtse::log;

CWorkProcess::CWorkProcess()
{
    m_poConf    = NULL;              //配置
    m_poLog        = NULL;              //Log
    m_poUpdateQueue = NULL;         //更新队列
    m_poUnpack    = NULL;              // 解包工具
	m_poPack	= NULL;
}
CWorkProcess::~CWorkProcess()
{
    if(m_poUnpack)
	{
		delete m_poUnpack;
		m_poUnpack = NULL;
	}

	if(m_poPack)
	{
		delete m_poPack;
		m_poPack = NULL;
	}
}

TINT32 CWorkProcess::Init( CConf *poConf, CTseLogger *poLog, CTaskQueue *poUpdateQue, ILongConn *poUpdtLongConn, ILongConn *poQueryLongConn )
{
	TINT32 dwRetCode = 0;

    if(0 == poConf || 0 == poLog || 0 == poUpdateQue)
        return S_FAIL;
    m_poConf = poConf;
    m_poLog = poLog;
    m_poUpdateQueue = poUpdateQue;
	m_poUpdtLongConn = poUpdtLongConn;
	m_poQueryLongConn = poQueryLongConn;

    m_poUnpack = new CBaseProtocolUnpack;
    m_poUnpack->Init();
	m_poPack = new CBaseProtocolPack;
	m_poPack->Init();

    return dwRetCode;
}

TVOID * CWorkProcess::Start(TVOID *pParam)
{
    if(NULL == pParam)
        return NULL;
    CWorkProcess * poUpdateProcess = (CWorkProcess *)pParam;
    poUpdateProcess->WorkRoutine();
    return NULL;
}

TINT32    CWorkProcess::WorkRoutine()
{
    CSessionWrapper        *poSession        = 0;
    while(1)
    {
        if(S_OK == m_poUpdateQueue->WaitTillPop(poSession))
        {
			ProcessReq(poSession);
        }
    }
    return S_OK;
}

TINT32 CWorkProcess::ProcessReq( CSessionWrapper *poSession )
{
	TINT32 dwRetCode = 0;
	TUINT64 uddwCurTimeUs = 0;

	// 0. reset tmp param
	m_udwResBufLen = 0;
	m_udwCurResNum = 0;
	m_udwTotalResNum = 0;

	// 1. 解包
	if(poSession->m_udwReqTimes == 0)
	{
		dwRetCode = ParsePackage(poSession, &poSession->m_stReqParam);
		if(dwRetCode < 0)
		{
			TSE_LOG_ERROR(m_poLog, ("ParsePackage failed[%d], [seq=%u]", dwRetCode, poSession->m_udwClientSeq));
			poSession->m_dwRetCode = EN_RET_CODE__PARSE_PACKAGE_ERR;
			goto UPDATE_END;
		}
	}	

	// 2. 处理请求
	dwRetCode = DoWork(poSession, &poSession->m_stReqParam);
	if(dwRetCode != 0)
	{
		uddwCurTimeUs = CTimeUtils::GetCurTimeUs();
		if(uddwCurTimeUs > poSession->m_stReqParam.m_uddwTimeOut)
		{
			TSE_LOG_ERROR(m_poLog, ("curtime[%llu],timeout[%llu], [seq=%u]", uddwCurTimeUs, 
				poSession->m_stReqParam.m_uddwTimeOut, poSession->m_udwClientSeq));
			poSession->m_dwRetCode = EN_RET_CODE__USER_DATA_IS_USING;
			goto UPDATE_END;
		}
		else
		{
            usleep(1000);
			m_poUpdateQueue->WaitTillPush(poSession);
			return 0;
		}
	}

UPDATE_END:
	// 3. 返回结果
	SendRes(poSession);
	return 0;
}

TINT32 CWorkProcess::ParsePackage( CSessionWrapper *poSession, SReqParam *pstNode )
{
	pstNode->Reset();

	m_poUnpack->UntachPackage();
	m_poUnpack->AttachPackage(poSession->m_szReqBuf, poSession->m_udwReqBufLen);

	if(FALSE == m_poUnpack->Unpack())
	{
		TSE_LOG_ERROR(m_poLog, ("unpack failed, [seq=%u], [preSetbuflen %u],", poSession->m_udwClientSeq, poSession->m_udwReqBufLen));
		return -1;
	}

	TUCHAR *pszValBuf = NULL;
	TUINT32 udwValBufLen = 0;
	TUINT32 udwTimeOutUs = 0;
	
	m_poUnpack->GetVal(EN_KEY_HU2LOCK__REQ_KEY_NUM, &pstNode->m_udwKeyNum);
	m_poUnpack->GetVal(EN_KEY_HU2LOCK__REQ_KEY_LIST, &pszValBuf, &udwValBufLen);
	m_poUnpack->GetVal(EN_KEY_HU2LOCK__REQ_TIMEOUT_US, &udwTimeOutUs);
	if(pstNode->m_udwKeyNum != udwValBufLen/sizeof(SLockNode))
	{
		TSE_LOG_ERROR(m_poLog, ("num[%u],list_len[%u],not right, [seq=%u]", pstNode->m_udwKeyNum, udwValBufLen, poSession->m_udwClientSeq));
		pstNode->m_udwKeyNum = 0;
		return 0;
	}
	if(pstNode->m_udwKeyNum > MAX_LOCK_KEY_NUM_IN_ONE_REQ)
	{
		TSE_LOG_ERROR(m_poLog, ("num[%u]>max[%u], reset to max, [seq=%u]", pstNode->m_udwKeyNum, MAX_LOCK_KEY_NUM_IN_ONE_REQ, poSession->m_udwClientSeq));
		pstNode->m_udwKeyNum = MAX_LOCK_KEY_NUM_IN_ONE_REQ;
	}
	if(pstNode->m_udwKeyNum > 0)
	{
		memcpy((char*)&pstNode->m_auddwKey[0], pszValBuf, pstNode->m_udwKeyNum*sizeof(SLockNode));
	}
	else
	{
		TSE_LOG_ERROR(m_poLog, ("req_lock_num[%u] is empty, [seq=%u]", pstNode->m_udwKeyNum, poSession->m_udwClientSeq));
	}

	pstNode->m_uddwTimeOut = CTimeUtils::GetCurTimeUs() + udwTimeOutUs;

	TSE_LOG_DEBUG(m_poLog, ("time_out_us[%u], time_end[%llu], cur_time[%llu] [seq=%u]", 
		udwTimeOutUs, pstNode->m_uddwTimeOut, CTimeUtils::GetCurTimeUs(), poSession->m_udwClientSeq));

	return 0;
}

TINT32 CWorkProcess::DoWork( CSessionWrapper *poSession, SReqParam *pstNode )
{
	TINT32 dwRetCode = 0;
	CLockGroup *pobjLockHash = CGlobalServ::Instance()->m_poLockHash;

	if(poSession->m_udwServiceType == EN_SERVICE_TYPE_HU2LOCK__GET_REQ)
	{
		poSession->m_udwReqTimes++;
		dwRetCode = pobjLockHash->GetLock(&pstNode->m_auddwKey[0], pstNode->m_udwKeyNum);
	}
	else
	{
		pobjLockHash->ReleaseLock(&pstNode->m_auddwKey[0], pstNode->m_udwKeyNum);
	}

	return dwRetCode;
}


TINT32 CWorkProcess::SendRes( CSessionWrapper *poSession )
{
	TUCHAR *pucPackage = NULL;
	TUINT32 udwPackageLen = 0;

	// 1. get package
	m_poPack->ResetContent();
	if(poSession->m_udwServiceType == EN_SERVICE_TYPE_HU2LOCK__GET_REQ)
	{
		m_poPack->SetServiceType(EN_SERVICE_TYPE_LOCK2HU__GET_RSP);
	}
	else
	{
		m_poPack->SetServiceType(EN_SERVICE_TYPE_LOCK2HU__RELEASE_RSP);
	}
	m_poPack->SetSeq(poSession->m_udwClientSeq);
	m_poPack->SetKey(EN_GLOBAL_KEY__RES_CODE, poSession->m_dwRetCode);
	m_poPack->GetPackage(&pucPackage, &udwPackageLen);

	// 2. send back
	LTasksGroup        stTasks;
	stTasks.m_Tasks[0].SetConnSession(poSession->m_stHandle);
	stTasks.m_Tasks[0].SetSendData(pucPackage, udwPackageLen);
	stTasks.m_Tasks[0].SetNeedResponse(0);
	stTasks.SetValidTasks(1);
	if(m_poUpdtLongConn->SendData(&stTasks) == FALSE)
	{
		TSE_LOG_ERROR(m_poLog, ("send update res failed [seq=%u]", poSession->m_udwClientSeq));
		poSession->m_dwRetCode = EN_RET_CODE__SEND_FAILED;
	}

	// 3. log
	poSession->m_uddwResTimeUs = CTimeUtils::GetCurTimeUs();
	TUINT64 uddwCostUs = poSession->m_uddwResTimeUs - poSession->m_uddwReqTimeUs;
    string sKey = "";
    char szTmp[64];
    for(int idx = 0; idx < poSession->m_stReqParam.m_udwKeyNum; idx++)
    {
        sprintf(szTmp, "%d:%ld,", poSession->m_stReqParam.m_auddwKey[idx].m_dwType , poSession->m_stReqParam.m_auddwKey[idx].m_ddwKey);
        sKey.append(szTmp);
    }
	TSE_LOG_INFO(m_poLog, ("type[%u],num[%u],key[%s],ret[%d],times[%u],packlen[%u],cost[%llu] [seq=%u]", \
		poSession->m_udwServiceType, poSession->m_stReqParam.m_udwKeyNum, 
		sKey.c_str(),poSession->m_dwRetCode, poSession->m_udwReqTimes, udwPackageLen, uddwCostUs, poSession->m_udwClientSeq));

	// 4. release
	CWorkSessionMgr::Instance()->ReleaseSesion(poSession);

	return 0;
}
