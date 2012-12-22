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

#include "xeen/maze/map.h"
#include "xeen/maze/objects_.h"

XEEN::Maze::Objects::Objects(FilePtr data) : _data(data)
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

                if(_data->getByteAt(dataPos + 2) >= 16)//TODO: || _data->getByteAt(dataPos + 3) >= 4) (Fails on Winterkill where a monster has facing 6?)
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

bool XEEN::Maze::Objects::getObjectAt(const Common::Point& pos, ObjectEntry& data) const
{
    XEEN_VALID();

    for(unsigned i = 0; i != _counts[0]; i ++)
    {
        const Common::Point objPos(_data->getI8At(_offsets[0] + (i * 4)), _data->getI8At(_offsets[0] + (i * 4) + 1));
    
        if(pos == objPos)
        {
            data.id = _data->getByteAt(_data->getByteAt(_offsets[0] + (i * 4) + 2));
            data.facing = _data->getByteAt(_offsets[0] + (i * 4) + 3);
            
            return true;
        }
    }
    
    return false;
}

uint32 XEEN::Maze::Objects::getMonstersAt(const Common::Point& pos, NonNull<ObjectEntry> data) const
{
    uint32 foundMonsters = 0;

    for(unsigned i = 0; i != _counts[1] && foundMonsters != 3; i ++)
    {
        const Common::Point objPos(_data->getI8At(_offsets[0] + (i * 4)), _data->getI8At(_offsets[0] + (i * 4) + 1));
    
        if(pos == objPos)
        {
            data[foundMonsters++].id = _data->getByteAt(16 + _data->getByteAt(_offsets[1] + (i * 4) + 2));
        }
    }

    return foundMonsters;
}
