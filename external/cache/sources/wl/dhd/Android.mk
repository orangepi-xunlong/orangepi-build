LOCAL_PATH:= $(call my-dir)

#OBJS_c := dhdu.c dhdu_linux.c bcmutils.c miniopt.c
OBJS_c := dhdu.c dhdu_linux.c ucode_download.c
#OBJS_c += ../shared/bcmutils.c ../shared/miniopt.c  ../shared/wlu_pipe.c ../shared/wlu_pipe_linux.c ../shared/wlu_client_shared.c
INCLUDES := $(LOCAL_PATH)/../include $(LOCAL_PATH)/../shared
L_CFLAGS := -DBCMWPA2 -DWLCNT -DWLBTAMP -Wextra -DWLPFN -DWLPFN_AUTO_CONNECT -DLINUX -DRWLASD -DRWL_SOCKET -DRWL_DONGLE -DRWL_WIFI
L_CFLAGS += -DSDTEST -DTARGETENV_android -Dlinux -DLINUX

include $(CLEAR_VARS)
LOCAL_MODULE := dhd
LOCAL_MODULE_TAGS := debug tests
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_STATIC_LIBRARIES := libshared
LOCAL_CFLAGS = $(L_CFLAGS)
LOCAL_SRC_FILES := $(OBJS_c)
LOCAL_C_INCLUDES := $(INCLUDES)
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := libdhd
LOCAL_MODULE_TAGS := debug tests
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_STATIC_LIBRARIES := libshared
LOCAL_CFLAGS = $(L_CFLAGS)
LOCAL_CFLAGS += -DLIB
LOCAL_SRC_FILES := $(OBJS_c)
LOCAL_C_INCLUDES := $(INCLUDES)
include $(BUILD_STATIC_LIBRARY)
