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

#include "xeen/utility.h"

namespace XEEN
{
    class Game;
    class Character;

    namespace Maze
    {
        class Map;
    }

    class Party : public GameHolder, public Validateable, public Common::NonCopyable, public ValueManager
    {
        friend class Game;

        public:
            static const uint32 MAX_CHARACTERS = 30;
            static const uint32 MAX_SLOTS = 6;
            static const uint32 INVALID_CHARACTER = 255;
            static const uint32 INVALID_SLOT = 255;

            static const uint32 PARTY_COUNT = 0;
            static const uint32 GOLD = 1;
            static const uint32 GOLD_BANK = 2;
            static const uint32 GEMS = 3;
            static const uint32 GEMS_BANK = 4;
            static const uint32 FOOD = 5;
            static const uint32 MAZE_ID = 6;
            static const uint32 MAZE_X = 7;
            static const uint32 MAZE_Y = 8;
            static const uint32 MAZE_FACING = 9;
            static const uint32 DAY = 10;
            static const uint32 YEAR = 11;
            static const uint32 MINUTES = 12;

        private:
            Party(Valid<Game> parent);
            ~Party();

        public:
            Common::Point getPosition() const;
            Direction getFacing() const;
            uint8 getMemberIdFromSlot(unsigned slot) const;            

            bool getGameFlag(LessThan<uint32, 256> id) const;
            void setGameFlag(LessThan<uint32, 256> id, bool set);

            // Manage members
            Character* getMember(uint16 id) const;
            Character* getMemberInSlot(unsigned slot) const;
            
            void addMember(uint16 id);
            void removeMember(unsigned slot);
            void exchangeMember(unsigned slot1, unsigned slot2);
            
            bool hasSkill(uint32 skill) const;

            // Manager Maze
            Valid<Maze::Map> getMap() const;

            void changeMap(uint8 id);
            
            void moveTo(const Common::Point& position, uint8 facing);
            void moveRelative(Direction dir);
            void turn(bool left);
            void bash();
                
        private:
            Character* _characters[MAX_CHARACTERS];
            
            FilePtr _mazePTY;
            FilePtr _mazeCHR;
    };
}

#endif // XEEN_PARTY_H
