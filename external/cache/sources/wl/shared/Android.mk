LOCAL_PATH:= $(call my-dir)

OBJS_c := bcmutils.c bcmwifi_channels.c bcm_app_utils.c miniopt.c wlu_client_shared.c wlu_common.c wlu_pipe.c wlu_pipe_linux.c
INCLUDES := $(LOCAL_PATH)/../include $(LOCAL_PATH)/../wl
L_CFLAGS := -DBCMWPA2 -DWLCNT -DWLBTAMP -Wextra -DWLPFN -DWLPFN_AUTO_CONNECT -DLINUX -DRWLASD -DRWL_SOCKET -DRWL_DONGLE -DRWL_WIFI
L_CFLAGS += -DSDTEST -DTARGETENV_android -Dlinux -DLINUX -DD11AC_IOTYPES
#IFLAGS += -DIL_BIGENDIAN

include $(CLEAR_VARS)
LOCAL_MODULE := libshared
LOCAL_MODULE_TAGS := debug tests
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_CFLAGS = $(L_CFLAGS)
LOCAL_SRC_FILES := $(OBJS_c)
LOCAL_C_INCLUDES := $(INCLUDES)
#LOCAL_INCLUDES = $(LOCAL_PATH)
include $(BUILD_STATIC_LIBRARY)
