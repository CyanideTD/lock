BASE_PATH := ../../../base
LIB_PATH := $(BASE_PATH)/lib
project.targets += lock_svr
project.extra_warning := -Wno-write-strings -fpermissive
project.optimize_flags :=
lock_svr.path := ../bin
lock_svr.name := lock_svr
lock_svr.defines := TIXML_USE_STL _UTF8_CODE_
lock_svr.sources := $(wildcard *.cpp)
lock_svr.includes := $(BASE_PATH) .
lock_svr.ldadd := $(LIB_PATH)/libSvrPub.a $(LIB_PATH)/libWtseBase.a -pthread

include $(BASE_PATH)/Generic.mak

