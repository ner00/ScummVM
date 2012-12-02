MODULE := engines/xeen

MODULE_OBJS := \
    xeen.o \
    detection.o \
    ccfile.o \
    characters.o \
    party.o \
    game.o \
    archive/toc.o \
    graphics/font.o \
    graphics/sprite.o \
    graphics/spritemanager.o \
    maze/eventlist.o \
    maze/map.o \
    maze/mazeobjects.o \
    maze/mazetext.o \
    ui/window.o \
    ui/characterwindow.o \
    ui/gamewindow.o

# This module can be built as a plugin
ifeq ($(ENABLE_XEEN), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
