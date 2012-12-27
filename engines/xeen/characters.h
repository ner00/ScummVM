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
#include "xeen/archive/file.h"

namespace XEEN
{
    class Party;

    struct Statistic
    {        
        Statistic(int8* realv, int8* tempv) : real(realv), temp(tempv) {}
        
        int16 getValue() const { return (*real) + (*temp); }
        int16 getReal() const { return *real; }
        int16 getTemp() const { return *temp; }
        void modifyReal(int8 value) { *real += value; }
        void modifyTemp(int8 value) { *temp += value; }
        void resetTemp() { *temp = 0; }
        
        private:
            int8* real;
            int8* temp;
    };

    enum Side  { CLOUDS, DARKSIDE };

    enum Stat { MIGHT, INTELLECT, PERSONALITY, ENDURANCE, SPEED, ACCURACY, LUCK, LEVEL, 
                FIRE, ELEC, COLD, POISON, ENERGY, MAGIC, STAT_COUNT };

    // Stats for a single playable character
    class Character : public Validateable, public Common::NonCopyable
    {
        friend class Party;

        public:
            enum Value { HP, SP, SEX, CLASS, RACE, EXPERIENCE, VALUE_MAX };
            enum Sex   { MALE, FEMALE, MAX_SEX };
            enum Race  { HUMAN, ELF, DWARF, GNOME, HALFORC, MAX_RACE };
            enum Class { KNIGHT, PALADIN, ARCHER, CLERIC, SORCERER, ROBBER, NINJA, BARBARIAN, DRUID, RANGER, MAX_CLASS };
            enum Skill { THIEVERY, ARMSMASTER, ASTROLOGER, BODYBUILDER, CARTOGRAPER, CRUSADER, DIRSENSE, LINGUIST,
                         MERCHANT, MOUNTAINEER, NAVIGATOR, PATHFINDER, PRAYERMASTER, PRESTIDIGITATOR, SWIMMER,
                         TRACKER, SPOTSECRETDOORS, DANGERSENSE, MAX_SKILL };

            static const char* const sexNames[];
            static const char* const raceNames[];
            static const char* const classNames[];

    
        private:
            Character(FilePtr data, uint8 index, CCFileId faceSprite);
        
        public:
            uint32 getValue(Value val) const;
            void setValue(Value val, uint32 data);
        
            const char* getName() const;

            uint8 hasSkill(uint32 skill) const;
            void setSkill(uint32 skill, bool state);

            Statistic getStat(Stat stat) const;
        
            bool hasSpell(uint32 id) const;
            static const char* getSpellName(uint32 id);

        public:
            template <typename T>
            void modifyValue(Value val, uint32 data) { T modify; setValue(val, modify(getValue(val), data)); }


        public:
            const char* getSexName() const { const uint32 sex = getValue(SEX); return (enforce(sex < MAX_SEX)) ? sexNames[sex] : "BAD SEX"; }
            const char* getRaceName() const { const uint32 race = getValue(RACE); return (enforce(race < MAX_RACE)) ? raceNames[race] : "BAD RACE"; }
            const char* getClassName() const { const uint32 clazz = getValue(CLASS); return (enforce(clazz < MAX_CLASS)) ? classNames[clazz] : "BAD CLASS"; }
            uint32 getSkillCount() const;


        public:
            FilePtr _data;
            uint8 _index;
        
            CCFileId face;
    };
}

#endif // XEEN_CHARACTERS_H
