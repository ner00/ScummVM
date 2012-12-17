/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#define XEEN_MAZE_SOURCE

#include "xeen/game.h"
#include "xeen/ui/basicwindows.h"
#include "xeen/utility.h"

#include "xeen/maze/eventlist_.h"
#include "xeen/maze/map.h"

XEEN::Maze::EventList::EventList(Map* parent, FilePtr data) : _parent(parent), _data(data)
{
    if(_data)
    {
        while(!_data->eos())
        {
            uint8 length = _data->readByte();

            if(length < 5)
            {
                break;
            }

            const uint8 x = _data->readByte();
            const uint8 y = _data->readByte();
            const uint8 f = _data->readByte();
            const uint8 l = _data->readByte();
            const uint32 key = (x << 16) | (y << 8);

            (void)f;

            Event& ev = _events[key];

            if(enforce(l == ev.lines.size()))
            {
                ev.lines.push_back(_data->pos() - 5);
            }
           
            _data->seek(_data->pos() + length - 4);
        }
    }
    else
    {
        _events.clear();
        markInvalid();
    }
}

void XEEN::Maze::EventList::runEventAt(uint8 x, uint8 y, Direction facing)
{
    XEEN_VALID();

    const uint32 key = (x << 16) | (y << 8);

    if(_events.contains(key))
    {
        Event& ev = _events[key];

        debug("\nEVENT START");
        for(uint32 i = 0; i != ev.lines.size(); i ++)
        {
            runEventLine(ev.lines[i]);
        }
    }
}

int32 XEEN::Maze::EventList::runEventLine(int32 off)
{
    XEEN_VALID();

    if(valid(_parent))
    {
        const uint8 length = _data->getByteAt(off);
        const uint8 opcode = _data->getByteAt(off + 5);

        switch(opcode)
        {
            case 0x00: { return evNOP(off); }
            case 0x01: { return evMESSAGE(off); }
            case 0x02: { return evMAPTEXT(off); }
            case 0x03: { return evMAPTEXT(off); }
            case 0x04: { return evMAPTEXT(off); }
            case 0x05: { return evNPC(off); }
            case 0x07: { return evTELEPORT(off); }
            case 0x08: { return evIF(off); }
            case 0x09: { return evIF(off); }
            case 0x0A: { return evIF(off); }
            case 0x1F: { return evTELEPORT(off); }
            case 0x27: { return evMAPTEXT(off); }
            case 0x29: { return evMESSAGE(off); }
            case 0x35: { return evMESSAGE(off); }
            default: debug("EV: %02X", opcode); return 1;
        }
    }

    return 0;
}

int32 XEEN::Maze::EventList::evNOP(uint32 offset)
{
    return 1;
}

int32 XEEN::Maze::EventList::evMAPTEXT(uint32 offset)
{
    // TODO
    debug("MAPTEXT");
    debug("%s", _parent->getString(_data->getByteAt(offset + 6)));
    return 1;
}

int32 XEEN::Maze::EventList::evMESSAGE(uint32 offset)
{
    // TODO
    debug("MESSAGE");
    debug("%s", _parent->getString(_data->getByteAt(offset + 6)));
    return 1;
}

int32 XEEN::Maze::EventList::evNPC(uint32 offset)
{
    // TODO
    debug("NPC");

    const char* name = _parent->getString(_data->getByteAt(offset + 6));
    const char* msg = _parent->getString(_data->getByteAt(offset + 7));

    _parent->getGame()->showWindow(new NPCWindow(_parent->getGame(), name, msg));

    return 1;
}

int32 XEEN::Maze::EventList::evTELEPORT(uint32 offset)
{
    // TODO
    debug("TELEPORT");
    return 1;
}

int32 XEEN::Maze::EventList::evIF(uint32 offset)
{
    // TODO
    debug("IF");
    return 1;
}