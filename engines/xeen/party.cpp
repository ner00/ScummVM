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

#include "xeen/archive/file.h"
#include "xeen/graphics/sprite.h"
#include "xeen/graphics/spritemanager.h"

#include "xeen/maze/map.h"

static const int OFF_PARTY_COUNT        = 0x000;
static const int OFF_REAL_PARTY_COUNT   = 0x001;
static const int OFF_MEMBERS            = 0x002;
static const int OFF_MAZE_FACING        = 0x00A;
static const int OFF_MAZE_X             = 0x00B;
static const int OFF_MAZE_Y             = 0x00C;
static const int OFF_MAZE_ID            = 0x00D;
static const int OFF_DAY                = 0x264;
static const int OFF_YEAR               = 0x266;
static const int OFF_MINUTES            = 0x268;
static const int OFF_FOOD               = 0x26A;
static const int OFF_GOLD               = 0x27E;
static const int OFF_GEMS               = 0x282;
static const int OFF_GOLD_BANK          = 0x286;
static const int OFF_GEMS_BANK          = 0x28A;

static inline byte getByteAt(XEEN::FilePtr file, uint32 pos)
{
    file->seek(pos);
    return file->readByte();
}

static inline void setByteAt(XEEN::FilePtr file, uint32 pos, uint8 val)
{
    file->getData()[pos] = val;
}

static inline uint16 getWordAt(XEEN::FilePtr file, uint32 pos)
{
    file->seek(pos);
    return file->readUint16LE();
}

static inline uint32 getDwordAt(XEEN::FilePtr file, uint32 pos)
{
    file->seek(pos);
    return file->readUint32LE();
}

#define GET8(T)    getByteAt(_mazePTY, T)
#define SET8(T, V) setByteAt(_mazePTY, T, V)

#define GET16(T)   getWordAt(_mazePTY, T)
#define GET32(T)   getDwordAt(_mazePTY, T)



XEEN::Party::Party()
{
    _mazePTY = XEENgame.getFile("MAZE.PTY", true);
    _mazeCHR = XEENgame.getFile("MAZE.CHR", true);

    memset(_characters, 0, sizeof(_characters));
    
    if(_mazePTY && _mazeCHR)
    {
        // Read each character
        for(uint32 i = 0; i != MAX_CHARACTERS; i ++)
        {
            // Only accept a character that has a matching face image
            Sprite* faceSprite = XEENgame.getSpriteManager()->getSprite(CCFileId("CHAR%02d.FAC", i + 1));
            _characters[i] = valid(faceSprite) ? new Character(_mazeCHR, i, faceSprite) : 0;
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

uint32 XEEN::Party::getValue(PartyValue val) const
{
    XEEN_VALID_RET(0);

    static const struct
    {
        int offset;
        int size;
    }   values[] =
    {
        { OFF_PARTY_COUNT, 1 },
        { OFF_GOLD, 4 },
        { OFF_GOLD_BANK, 4 },
        { OFF_GEMS, 4 },
        { OFF_GEMS_BANK, 4 },
        { OFF_FOOD, 2 },
        { OFF_MAZE_ID, 1 },
        { OFF_MAZE_X, 1 },
        { OFF_MAZE_Y, 1 },
        { OFF_MAZE_FACING, 1 },
        { OFF_DAY, 2 },
        { OFF_YEAR, 2 },
        { OFF_MINUTES, 2 }
    };
        
    if(enforce(val < PARTY_VALUE_MAX))
    {
        if(values[val].size == 4)
        {
            return GET32(values[val].offset);
        }
        else if(values[val].size == 2)
        {
            return GET16(values[val].offset);
        }
        else
        {
            return GET8(values[val].offset);
        }
    }
    
    return 0;
}

Common::Point XEEN::Party::getPosition() const
{
    XEEN_VALID_RET(Common::Point(0, 0));

    return Common::Point(GET8(OFF_MAZE_X), GET8(OFF_MAZE_Y));
}

uint8 XEEN::Party::getMemberIdFromSlot(unsigned slot) const
{
    XEEN_VALID_RET(0);
    return (enforce(slot < MAX_SLOTS)) ? GET8(OFF_MEMBERS + slot) : 0;
}


// Manage Members

XEEN::Character* XEEN::Party::getMember(uint16 id)
{
    XEEN_VALID_RET(0);
    return (enforce(id < MAX_CHARACTERS)) ? _characters[id] : 0;
}

XEEN::Character* XEEN::Party::getMemberInSlot(unsigned slot)
{
    XEEN_VALID_RET(0);
    return getMember(getMemberIdFromSlot(slot));
}

void XEEN::Party::addMember(uint16 id)
{
    XEEN_VALID();

    const uint8 memberCount = getValue(PARTY_COUNT);
    
    if(enforce(id < MAX_CHARACTERS) && memberCount < MAX_SLOTS)
    {
        SET8(OFF_MEMBERS + memberCount, id);
        SET8(OFF_PARTY_COUNT, memberCount + 1);
    }
}

void XEEN::Party::removeMember(unsigned slot)
{
    XEEN_VALID();
    
/*    const uint8 memberCount = getByteAt(_mazePTY, 0);
    
    if(enforce(slot < MAX_SLOTS) && slot < memberCount)
    {
        _members[slot] = 0;
        
        for(int i = slot; i != MAX_SLOTS - 1; i ++)
        {
            _members[i + 1] = _members[i];
        }
        
        _members[MAX_SLOTS - 1] = 0;
        _memberCount -= 1;
    }*/
}

void XEEN::Party::exchangeMember(unsigned slot1, unsigned slot2)
{
    XEEN_VALID();
    
/*    if(enforce(slot1 < MAX_SLOTS) && enforce(slot2 < MAX_SLOTS))
    {
        if(slot1 < _memberCount && slot2 < _memberCount)
        {
            uint16 t = _members[slot1];
            _members[slot1] = _members[slot2];
            _members[slot2] = t;
        }
    }*/
}

XEEN::Map* XEEN::Party::getMap() const
{
    XEEN_VALID_RET(0);
    
    if(valid(XEENgame))
    {
        return XEENgame.getMapManager()->getMap(getValue(MAZE_ID));
    }
    
    return 0;
}


void XEEN::Party::moveTo(uint8 maze, const Common::Point& position, uint8 facing)
{
    XEEN_VALID();

    SET8(OFF_MAZE_ID, maze);
    moveTo(position, facing);
}

#include "xeen/maze/eventlist.h"
void XEEN::Party::moveTo(const Common::Point& position, uint8 facing)
{
    XEEN_VALID();
    
    SET8(OFF_MAZE_X, position.x);
    SET8(OFF_MAZE_Y, position.y);
    SET8(OFF_MAZE_FACING, facing < 3 ? facing : GET8(OFF_MAZE_FACING));

    Map* const map = getMap();
    if(valid(map))
    {
        map->runEventAt(position.x, position.y, facing);
    }
}

void XEEN::Party::moveRelative(const Common::Point& delta)
{
    XEEN_VALID();

    moveTo(Map::translatePoint(getPosition(), delta.x, delta.y, getValue(MAZE_FACING)));
}

void XEEN::Party::turn(bool left)
{
    XEEN_VALID();

    SET8(OFF_MAZE_FACING, (getValue(MAZE_FACING) + (left ? -1 : 1)) & 3);
}
