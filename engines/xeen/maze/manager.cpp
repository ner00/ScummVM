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

#include "xeen/utility.h"
#include "xeen/game.h"

#include "xeen/maze/map.h"
#include "xeen/maze/manager.h"
#include "xeen/maze/segment_.h"
#include "xeen/maze/monsterdata_.h"

XEEN::Maze::Manager::Manager(Valid<Game> parent) : GameHolder(parent)
{
    memset(_maps, 0, sizeof(_maps));
    memset(_segments, 0, sizeof(_segments));

    const Game::Type gt = getGame()->getGameType();

    memset(_objectData, 0, sizeof(_objectData));

    if(gt == Game::CLOUDS)
    {
        Common::File cloudsdat;
        if(cloudsdat.open("CLOUDS.DAT") && cloudsdat.size() == sizeof(_objectData))
        {
            cloudsdat.read(&_objectData, sizeof(_objectData));
        }
        else
        {
            markInvalid("CLOUDS.DAT not found or mis-sized.");
            return;
        }
    }
    else
    {
        markInvalid("Unrecognized game type.");
        return;
    }


    _monsterData = new MonsterData(this);
}

XEEN::Maze::Manager::~Manager()
{
    for(int i = 0; i != 256; i ++)
    {
        delete _maps[i];
        delete _segments[i];
    }

    delete _monsterData;
}

XEEN::Maze::Map* XEEN::Maze::Manager::getMap(uint16 id)
{
    if(!_maps[id])
    {
        _maps[id] = new Map(this, id);
    }
    
    return _maps[id];
}

XEEN::Maze::Segment* XEEN::Maze::Manager::getSegment(uint16 id)
{
    if(!_segments[id])
    {
        CCFileId mapID("MAZE%s%03d.DAT", (id < 100) ? "0" : "X", id);

        _segments[id] = new Segment(this, getGame()->getFile(mapID, true));
        _segments[id]->loadSurrounding(); //< Can't be done is Segment's constructor
    }
    
    return _segments[id];
}

XEEN::Maze::Manager::ObjectData XEEN::Maze::Manager::getObjectData(uint32 id, Direction dir) const
{
    XEEN_VALID();

    if(id < MAX_OBJECTS)
    {
        const uint32 target = id * 12 + dir;
        const ObjectData result = {_objectData[target], _objectData[target + 4], _objectData[target + 8]};
        return result;
    }
    else
    {
        static const ObjectData result = {0, 0, 0};
        debug("Object ID out of range (%d)", id);
        return result;
    }
}
