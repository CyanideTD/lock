#ifndef _LOCK_GROUP_H_
#define _LOCK_GROUP_H_

#include <pthread.h>
#include <map>
#include "lock_def.h"

using namespace std;

typedef map<TINT64, SHashVal> CLockKeyMap;
typedef map<TINT64, SHashVal>::iterator CLockKeyIterator;
typedef map<TINT32, CLockKeyMap*> CLockTypeMap;
typedef map<TINT32, CLockKeyMap*>::iterator CLockTypeIterator;

class CLockGroup
{
private:
	CLockTypeMap	*m_pobjHash;
	pthread_mutex_t m_oCacheMutex;
	TUINT32			m_udwTimeoutMs;

public:
	TINT32 Init(TUINT32 udwTimeoutMs);

	TINT32 GetLock(SLockNode *pstNode, TUINT32 udwNum);
	TINT32 ReleaseLock(SLockNode *pstNode, TUINT32 udwNum);

    ELockStatus LockFind( SLockNode *pstNode, TUINT64 uddwCurTimeMs, CLockKeyMap *&pKeyMapList, SHashVal *&pstVal);
};




#endif