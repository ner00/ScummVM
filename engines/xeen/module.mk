MODULE := engines/xeen

MODULE_OBJS := \
    xeen.o \
    detection.o \
    ccfile.o \
    sprite.o

# This module can be built as a plugin
ifeq ($(ENABLE_XEEN), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
