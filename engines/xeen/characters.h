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

#ifndef XEEN_CHARACTERS_H
#define XEEN_CHARACTERS_H

#include "common/scummsys.h"
#include "xeen/utility.h"

namespace XEEN
{
    class CCFile;
    class CCFileData;
    class CharacterManager;
    class Sprite;

    enum Sex {MALE, FEMALE};
    enum Race {HUMAN, ELF, DWARF, GNOME, HALFORC};
    enum Side {CLOUDS, DARKSIDE};
    enum Class {KNIGHT, PALADIN, ARCHER, CLERIC, SORCERER, ROBBER, NINJA, BARBARIAN, DRUID, RANGER};

    // Stats for a single playable character
    class Character
    {
        friend class CharacterManager;
    
        private:
            Character(CCFileData* data, Sprite* faceSprite);
    
        public:
            Sprite* face;
        
            char name[16];
            Sex sex;
            Race race;
            Side saveSide;
            Class profession;
            
            int8 stats[14];
            
            int8 actemp;
            
            int8 level[2];
            
            int8 dbday;
            int8 agetemp;
            
            int8 skills[18];
            int8 awards[64];
            int8 spells[39];

            // Beacon            
            uint8 beaconMap;
            uint8 beaconX;
            uint8 beaconY;
            Side beaconSide;

            
            uint8 hasSpells;
            uint8 currentSpell;
            uint8 quickOption;

            // Items
            uint8 weapons[36];
            uint8 armor[36];
            uint8 accessories[36];
            uint8 misc[36];
            
            
            int8 resistences[12];            
            int8 conditions[16];
            
            int8 unknown[3];

            int16 hp;
            int16 sp;
            int16 bday2;
            
            uint32 experience;
            
            uint8 adventureSpell;
            uint8 combatSpell;
    };
    
    class CharacterManager
    {
        private:
            static const uint32 MAX_CHARACTERS = 30;
    
        public:
            CharacterManager(CCFile& parent);
            ~CharacterManager();
            
            Character* getCharacter(uint16 id);
            
        private:
            CCFile& _cc;
            
            Character* _characters[MAX_CHARACTERS];
    };
}

#endif // XEEN_CHARACTERS_H
