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
#include "xeen/utility.h"

#include "xeen/maze/eventlist_.h"
#include "xeen/maze/map.h"

XEEN::Maze::EventList::EventList(Map* parent, FilePtr data) : _parent(parent), _data(data)
{
    memset(_eventOffset, 0xFF, sizeof(_eventOffset));

    if(_data)
    {
        uint8 lastX = 255;
        uint8 lastY = 255;

        while(!_data->eos())
        {
            uint8 length = _data->readByte();

            if(length < 5)
            {
                break;
            }

            uint8 x = _data->readByte();
            uint8 y = _data->readByte();

            if(x != lastX || y != lastY)
            {
                lastX = x;
                lastY = y;

                _eventOffset[y * MAX_MAP_WIDTH + x] = _data->pos() - 3;
            }
           
            _data->seek(_data->pos() + length - 2);
        }
    }
    else
    {
        _data.reset();
        markInvalid();
    }
}

void XEEN::Maze::EventList::runEventAt(uint8 x, uint8 y, Direction facing)
{
    XEEN_VALID();

    const int32 off = _eventOffset[y * MAX_MAP_WIDTH + x];

    if(off >= 0)
    {
        const uint8 dir = _data->getByteAt(off + 3);

        if(dir == 4 || dir == facing)
        {
            runEventLine(off);
        }
    }
}

uint8 XEEN::Maze::EventList::runEventLine(int32 off)
{
    XEEN_VALID();

    if(valid(_parent))
    {
        _data->seek(off);
    
        uint8 length = _data->readByte();
        _data->seek(4, SEEK_CUR);
    
        uint8 opcode = _data->readByte();
        switch(opcode)
        {
            case 0x00: return 0;
            case 0x01: return 0;
            case 0x02: debug("%s", _parent->getString(_data->readByte())); return 0;
        }
    }

    return 0;
}
