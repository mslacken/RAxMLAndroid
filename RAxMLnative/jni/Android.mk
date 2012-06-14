LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := nativeRAxML
LOCAL_SRC_FILES := nativeRAxML.c

include $(BUILD_SHARED_LIBRARY)
