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

#include "xeen/maze/objects_.h"

XEEN::Maze::Objects::Objects(uint16 mapNumber) : _data(XEENgame.getFile(CCFileId("MAZE%s%03d.MOB", (mapNumber < 100) ? "0" : "X", mapNumber), true))
{
    memset(_offsets, 0xFF, sizeof(_offsets));
    memset(_counts, 0xFF, sizeof(_counts));
    
    if(_data)
    {
        unsigned onList = 0;
        int32 dataPos = 48;
        
        _offsets[0] = dataPos;
        
        for(; dataPos < _data->size() && onList != 3; dataPos += 4)
        {
            if(_data->getU32At(dataPos) == 0xFFFFFFFF)
            {
                onList ++;
                
                if(onList != 3)
                {
                    _offsets[onList] = dataPos + 4;
                }
            }
            else
            {
                _counts[onList] ++;

                if(_data->getByteAt(dataPos + 2) >= 16 || _data->getByteAt(dataPos + 3) >= 4)
                {
                    markInvalidAndClean("Maze object ID or Facing invalid?");
                    return;
                }
            }
        }
        
        if(onList != 3 || dataPos != _data->size())
        {
            markInvalidAndClean("Check maze object loading.");
        }
    }
    else
    {
        markInvalidAndClean("MAZEXXXX.MOB not found.");
    }
}

void XEEN::Maze::Objects::cleanse()
{
    memset(_offsets, 0xFF, sizeof(_offsets));
    memset(_counts, 0xFF, sizeof(_counts));
}

bool XEEN::Maze::Objects::getObjectAt(uint8 x, uint8 y, Entry& data) const
{
    XEEN_VALID_RET(false);

    for(unsigned i = 0; i != _counts[0]; i ++)
    {
        const uint8 ox = _data->getByteAt(_offsets[0] + (i * 4));
        const uint8 oy = _data->getByteAt(_offsets[0] + (i * 4) + 1);
    
        if(ox == x && oy == y)
        {
            data.id = _data->getByteAt(_data->getByteAt(_offsets[0] + (i * 4) + 2));
            data.facing = _data->getByteAt(_offsets[0] + (i * 4) + 3);
            
            return true;
        }
    }
    
    return false;
}
