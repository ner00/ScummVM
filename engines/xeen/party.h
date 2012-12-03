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
    class CCFileData;

    class Party : public Validateable
    {
        friend class Game;

        public:
            static const uint32 MAX_CHARACTERS = 30;
            static const uint32 MAX_SLOTS = 6;
            static const uint32 INVALID_CHARACTER = 255;
            static const uint32 INVALID_SLOT = 255;

            enum PartyValue {
                PARTY_COUNT, GOLD, GOLD_BANK, GEMS, GEMS_BANK, FOOD, MAZE_ID,
                MAZE_X, MAZE_Y, MAZE_FACING, PARTY_VALUE_MAX };

        private:
            Party();
            ~Party();

        public:
            // Read basic values
            uint32 getValue(PartyValue val) const;
            Common::Point getPosition() const;
            uint8 getMemberIdFromSlot(unsigned slot) const;            

            // Manage members
            Character* getMember(uint16 id);
            Character* getMemberInSlot(unsigned slot);            
            
            void addMember(uint16 id);
            void removeMember(unsigned slot);
            void exchangeMember(unsigned slot1, unsigned slot2);
            
            // Manager Maze
            Map* getMap() const;
            
            void moveTo(uint8 maze, const Common::Point& position, uint8 facing);
            void moveTo(const Common::Point& position, uint8 facing = 255);
            void moveRelative(const Common::Point& delta);
            void turn(bool left);
                
        private:
            Character* _characters[MAX_CHARACTERS];
            
            CCFileData* _mazePTY;
    };
}

#endif // XEEN_PARTY_H
