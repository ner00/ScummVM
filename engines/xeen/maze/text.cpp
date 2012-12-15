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
#include "xeen/maze/text_.h"

XEEN::Maze::Text::Text(FilePtr data) : _data(data)
{
    memset(_stringOffsets, 0xFF, sizeof(_stringOffsets));

    if(_data)
    {    
        uint32 foundStrings = 1;
        _stringOffsets[0] = 0;
        
        for(int32 offset = 1; offset < _data->size() - 1; offset ++)
        {
            if(_data->getData()[offset] == 0)
            {
                _stringOffsets[foundStrings ++] = offset + 1;
            }
        }
    }
    else
    {
        markInvalid();
    }
}

const char* XEEN::Maze::Text::getString(uint32 id) const
{
    XEEN_VALID();

    if(enforce(id < MAX_STRINGS))
    {
        return (_stringOffsets[id] != 0xFFFF) ? (const char*)(&_data->getData()[_stringOffsets[id]]) : "";
    }
    
    return "";
}
