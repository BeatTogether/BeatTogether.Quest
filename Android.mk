HOST_NAME ?= master.beattogether.systems
PORT ?= 2328
STATUS_URL ?= http://master.beattogether.systems/status
LOCAL_PATH := $(call my-dir)

TARGET_ARCH_ABI := arm64-v8a

include $(CLEAR_VARS)
LOCAL_MODULE := hook
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
# Creating prebuilt for dependency: beatsaber-hook - version: 3.4.4
include $(CLEAR_VARS)
LOCAL_MODULE := beatsaber-hook_3_4_4
LOCAL_EXPORT_C_INCLUDES := extern/beatsaber-hook
LOCAL_SRC_FILES := extern/libbeatsaber-hook_3_4_4.so
LOCAL_CPP_FEATURES += exceptions
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: modloader - version: 1.2.3
include $(CLEAR_VARS)
LOCAL_MODULE := modloader
LOCAL_EXPORT_C_INCLUDES := extern/modloader
LOCAL_SRC_FILES := extern/libmodloader.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: codegen - version: 0.18.2
include $(CLEAR_VARS)
LOCAL_MODULE := codegen
LOCAL_EXPORT_C_INCLUDES := extern/codegen
LOCAL_SRC_FILES := extern/libcodegen.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: capstone - version: 0.1.0
include $(CLEAR_VARS)
LOCAL_MODULE := capstone
LOCAL_EXPORT_C_INCLUDES := extern/capstone
LOCAL_SRC_FILES := extern/libcapstone.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := beattogether
LOCAL_SRC_FILES += $(call rwildcard,src/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.c)
LOCAL_SHARED_LIBRARIES += modloader
LOCAL_SHARED_LIBRARIES += beatsaber-hook_3_4_4
LOCAL_SHARED_LIBRARIES += codegen
LOCAL_STATIC_LIBRARIES += capstone
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += -I'extern/libil2cpp/il2cpp/libil2cpp' -DID='"BeatTogether"' -DVERSION='"$(VERSION)"' -DHOST_NAME='"$(HOST_NAME)"' -DPORT='$(PORT)' -DSTATUS_URL='"$(STATUS_URL)"' -I'./shared' -I'./extern' -isystem'./extern/codegen/include'
LOCAL_CPPFLAGS += -std=c++2a
LOCAL_C_INCLUDES += ./include ./src
include $(BUILD_SHARED_LIBRARY)
