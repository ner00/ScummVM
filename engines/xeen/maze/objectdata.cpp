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
#include "xeen/maze/objectdata_.h"

XEEN::Maze::ObjectData::ObjectData(Valid<Manager> parent) : _parent(parent)
{
    memset(_cloudsDAT, 0, sizeof(_cloudsDAT));
    memset(_darkDAT, 0, sizeof(_darkDAT));

    const Game::Type gt = _parent->getGame()->getGameType();

    if(gt == Game::CLOUDS)
    {
        Common::File cloudsdat;
        if(cloudsdat.open("CLOUDS.DAT"))
        {
            cloudsdat.read(&_cloudsDAT, DAT_SIZE);
        }
        else
        {
            markInvalid("CLOUDS.DAT not found.");
        }
    }
    else
    {
        markInvalid("Unrecognized game type.");
        assert(false);
    }
}

const uint8* XEEN::Maze::ObjectData::getDataForObject(uint32 id) const
{
    XEEN_VALID();

    const Game::Type gt = _parent->getGame()->getGameType();

    if(enforce(id < MAX_OBJECTS) && gt == Game::CLOUDS)
    {
        return &_cloudsDAT[id * 12];
    }

    // TODO: Other games
    assert(false);
}

