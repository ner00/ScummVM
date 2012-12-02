MODULE := engines/xeen

MODULE_OBJS := \
    xeen.o \
    detection.o \
    ccfile.o \
    map.o \
    characters.o \
    party.o \
    mazetext.o \
    mazeobjects.o \
    font.o \
    window.o \
    game.o \
    archive/toc.o \
    graphics/sprite.o \
    graphics/spritemanager.o    

# This module can be built as a plugin
ifeq ($(ENABLE_XEEN), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
