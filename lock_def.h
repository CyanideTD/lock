#ifndef _LOCK_DEF_H_
#define _LOCK_DEF_H_

#include "base/common/wtse_std_header.h"

enum ELockIdGenType
{
    EN_LOCK_ID_TYPE__UID = 0,
    EN_LOCK_ID_TYPE__AID,
    EN_LOCK_ID_TYPE__TASK_ID,
    EN_LOCK_ID_TYPE__WILD,
    EN_LOCK_ID_TYPE__THRONE,
};

enum ELockStatus
{
    EN_LOCK_STATUS__NOT_EXIST = 0,
    EN_LOCK_STATUS__EXPIRE,
    EN_LOCK_STATUS__AVAILABLE,
    EN_LOCK_STATUS__LOCKING,
};

#pragma pack(1)

struct SLockNode
{
    TINT32 m_dwType;
    TINT64 m_ddwKey;
};

struct SHashVal
{
    TUINT64 m_uddwTime;

    SHashVal& operator=(SHashVal& value)
    {
        m_uddwTime = value.m_uddwTime;
        return *this;
    }
};

#pragma pack()

#endif