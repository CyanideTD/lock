/************************************************************
 * Copyright (C), 1998-2009, Tencent Technology Company Limited
 *
 * 文件名称: my_assert.h
 * 作者: allenhuang
 * 日期: 2009/04/27
 * 版本: 1.0
 * 模块描述: 重定义assert
 * 组成类:
 *
 * 修改历史:
 *      <author>        <time>       <version >   <desc>
 *      allenhuang   2009/04/27          1.0        创建
 **************************************************************/

#ifndef __MY_ASSERT_H__
#define __MY_ASSERT_H__

#include <assert.h>

//重新定义assert
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

