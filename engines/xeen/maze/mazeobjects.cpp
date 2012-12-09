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


#include "xeen/game.h"
#include "xeen/utility.h"

#include "xeen/maze/mazeobjects.h"

///
/// MazeObjects
///
XEEN::MazeObjects::MazeObjects(uint16 mapNumber) : _data(XEENgame.getFile(CCFileId("MAZE%s%03d.MOB", (mapNumber < 100) ? "0" : "X", mapNumber), true))
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
                
                // TODO: Assert object entry validity: id < 16, facing < 4 ?
            }
        }
        
        if(onList != 3 || dataPos != _data->size())
        {
            markInvalidAndClean("Check maze object loading.");
        }
    }
    else
    {
        markInvalid();
    }
}

XEEN::MazeObjects::~MazeObjects()
{
    // TODO: Store the values back into the save game
}

void XEEN::MazeObjects::cleanse()
{
    memset(_offsets, 0xFF, sizeof(_offsets));
    memset(_counts, 0xFF, sizeof(_counts));
}

bool XEEN::MazeObjects::getObjectAt(const Common::Point& position, Entry& data)
{
    XEEN_VALID_RET(false);

    for(unsigned i = 0; i != _counts[0]; i ++)
    {
        const uint8 x = _data->getByteAt(_offsets[0] + (i * 4));
        const uint8 y = _data->getByteAt(_offsets[0] + (i * 4) + 1);
        const uint8 type = _data->getByteAt(_offsets[0] + (i * 4) + 2);    
        const uint8 facing = _data->getByteAt(_offsets[0] + (i * 4) + 3);
    
        if(x == position.x && y == position.y)
        {
            data.position = Common::Point(x, y);
            data.facing = facing;
            data.id = _data->getByteAt(0 + type);
            
            return true;
        }
    }
    
    return false;
}
