/************************************************************
 * Copyright (C), 1998-2009, Tencent Technology Company Limited
 *
 * �ļ�����: my_assert.h
 * ����: allenhuang
 * ����: 2009/04/27
 * �汾: 1.0
 * ģ������: �ض���assert
 * �����:
 *
 * �޸���ʷ:
 *      <author>        <time>       <version >   <desc>
 *      allenhuang   2009/04/27          1.0        ����
 **************************************************************/

#ifndef __MY_ASSERT_H__
#define __MY_ASSERT_H__

#include <assert.h>

//���¶���assert
#ifdef _DEBUG

#define Assert(condition)     {                                    \
    if (!(condition))                                             \
    {                                                            \
        printf ("_Assert: %s, %d\n",__FILE__, __LINE__);        \
        assert(condition);                                        \
    }}

#else

#define Assert(condition) ((void) 0)

#endif

#endif

