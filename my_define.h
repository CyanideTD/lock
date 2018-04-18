#ifndef _MY_DEFINE_H_
#define _MY_DEFINE_H_

#define MAX_NET_IO_REQ_PACKAGE_LEN	(10<<10)
#define MAX_NET_IO_RES_PACKAGE_LEN	(100<<10)
#define MAX_FILE_NAME_LEN			(1<<10)
#define MAX_TC_CONFIG_LEN			(10<<10)

#define MAX_MSG_CONTENT_LEN	(128)
#define MAX_USER_ID_LEN		(128)

#define MAX_LOCK_KEY_NUM_IN_ONE_REQ		(10)

//QueryNetIO长连接超时 (s)
static const unsigned int LONG_CONN_TIMEOUT_QUERY        = 100;

//取session时的超时时间
static const unsigned int SESSION__WAITE_TIME_US        = (1000 * 1000);

//typedef int HRESULT;
//static const HRESULT S_OK                                = 0;
//static const HRESULT S_FAIL                              = -1;

#endif
