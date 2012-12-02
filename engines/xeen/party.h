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

#ifndef XEEN_PARTY_H
#define XEEN_PARTY_H

#include "common/scummsys.h"

namespace XEEN
{
    class Game;
    class CCFile;
    class Character;

    class Party : public Validateable
    {
        friend class Game;

        static const uint32 MAX_CHARACTERS = 30;

        private:
            Party();
            ~Party();

        public:
            Character* getCharacter(uint16 id);
            Character* getCharacterInSlot(unsigned slot);
    
        public:
            Character* _characters[MAX_CHARACTERS];
        
            uint8 memberCount;
            uint8 realMemberCount;
            uint8 members[8];

            uint8 facing;
            Common::Point position;
            uint8 mazeID;
    };
}

#endif // XEEN_PARTY_H
