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
#include "xeen/party.h"
#include "xeen/characters.h"
#include "xeen/utility.h"
#include "xeen/ccfile.h"

#include "xeen/graphics/sprite.h"
#include "xeen/graphics/spritemanager.h"

#include "xeen/maze/map.h"

XEEN::Party::Party()
{
    Common::ScopedPtr<CCFileData> reader(XEENgame.getAssets().getSaveFile().getFile("MAZE.PTY"));
    Common::ScopedPtr<CCFileData> charReader(XEENgame.getAssets().getSaveFile().getFile("MAZE.CHR"));

    memset(_characters, 0, sizeof(_characters));
    
    if(reader && charReader)
    {
        // Read the data pertaining to the whole party
        _memberCount = reader->readByte();
        _realMemberCount = reader->readByte();
        reader->read(_members, 8);
        
        _facing = reader->readByte();
        _position.x = reader->readByte();
        _position.y = reader->readByte();
        _mazeID = reader->readByte();
    
        // Read each character
        for(uint32 i = 0; i != MAX_CHARACTERS; i ++)
        {
            charReader->seek(i * 354);
        
            // Only accept a character that has a matching face image
            Sprite* faceSprite = XEENgame.getSpriteManager().getSprite(CCFileId("CHAR%02d.FAC", i + 1));
            _characters[i] = valid(faceSprite) ? new Character(charReader, faceSprite) : 0;
        }
    }
    else
    {
        markInvalid();
    }
}

XEEN::Party::~Party()
{
    for(uint32 i = 0; i != MAX_CHARACTERS; i ++)
    {
        delete _characters[i];
    }
}

XEEN::Character* XEEN::Party::getCharacter(uint16 id)
{
    XEEN_VALID_RET(0);

    return (enforce(id < MAX_CHARACTERS)) ? _characters[id] : 0;
}

XEEN::Character* XEEN::Party::getCharacterInSlot(unsigned slot)
{
    XEEN_VALID_RET(0);
    
    return (enforce(slot < MAX_SLOTS)) ? getCharacter(_members[slot]) : 0;
}

uint16 XEEN::Party::getCharacterIdInSlot(unsigned slot) const
{
    XEEN_VALID_RET(0);
    
    return (enforce(slot < MAX_SLOTS)) ? _members[slot] : INVALID_CHARACTER;
}

void XEEN::Party::addMember(uint16 id)
{
    XEEN_VALID();
    
    if(enforce(id < MAX_CHARACTERS) && _memberCount < MAX_SLOTS)
    {
        _members[_memberCount ++] = id;
    }
}

void XEEN::Party::removeMember(unsigned slot)
{
    XEEN_VALID();
    
    if(enforce(slot < MAX_SLOTS) && slot < _memberCount)
    {
        _members[slot] = 0;
        
        for(int i = slot; i != MAX_SLOTS - 1; i ++)
        {
            _members[i + 1] = _members[i];
        }
        
        _members[MAX_SLOTS - 1] = 0;
        _memberCount -= 1;
    }
}

void XEEN::Party::exchangeMember(unsigned slot1, unsigned slot2)
{
    XEEN_VALID();
    
    if(enforce(slot1 < MAX_SLOTS) && enforce(slot2 < MAX_SLOTS))
    {
        if(slot1 < _memberCount && slot2 < _memberCount)
        {
            uint16 t = _members[slot1];
            _members[slot1] = _members[slot2];
            _members[slot2] = t;
        }
    }
}

XEEN::Map* XEEN::Party::getMap() const
{
    XEEN_VALID_RET(0);
    
    if(valid(XEENgame))
    {
        MapManager& maps = XEENgame.getMapManager();
        
        if(valid(maps))
        {
            return maps.getMap(_mazeID);
        }
    }
    
    return 0;
}

void XEEN::Party::moveTo(uint8 maze, const Common::Point& position, uint8 facing)
{
    XEEN_VALID();

    _mazeID = maze;
    moveTo(position, facing);
}

void XEEN::Party::moveTo(const Common::Point& position, uint8 facing)
{
    XEEN_VALID();
    
    _position = position;
    _facing = (facing < 3) ? facing : _facing;
}

void XEEN::Party::moveRelative(const Common::Point& delta)
{
    XEEN_VALID();

    _position = Map::translatePoint(_position, delta.x, delta.y, _facing);
}

void XEEN::Party::turn(bool left)
{
    XEEN_VALID();

    _facing = _facing + (left ? -1 : 1);
    _facing &= 3;
}
