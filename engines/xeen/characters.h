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
    class Character : public Validateable, public Common::NonCopyable, public GameHolder, public ValueManager
    {
        friend class Party;

        public:
            static const uint32 HP = 0;
            static const uint32 SP = 1;
            static const uint32 SEX = 2;
            static const uint32 CLASS = 3;
            static const uint32 RACE = 4;
            static const uint32 EXPERIENCE = 5;
            static const uint32 TEMPAGE = 6;
            static const uint32 BIRTHDAY = 7;
            static const uint32 BIRTHYEAR = 8;

            enum Sex   { MALE, FEMALE, MAX_SEX };
            enum Race  { HUMAN, ELF, DWARF, GNOME, HALFORC, MAX_RACE };
            enum Class { KNIGHT, PALADIN, ARCHER, CLERIC, SORCERER, ROBBER, NINJA, BARBARIAN, DRUID, RANGER, MAX_CLASS };
            enum Skill { THIEVERY, ARMSMASTER, ASTROLOGER, BODYBUILDER, CARTOGRAPER, CRUSADER, DIRSENSE, LINGUIST,
                         MERCHANT, MOUNTAINEER, NAVIGATOR, PATHFINDER, PRAYERMASTER, PRESTIDIGITATOR, SWIMMER,
                         TRACKER, SPOTSECRETDOORS, DANGERSENSE, MAX_SKILL };
            static const uint32 MAX_AWARD = 64;

            static const char* const sexNames[];
            static const char* const raceNames[];
            static const char* const classNames[];

    
        private:
            Character(Valid<Game> parent, FilePtr data, uint8 index, CCFileId faceSprite);
        
        public:
            void giveDamage(uint16 amount, uint8 type);
        
            const char* getName() const;
            uint32 getAge() const;

            uint8 hasSkill(uint32 skill) const;
            void setSkill(uint32 skill, bool state);

            uint8 hasAward(uint32 award) const;
            void setAward(uint32 award, bool state);

            Statistic getStat(Stat stat) const;
        
            bool hasSpell(uint32 id) const;
            static const char* getSpellName(uint32 id);

        public:
            const char* getSexName() const { const uint8 sex = getValue<uint8>(SEX); return (enforce(sex < MAX_SEX)) ? sexNames[sex] : "BAD SEX"; }
            const char* getRaceName() const { const uint8 race = getValue<uint8>(RACE); return (enforce(race < MAX_RACE)) ? raceNames[race] : "BAD RACE"; }
            const char* getClassName() const { const uint8 clazz = getValue<uint8>(CLASS); return (enforce(clazz < MAX_CLASS)) ? classNames[clazz] : "BAD CLASS"; }
            uint32 getSkillCount() const;
            uint32 getAwardCount() const;


        public:
            FilePtr _data;
            uint8 _index;
        
            CCFileId face;
    };
}

#endif // XEEN_CHARACTERS_H
