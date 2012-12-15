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
#include "xeen/archive/file.h"
#include "xeen/maze/monsterdata_.h"

XEEN::Maze::MonsterData::MonsterData(Valid<Manager> parent) : _parent(parent)
{
    memset(_xeenMON, 0, sizeof(_xeenMON));
    memset(_darkMON, 0, sizeof(_darkMON));

    const Game::Type gt = _parent->getGame()->getGameType();

    if(gt == Game::CLOUDS)
    {
        Common::File xeenmon;
        if(xeenmon.open("XEEN.MON"))
        {
            xeenmon.read(&_xeenMON, DAT_SIZE);
        }
        else
        {
            markInvalid("XEEN.MON not found.");
        }
    }
    else
    {
        markInvalid("Unrecognized game type.");
        assert(false);
    }
}

XEEN::NonNull<const char> XEEN::Maze::MonsterData::getName(uint32 id) const
{
    XEEN_VALID();

    const Game::Type gt = _parent->getGame()->getGameType();

    if(enforce(id < MAX_MONSTERS) && gt == Game::CLOUDS)
    {
        return (const char*)&_xeenMON[id * 60];
    }

    // TODO: Other games
    assert(false);
}
