LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := nativeRAxML
LOCAL_SRC_FILES := nativeRAxML.c android_helpers.c \
	phylogenetic-likelihood-library/treeIO.c \
	phylogenetic-likelihood-library/bipartitionList.c \
	phylogenetic-likelihood-library/trash.c
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
