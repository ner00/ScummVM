MODULE := engines/xeen

MODULE_OBJS := \
    xeen.o \
    detection.o \
    characters.o \
    party.o \
    game.o \
    archive/archive.o \
    archive/toc.o \
    graphics/font.o \
    graphics/sprite.o \
    graphics/manager.o \
    maze/eventlist.o \
    maze/map.o \
    maze/manager.o \
    maze/objects.o \
    maze/objectdata.o \
    maze/monsterdata.o \
    maze/segment.o \
    maze/text.o \
    event/castspell.o \
    event/selectspell.o \
    event/gameinfo.o \
    event/quickreference.o \
    ui/window.o \
    ui/characteraction.o \
    ui/characterwindow.o \
    ui/gamewindow.o \
    ui/scrollwindow.o \
    ui/characterstatuswindow.o \
    ui/npcwindow.o \
    ui/messagewindows.o \
    ui/movementwindow.o

# This module can be built as a plugin
ifeq ($(ENABLE_XEEN), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
