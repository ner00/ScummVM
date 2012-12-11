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

#include "xeen/maze/objects_.h"
#include "xeen/maze/segment_.h"

static const uint32 OFF_WALLS         = 0x000;
static const uint32 OFF_CELL_FLAGS    = 0x200;
static const uint32 OFF_SURR_MAZES    = 0x302;
static const uint32 OFF_WALL_TYPES    = 0x30E;
static const uint32 OFF_SURFACE_TYPES = 0x31E;

XEEN::Maze::Segment::Segment(uint16 mapNumber) : _data(XEENgame.getFile(CCFileId("MAZE%s%03d.DAT", (mapNumber < 100) ? "0" : "X", mapNumber), true)), _north(0), _east(0), _objects(0)
{    
    if(_data)
    {
        const uint16 northMaze = _data->getU16At(OFF_SURR_MAZES + 0);
        const uint16 eastMaze = _data->getU16At(OFF_SURR_MAZES + 2);

        if(northMaze)
        {
            _north = XEENgame.getMapManager()->getSegment(northMaze);
        }

        if(eastMaze)
        {
            _east = XEENgame.getMapManager()->getSegment(eastMaze);
        }
    }
    else
    {
        _data.reset();
        markInvalid();
    }
    
    // Load objects
    _objects = new Objects(mapNumber);
}

uint16 XEEN::Maze::Segment::getWall(uint8 x, uint8 y) const
{
    XEEN_VALID_RET(0);

    if(enforce(x < 16 && y < 16))
    {
        return _data->getU16At(OFF_WALLS + (y * 32 + x * 2));
    }

    return 0;
}

uint8 XEEN::Maze::Segment::getCellFlags(uint8 x, uint8 y) const
{
    XEEN_VALID_RET(0);

    if(enforce(x < 16 && y < 16))
    {
        return _data->getByteAt(OFF_CELL_FLAGS + (y * 16 + x));
    }

    return 0;    
}

uint8 XEEN::Maze::Segment::lookupSurface(uint8 id) const
{
    XEEN_VALID_RET(0);

    if(enforce(id < 16))
    {
        return _data->getByteAt(OFF_SURFACE_TYPES + id);
    }

    return 0;    
}

bool XEEN::Maze::Segment::getObjectAt(uint8 x, uint8 y, Objects::Entry& data) const
{
    XEEN_VALID_RET(false);

    if(valid(_objects))
    {
        return _objects->getObjectAt(x, y, data);
    }

    return false;
}
