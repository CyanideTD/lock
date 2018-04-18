#include "lock_group.h"
#include "time_utils.h"
#include "my_define.h"

TINT32 CLockGroup::Init( TUINT32 udwTimeoutMs )
{
	m_udwTimeoutMs = udwTimeoutMs;

	// 初始化锁
	if (pthread_mutex_init(&m_oCacheMutex, NULL) < 0)
	{
		return -10;
	}

	// 初始化hash map
	m_pobjHash = new CLockTypeMap;
    m_pobjHash->clear();

    return 0;
}

TINT32 CLockGroup::GetLock( SLockNode *pstNode, TUINT32 udwKeyNum )
{
	TINT32 dwRetCode = 0;
	TUINT64 uddwCurTimeMs = 0;
	SHashVal *pValList[MAX_LOCK_KEY_NUM_IN_ONE_REQ];
    CLockKeyMap *pKeyMapList[MAX_LOCK_KEY_NUM_IN_ONE_REQ];
	TUINT32 idx = 0;

	assert(udwKeyNum <= MAX_LOCK_KEY_NUM_IN_ONE_REQ);

	pthread_mutex_lock(&m_oCacheMutex);

	uddwCurTimeMs = CTimeUtils::GetCurTimeMs();
	for(idx = 0; idx < udwKeyNum; idx++)
	{
		ELockStatus eStatus = LockFind(&pstNode[idx], uddwCurTimeMs, pKeyMapList[idx], pValList[idx]);
		if(eStatus == EN_LOCK_STATUS__LOCKING) // 加锁中，未超时
		{
			break;
		}
	}
	if(idx >= udwKeyNum)
	{
		for(idx = 0; idx < udwKeyNum; idx++)
		{
			if(pValList[idx] == NULL)
			{
				SHashVal stVal;
				stVal.m_uddwTime = uddwCurTimeMs;
				pKeyMapList[idx]->insert(make_pair(pstNode->m_ddwKey, stVal));
            }
			else
			{
				pValList[idx]->m_uddwTime = uddwCurTimeMs;
			}
		}
	}
	else
	{
		dwRetCode = -1;
	}

	pthread_mutex_unlock(&m_oCacheMutex);

	return dwRetCode;
}

TINT32 CLockGroup::ReleaseLock( SLockNode *pstNode, TUINT32 udwKeyNum )
{
    CLockKeyMap *pobjKeyMap = NULL;

    pthread_mutex_lock(&m_oCacheMutex);
    for(TUINT32 idx = 0; idx < udwKeyNum; idx++)
    {
        CLockTypeIterator it = m_pobjHash->find(pstNode[idx].m_dwType);
        if(it != m_pobjHash->end())
        {
            pobjKeyMap = it->second;
            CLockKeyIterator key_it = pobjKeyMap->find(pstNode[idx].m_ddwKey);
            if(key_it != pobjKeyMap->end()) //找到
            {
                key_it->second.m_uddwTime = 0;
            }
        }
    }
    pthread_mutex_unlock(&m_oCacheMutex);

    return 0;
}

ELockStatus CLockGroup::LockFind( SLockNode *pstNode, TUINT64 uddwCurTimeMs, CLockKeyMap *&pobjKeyMap, SHashVal *&pstVal )
{
    ELockStatus eStatus = EN_LOCK_STATUS__NOT_EXIST;
    pstVal = NULL;
    pobjKeyMap = NULL;

    CLockTypeIterator it = m_pobjHash->find(pstNode->m_dwType);
    if(it == m_pobjHash->end())
    {
        //动态新增type
        pobjKeyMap = new CLockKeyMap;
        pobjKeyMap->clear();
        m_pobjHash->insert(make_pair(pstNode->m_dwType, pobjKeyMap));

        pstVal = NULL;
        eStatus = EN_LOCK_STATUS__NOT_EXIST;
    }
    else
    {
        pobjKeyMap = it->second;
        CLockKeyIterator key_it = pobjKeyMap->find(pstNode->m_ddwKey);
        if(key_it != pobjKeyMap->end()) //找到
        {
            pstVal = &key_it->second;
            if(pstVal->m_uddwTime == 0)// 已解锁
            {
                eStatus = EN_LOCK_STATUS__AVAILABLE;
            }
            else if(key_it->second.m_uddwTime + m_udwTimeoutMs > uddwCurTimeMs) // 加锁中，未超时
            {
                eStatus = EN_LOCK_STATUS__LOCKING;
            }
            else // 加锁中，超时
            {
                eStatus = EN_LOCK_STATUS__EXPIRE;
            }
        }
        else//没找到
        {
            pstVal = NULL;
            eStatus = EN_LOCK_STATUS__NOT_EXIST;
        }
    }
    return eStatus;
}
