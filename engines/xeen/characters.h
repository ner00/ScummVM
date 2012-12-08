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

#include "xeen/utility.h"

#include "common/scummsys.h"
#include "common/stream.h"
#include "common/ptr.h"

namespace XEEN
{
    class Game;
    class CCFileData;
    class Sprite;
    class Party;

    enum Sex {MALE, FEMALE};
    inline const char* getSexName(Sex sex)
    {
        return (sex == MALE) ? "Male" : "Female";
    }

    enum Race {HUMAN, ELF, DWARF, GNOME, HALFORC};

    inline const char* getRaceName(Race race)
    {
        switch(race)
        {
            case HUMAN: return "Human";
            case ELF: return "Elf";
            case DWARF: return "Dwarf";
            case GNOME: return "Gnome";
            case HALFORC: return "Half-orc";
            default: return "BAD RACE VALUE";
        }
    }


    enum Side {CLOUDS, DARKSIDE};
    enum Class {KNIGHT, PALADIN, ARCHER, CLERIC, SORCERER, ROBBER, NINJA, BARBARIAN, DRUID, RANGER};
    
    inline const char* getClassName(Class clazz)
    {
        switch(clazz)
        {
            case KNIGHT: return "Knight";
            case PALADIN: return "Paladin";
            case ARCHER: return "Archer";
            case CLERIC: return "Cleric";
            case SORCERER: return "Sorcerer";
            case ROBBER: return "Robber";
            case NINJA: return "Ninja";
            case BARBARIAN: return "Barbarian";
            case DRUID: return "Druid";
            case RANGER: return "Ranger";
            default: return "BAD CLASS VALUE";
        }
    }    

    struct Statistic
    {        
        Statistic(int8* realv, int8* tempv) : real(realv), temp(tempv) {}
        
        int16 getValue() const { return (*real) + (*temp); }
        void modifyReal(int8 value) { *real += value; }
        void modifyTemp(int8 value) { *temp += value; }
        void resetTemp() { *temp = 0; }
        
        private:
            int8* real;
            int8* temp;
    };

    enum Stat { MIGHT, INTELLECT, PERSONALITY, ENDURANCE, SPEED, ACCURACY, LUCK, LEVEL, STAT_COUNT };

    // Stats for a single playable character
    class Character : public Validateable
    {
        friend class Party;

        public:
            enum Value {
                HP, SP, EXPERIENCE, VALUE_MAX};
        
    
        private:
            Character(CCFileData* data, uint8 index, Sprite* faceSprite);
        
        public:
            uint32 getValue(Value val) const;
        
            const char* getName() const;
            Statistic getStat(Stat stat) const;
            
            Sex getSex() const;
            Class getClass() const;
            Race getRace() const;
        
            bool hasSpell(uint32 id) const;
            static const char* getSpellName(uint32 id);

        public:
            CCFileData* _data;
            uint8 _index;
        
            Sprite* face;
    };
}

#endif // XEEN_CHARACTERS_H
