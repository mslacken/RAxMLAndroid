LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := nativeRAxML
LOCAL_SRC_FILES := nativeRAxML.c android_helpers.c \
	phylogenetic-likelihood-library/treeIO.c \
	phylogenetic-likelihood-library/bipartitionList.c \
	phylogenetic-likelihood-library/trash.c \
	phylogenetic-likelihood-library/models.c \
	phylogenetic-likelihood-library/optimizeModel.c \
	phylogenetic-likelihood-library/evaluateGenericSpecial.c \
	phylogenetic-likelihood-library/newviewGenericSpecial.c \
	phylogenetic-likelihood-library/evaluatePartialGenericSpecial.c \
	phylogenetic-likelihood-library/makenewzGenericSpecial.c \
	phylogenetic-likelihood-library/topologies.c \
	phylogenetic-likelihood-library/searchAlgo.c \
	phylogenetic-likelihood-library/randomTree.c \
	phylogenetic-likelihood-library/restartHashTable.c \
	phylogenetic-likelihood-library/fastDNAparsimony.c

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
