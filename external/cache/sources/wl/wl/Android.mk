LOCAL_PATH:= $(call my-dir)

OBJS_c := wlu.c \
          wlu_common.c \
          wlu_linux.c \
          wlu_cmd.c \
          wlu_iov.c \
          wlu_pipe.c \
          wlu_pipe_linux.c \
          wlu_client_shared.c \
          wlc_ppr.c \
          wlu_rates_matrix.c

INCLUDES := $(LOCAL_PATH)/../include
INCLUDES += $(LOCAL_PATH)/../shared
L_CFLAGS := -DBCMWPA2 -DWLCNT -DWLBTAMP -Wextra -DWLPFN -DWLPFN_AUTO_CONNECT -DLINUX -DRWLASD -DRWL_SOCKET -DRWL_DONGLE -DRWL_WIFI
L_CFLAGS += -DSDTEST -DTARGETENV_android -Dlinux -DLINUX -DD11AC_IOTYPES

include $(CLEAR_VARS)
LOCAL_MODULE := wl
LOCAL_MODULE_TAGS := debug tests
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_STATIC_LIBRARIES := libshared
LOCAL_CFLAGS = $(L_CFLAGS)
LOCAL_SRC_FILES := $(OBJS_c)
LOCAL_C_INCLUDES := $(INCLUDES)
include $(BUILD_EXECUTABLE)
