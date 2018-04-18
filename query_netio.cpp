#include "query_netio.h"
#include "work_session_mgr.h"
#include "base_def.h"


TBOOL CQueryNetIO::m_bRunLongConn = FALSE;
extern int g_dwServFlag;


//=================================================================
// initialize
CQueryNetIO::CQueryNetIO()
{// do nothing
}

CQueryNetIO::~CQueryNetIO()
{
    if (NULL != m_poLongConn)
    {
        m_poLongConn->UninitLongConn();
        m_poLongConn->Release();
        m_poLongConn = NULL;
    }
    
    m_poUnpack->Uninit();
    m_poPack->Uninit();
}

HRESULT CQueryNetIO::Init(TCHAR * pszIp, TUINT16 uwPort,CConf *poConf, CTseLogger *poLog, CTaskQueue *poWorkQueue)
{
    
    if (pszIp[0] == '\0' || uwPort== 0||0 == poConf || 0 == poLog || 0 == poWorkQueue)
    {
        return S_FAIL;
    }
    // 1> Log
    m_poLog                                        = poLog;

    // 2> work queue
    m_poWorkQueue                                = poWorkQueue;
    m_uwPort                                    = uwPort;
    TUINT32 udwIpLen = strlen(pszIp);
    if(udwIpLen>32 || udwIpLen < 5)
    {
        return S_FAIL;
    }
    memcpy(m_szIp, pszIp, udwIpLen);
    //3> conf
    m_poConf                                    = poConf;
    
    // 4> LongConn
    m_poLongConn                                = CreateLongConnObj();
    if (0 == m_poLongConn)
    {
        TSE_LOG_ERROR(m_poLog, ("QueryNetIO Create longconn fail"));
        return S_FAIL;
    }
    m_hListenSock = CreateListenSocket((TCHAR*)pszIp, uwPort);
    if (m_hListenSock < 0)
    {
        TSE_LOG_ERROR(m_poLog, ("QueryNetIO Create listen socket fail"));
        return S_FAIL;
    }
    if (FALSE == m_poLongConn->InitLongConn(this, 1024, m_hListenSock, LONG_CONN_TIMEOUT_QUERY))
    {
        TSE_LOG_ERROR(m_poLog, ("QueryNetIO Init LongConn fail"));
        return S_FAIL;
    }

    // 4> 不接收新的请求
    m_bServNew                                    = FALSE;
    m_bRelease                                     = FALSE;
    // 5> pack, unpack
    m_poUnpack                                    = new CBaseProtocolUnpack;
    m_poUnpack->Init();
    m_poPack                                    = new CBaseProtocolPack;
    m_poPack->Init();

    CQueryNetIO::m_bRunLongConn = TRUE;
    return S_OK;
}

HRESULT CQueryNetIO::Uninit()
{
    m_bServNew                                    = FALSE;
    // do nothing else
    return S_OK;
}

//=================================================================
// control
TVOID *    CQueryNetIO::RoutineNetIO(TVOID *pParam)
{
    CQueryNetIO *pNetIO                            = 0;
    pNetIO                                        = (CQueryNetIO*)pParam;

    pNetIO->StartNetServ();
    for(;;)
    {
        //收发数据,不能停
        if(m_bRunLongConn)
        {
            pNetIO->m_poLongConn->RoutineLongConn(1000);
            pNetIO->m_bRelease = TRUE;
        }
        else
        {
            pNetIO->m_bRelease = TRUE;
            break;
        }
        pNetIO->m_bRelease = FALSE;
    }
    return 0;
}

TVOID    CQueryNetIO::StartNetServ()
{
    m_bServNew                                    = TRUE;
    TSE_LOG_INFO(m_poLog, ("QueryNetIO start net service"));
}

TVOID    CQueryNetIO::StopNetServ()
{
    m_bServNew                                    = FALSE;
    TSE_LOG_INFO(m_poLog, ("QueryNetIO stop net service"));
}

TVOID CQueryNetIO::StopLongConn()
{
    CQueryNetIO::m_bRunLongConn = FALSE;
    sleep(1);
    while (FALSE == m_bRelease )
    {
        usleep(1000);
    }
   
    if (NULL != m_poLongConn)
    {
        m_poLongConn->UninitLongConn();
        m_poLongConn->Release();
        m_poLongConn = NULL;
    }
}

HRESULT CQueryNetIO::RestartLongConn()
{
    if(NULL != m_poLongConn)
    {
        StopLongConn();
    }
    
    // 重新初始化长链接
    m_poLongConn                                = CreateLongConnObj();
    if (0 == m_poLongConn)
    {
        TSE_LOG_ERROR(m_poLog, ("QueryNetIO Create longconn fail"));
        return S_FAIL;
    }
    m_hListenSock = CreateListenSocket(m_szIp,m_uwPort);
    if (m_hListenSock < 0)
    {
        TSE_LOG_ERROR(m_poLog, ("QueryNetIO Create listen socket fail"));
        return S_FAIL;
    }
    if (FALSE == m_poLongConn->InitLongConn(this, 1024, m_hListenSock, LONG_CONN_TIMEOUT_QUERY))
    {
        TSE_LOG_ERROR(m_poLog, ("QueryNetIO Init LongConn fail"));
        return S_FAIL;
    }
    return S_OK;
}

TVOID    CQueryNetIO::OnTasksFinishedCallBack(LTasksGroup *pstTasksGrp)
{// do nothing
    return;
}

TVOID    CQueryNetIO::OnUserRequest(LongConnHandle stHandle, const TUCHAR *pszData, TUINT32 udwDataLen, TINT32 &dwWillResponse)
{
    if (FALSE == m_bServNew)
    {
        TSE_LOG_DEBUG(m_poLog, ("QueryNetIO on user request but service not started"));
        return;
    }

    TUINT16            uwServType                    = 0;
    TUINT32            udwReqSeq                    = 0;

    // 1> Get Service Type
    m_poUnpack->UntachPackage();
    m_poUnpack->AttachPackage((TUCHAR*)pszData, udwDataLen);
    uwServType                                   = m_poUnpack->GetServiceType();
    udwReqSeq                                    = m_poUnpack->GetSeq();

    TSE_LOG_DEBUG(m_poLog, ("onUserRequest Handle [seq=%u] [hadle:%lu] [sn:%u]", udwReqSeq, (TUINT64)(stHandle.handle), stHandle.SerialNum));
	switch (uwServType)
	{
	case EN_SERVICE_TYPE_HU2LOCK__GET_REQ:
	case EN_SERVICE_TYPE_HU2LOCK__RELEASE_REQ:
		if (S_OK != OnRequest(stHandle, udwReqSeq, pszData, udwDataLen, uwServType))
		{
			TSE_LOG_ERROR(m_poLog, ("QueryNetIO Handle data update fail [seq=%u]", udwReqSeq));
		}
		break;
	default :
		TSE_LOG_ERROR(m_poLog, ("QueryNetIO Unknown service type=[%u] [seq=%u] ", uwServType, udwReqSeq));
	}

	dwWillResponse = TRUE;
}

HRESULT    CQueryNetIO::OnRequest(LongConnHandle stHandle, TUINT32 udwReqSeqNum, 
                            const TUCHAR *pszData, TUINT32 udwDataLen, TUINT16 uwServType)
{
    TSE_LOG_DEBUG(m_poLog, ("new data update [seq=%u]", udwReqSeqNum));

	// 1> Check DataLen
	if (udwDataLen > MAX_NET_IO_REQ_PACKAGE_LEN)
	{
		TSE_LOG_ERROR(m_poLog, ("recv dataLen more than MAX_NET_IO_REQ_PACKAGE_LEN [%u:%u] [seq=%u]" ,
			udwDataLen, MAX_NET_IO_REQ_PACKAGE_LEN, udwReqSeqNum));

		SendBackErr(stHandle, EN_SERVICE_TYPE_HS2HU__SEARCH_RSP, udwReqSeqNum, EN_RET_CODE__PACKAGE_LEN_OVERFLOW);
		return S_FAIL;
	}

	// 2> Get SessionWrapper
	TUINT64 uddwTimeStart = CTimeUtils::GetCurTimeUs();
	CSessionWrapper        * poSessionWrapper    = 0;
	if (S_OK != CWorkSessionMgr::Instance()->WaitTimeSession(&poSessionWrapper))
	{
		TSE_LOG_ERROR(m_poLog, ("Get WorkSessionWrapper failed [seq=%u]", udwReqSeqNum));
		SendBackErr(stHandle, EN_SERVICE_TYPE_HS2HU__SEARCH_RSP, udwReqSeqNum, EN_RET_CODE__GET_SESSION_OVERTIME);
		return S_FAIL;
	}

	// 3> copy data to session
	poSessionWrapper->m_stHandle = stHandle;
	poSessionWrapper->m_udwClientSeq    = udwReqSeqNum;
	poSessionWrapper->m_udwServiceType = uwServType;
	poSessionWrapper->m_uddwReqTimeUs = uddwTimeStart;
	memcpy(poSessionWrapper->m_szReqBuf, pszData, udwDataLen);
	poSessionWrapper->m_udwReqBufLen = udwDataLen;

	// 4> push session to work queue
	m_poWorkQueue->WaitTillPush(poSessionWrapper);

    return S_OK;    
}


SOCKET    CQueryNetIO::CreateListenSocket(TCHAR* pszListenHost,    TUINT16 uwPort)
{
	// 1> 申请SOCKET
	SOCKET lSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (lSocket < 0)
	{
		return -1;
	}

	// 2> 设置端口可重用
	int option = 1;
	if ( setsockopt ( lSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof( option ) ) < 0 ) 
	{ 
		close(lSocket);
		return -1;
	}

	// 3> 绑定端口
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sockaddr_in));
	sa.sin_port = htons(uwPort);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(pszListenHost);
	int rv = bind(lSocket, (struct sockaddr *) &sa,  sizeof(sa));

	if (rv < 0)
	{
		close(lSocket);
		return -1;
	}

	// 4> 监听
	rv = listen(lSocket, uwPort);

	if (rv < 0)
	{
		close(lSocket);
		return -1;
	}

    return lSocket;
}

HRESULT    CQueryNetIO::CloseListenSocket()
{
    if (m_hListenSock >= 0)
    {
        close(m_hListenSock);
    }
    return S_OK;
}

HRESULT    CQueryNetIO::GetIp2PortByHandle(LongConnHandle stHandle, TUINT16 *puwPort, TCHAR **ppszIp)
{
    TUINT32 udwHost = 0;
    TUINT16 uwPort = 0;

    m_poLongConn->GetPeerName(stHandle, &udwHost, &uwPort);
    *puwPort = ntohs(uwPort);
    *ppszIp = inet_ntoa(*(in_addr*)&udwHost);
    return S_OK;
}

HRESULT CQueryNetIO::SendBackErr(LongConnHandle stHandle, TUINT16 uwSrvType, TUINT32 udwSeqNum, TINT32 dwErrorCode)
{
    TUCHAR *pszPackBuf = NULL;
    TUINT32 udwPackBufLen = 0;
    LTasksGroup stTasks;

    // 1> 打包
    m_poPack->ResetContent();
    m_poPack->SetServiceType(uwSrvType);
    m_poPack->SetSeq(udwSeqNum);
    m_poPack->SetKey(EN_GLOBAL_KEY__RES_CODE, dwErrorCode);

    m_poPack->GetPackage(&pszPackBuf, &udwPackBufLen);
    stTasks.m_Tasks[0].SetConnSession(stHandle);
    stTasks.m_Tasks[0].SetSendData(pszPackBuf, udwPackBufLen);
    stTasks.m_Tasks[0].SetNeedResponse(0);
    stTasks.SetValidTasks(1);
    if(FALSE == m_poLongConn->SendData(&stTasks))
    {
        TSE_LOG_ERROR(m_poLog, ("Send data failed [seq=%u]", udwSeqNum));
        return S_FAIL;
    }
    return S_OK;
}

