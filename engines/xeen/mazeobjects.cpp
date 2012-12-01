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
#include "xeen/ccfile.h"
#include "xeen/mazeobjects.h"

///
/// MazeObjects
///
XEEN::MazeObjects::MazeObjects(uint16 mapNumber)
{
    memset(_objectTypes, 0xFF, sizeof(_objectTypes));
    memset(_monsterTypes, 0xFF, sizeof(_monsterTypes));
    memset(_wallObjectTypes, 0xFF, sizeof(_wallObjectTypes));

    CCFileData* reader = XEENgame.getAssets().getSaveFile().getFile(CCFileId("MAZE%s%03d.MOB", (mapNumber < 100) ? "0" : "X", mapNumber));
    
    if(reader)
    {
        reader->read(_objectTypes, 16);
        reader->read(_monsterTypes, 16);
        reader->read(_wallObjectTypes, 16);

        // Read object list
        while(!reader->eos())
        {
            Entry e;
            e.position.x = reader->readSByte();
            e.position.y = reader->readSByte();
            e.id = reader->readByte();
            e.facing = reader->readByte();
            
            if(e.id == 0xFF)
            {
                break;
            }
            
            _objects.push_back(e);
        }

        delete reader;
    }
}

XEEN::MazeObjects::~MazeObjects()
{
    // TODO: Store the values back into the save game
}

bool XEEN::MazeObjects::getObjectAt(const Common::Point& position, Entry& data)
{
    for(Common::List<Entry>::iterator i = _objects.begin(); i != _objects.end(); i ++)
    {
        if(i->position == position)
        {
            data = *i;
            data.id = _objectTypes[data.id];
            return true;
        }
    }
    
    return false;
}
