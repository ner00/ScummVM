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

XEEN::Party::Party()
{
    Common::ScopedPtr<CCFileData> reader(XEENgame.getAssets().getSaveFile().getFile("MAZE.PTY"));
    Common::ScopedPtr<CCFileData> charReader(XEENgame.getAssets().getSaveFile().getFile("MAZE.CHR"));

    memset(_characters, 0, sizeof(_characters));
    
    if(reader && charReader)
    {
        // Read the data pertaining to the whole party
        memberCount = reader->readByte();
        realMemberCount = reader->readByte();
        reader->read(members, 8);
        
        facing = reader->readByte();
        position.x = reader->readByte();
        position.y = reader->readByte();
        mazeID = reader->readByte();
    
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
    
    return (enforce(slot < 8)) ? getCharacter(members[slot]) : 0;
}
