LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := demo
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_CPPFLAGS += -std=c++11 -fexceptions
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../sr

LOCAL_SRC_FILES := ../../../../ApplicationAndroid.cpp

LOCAL_LDLIBS := -llog -landroid -latomic

include $(BUILD_SHARED_LIBRARY)
