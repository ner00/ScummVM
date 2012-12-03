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
    class Map;

    class Party : public Validateable
    {
        friend class Game;

        public:
            static const uint32 MAX_CHARACTERS = 30;
            static const uint32 MAX_SLOTS = 6;
            static const uint32 INVALID_CHARACTER = 255;
            static const uint32 INVALID_SLOT = 255;

        private:
            Party();
            ~Party();

        public:
            uint8 getMemberCount() const { return _memberCount; }
            Character* getCharacter(uint16 id);
            Character* getCharacterInSlot(unsigned slot);
            uint16 getCharacterIdInSlot(unsigned slot) const;

            void addMember(uint16 id);
            void removeMember(unsigned slot);
            void exchangeMember(unsigned slot1, unsigned slot2);
            
            Map* getMap() const;
            Common::Point getPosition() const { return _position; }
            uint8 getFacing() const { return _facing; }
            
            void moveTo(uint8 maze, const Common::Point& position, uint8 facing);
            void moveTo(const Common::Point& position, uint8 facing = 255);
            void moveRelative(const Common::Point& delta);
            void turn(bool left);
    
        private:
            Character* _characters[MAX_CHARACTERS];        
        
            uint8 _memberCount;
            uint8 _realMemberCount;
            uint8 _members[8];        
        
            uint8 _facing;
            Common::Point _position;
            uint8 _mazeID;
    };
}

#endif // XEEN_PARTY_H
