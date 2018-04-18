#ifndef _BASE_DEF_H_
#define _BASE_DEF_H_

#include <string.h>
#include "base/common/wtsetypedef.h"
#include "svrpublib/ILongConn.h"
#include <sys/time.h>
#include <unistd.h>

enum ECommandType
{
    UNKNOWN_CMD = 0,
    CMD_SET = 1,        //重置一个key的value
    CMD_ADD = 2,        //追加
    CMD_DEL_USER = 3,	//删除一个key及其value
    CMD_DEL_VAL = 4,	//删除valve中的某些值
};

enum EDataType
{
    EN_DATA_TYPE__UNKNOWN =0,
    EN_DATA_TYPE__UIN,
    EN_DATA_TYPE__GROUP,
	EN_DATA_TYPE__GLOBAL,
};

enum RelationDataServiceType
{
	// hu <---> hs
	EN_HU2HS__BEGIN = 3000,
	EN_SERVICE_TYPE_HU2HS__SEARCH_REQ,
	EN_SERVICE_TYPE_HU2HS__UPDT_REQ,

	EN_HS2HU__BEGIN = 3100,
	EN_SERVICE_TYPE_HS2HU__SEARCH_RSP,
	EN_SERVICE_TYPE_HS2HU__UPDT_RSP,

	// hu <---> lock
	EN_HU2LOCK__BEGIN = 4000,
	EN_SERVICE_TYPE_HU2LOCK__GET_REQ,
	EN_SERVICE_TYPE_HU2LOCK__RELEASE_REQ,

	EN_LOCK2HU__BEGIN = 4100,
	EN_SERVICE_TYPE_LOCK2HU__GET_RSP,
	EN_SERVICE_TYPE_LOCK2HU__RELEASE_RSP,
};

enum EDBPROXYKeySet
{
	//////////////////////////
	// GLOBAL KEY
	EN_GLOBAL_KEY__BEGIN = 0,
	EN_GLOBAL_KEY__RES_CODE,
	EN_GLOBAL_KEY__RES_COST_TIME,
	EN_GLOBAL_KEY__RES_TOTAL_RES_NUM,
	EN_GLOBAL_KEY__RES_CUR_RES_NUM,
	EN_GLOBAL_KEY__RES_BUF,

	// hu <-> hs
	EN_KEY_HU2HS__BEGIN = 5000,
	EN_KEY_HU2HS__REQ_NUM,
	EN_KEY_HU2HS__REQ_BUF,

	EN_KEY_HS2HU__BEGIN = 5100,
	EN_KEY_HS2HU__RES_DB_TYPE,
	EN_KEY_HS2HU__RES_TBL_TYPE,
	EN_KEY_HS2HU__RES_INDEX_OPENTYPE,
	EN_KEY_HS2HU__RES_COMMAND,

	// hu -> lock
	EN_KEY_HU2LOCK__BEGIN = 6000,
	EN_KEY_HU2LOCK__REQ_KEY_NUM,
	EN_KEY_HU2LOCK__REQ_KEY_LIST,
	EN_KEY_HU2LOCK__REQ_TIMEOUT_US,

	// lock -> hu
	EN_KEY_LOCK2HU__BEGIN = 6100,
};

enum ErrorCode
{
	EN_RET_CODE__SUCCESS = 0,
	EN_RET_CODE__PACKAGE_LEN_OVERFLOW,
	EN_RET_CODE__GET_SESSION_OVERTIME,
	EN_RET_CODE__PARSE_PACKAGE_ERR,
	EN_RET_CODE__MYSQL_SEARCH_FAILED,
	EN_RET_CODE__HS_PARAM_ERROR,
	EN_RET_CODE__MYSQL_SQL_CONNECT_FAILED,
	EN_RET_CODE__MYSQL_SQL_SEARCH_FAILED,
	EN_RET_CODE__SEND_FAILED,


	EN_RET_CODE__USER_DATA_IS_USING = 100,
};


//获取当前时间,以us返回
inline TUINT64 rdtsc()
{
	struct timeval  tv;
	struct timezone tz;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime     = 0;
	gettimeofday(&tv, &tz);
	TUINT64 time = tv.tv_sec * (TUINT64)1000000 + tv.tv_usec;
	return time;
}

#endif
