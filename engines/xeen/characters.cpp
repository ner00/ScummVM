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
#include "xeen/characters.h"
#include "xeen/party.h"
#include "xeen/utility.h"

static const int OFF_SEX        = 0x010;
static const int OFF_RACE       = 0x011;
static const int OFF_SIDE       = 0x012;
static const int OFF_CLASS      = 0x013;
static const int OFF_STATS      = 0x014;
static const int OFF_LEVEL      = 0x023;
static const int OFF_BIRTHDAY   = 0x025;
static const int OFF_TEMPAGE    = 0x026;
static const int OFF_SKILLS     = 0x027;
static const int OFF_AWARDS     = 0x039;
static const int OFF_SPELLS     = 0x079;
static const int OFF_RESIST     = 0x137;
static const int OFF_HP         = 0x156;
static const int OFF_SP         = 0x158;
static const int OFF_BIRTHYEAR  = 0x15A;
static const int OFF_EXPERIENCE = 0x15C;

static const XEEN::ValueManager::ValueInfo charValues[] =
{
    { OFF_HP, 2, true },
    { OFF_SP, 2, true },
    { OFF_SEX, 1, false },
    { OFF_CLASS, 1, false },
    { OFF_RACE, 1, false },
    { OFF_EXPERIENCE, 4, false },
    { OFF_TEMPAGE, 1, false },
    { OFF_BIRTHDAY, 1, false },
    { OFF_BIRTHYEAR, 2, false }
};
static const uint32 valueCount = sizeof(charValues) / sizeof(charValues[0]);

const char* const XEEN::Character::sexNames[] =   { "Male", "Female" };
const char* const XEEN::Character::raceNames[] =  { "Human", "Elf", "Dwarf", "Gnome", "Half-orc" };
const char* const XEEN::Character::classNames[] = { "Knight", "Paladin", "Archer", "Cleric", "Sorcerer", "Robber", "Ninja", "Barbarian", "Druid", "Ranger" };

static const char* const spellNames[] = 
{
    "Acid Spray", "Awaken", "Beast Master", "Bless", "Clairvoyance", "Cold Ray", "Create Food", "Cure Disease", "Cure Paralysis", "Cure Poison", "Cure Wounds", "Dancing Sword",
    "Day of Protection", "Day of Sorcery", "Deadly Swarm", "Detect Monster", "Divine Intervention", "Dragon Sleep", "Elemental Storm", "Enchant Item", "Energy Blast",
    "Etherealize", "Fantastic Freeze", "Fiery Flail", "Finger of Death", "Fire Ball", "First Aid", "Flying Fist", "Frost Bite", "Golem Stopper", "Heroism", "Holy Bonus",
    "Holy Word", "Hypnotize", "Identify Monster", "Implosion", "Incinerate", "Inferno", "Insect Spray", "Item to Gold", "Jump", "Levitate", "Light", "Lightning Bolt",
    "Lloyd\'s Beacon", "Magic Arrow", "Mass Distortion", "Mega Volts", "Moon Ray", "Nature\'s Cure", "Pain", "Poison Volley", "Power Cure", "Power Shield", "Prismatic Light",
    "Prot. from Elements", "Raise Dead", "Recharge Item", "Resurrect", "Revitalize", "Shrapmetal", "Sleep", "Sparks", "Star Burst", "Stone to Flesh", "Sun Ray", "Super Shelter",
    "Suppress Disease", "Suppress Poison", "Teleport", "Time Distortion", "Town Portal", "Toxic Cloud", "Turn Undead", "Walk on Water", "Wizard Eye", "None Ready"
};
static const uint32 TOTAL_SPELLS = sizeof(spellNames) / sizeof(spellNames[0]);
static const uint32 MAX_SPELLS = 39;
static const int divineSpells[MAX_SPELLS] = {0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 12, 14, 16, 23, 26, 27, 28, 30, 31, 32, 33, 42, 46, 48, 49, 50, 52, 55, 56, 58, 59, 62, 64, 65, 67, 68, 71, 73, 74};

///
/// Character
///
XEEN::Character::Character(Valid<Game> parent, FilePtr data, uint8 index, CCFileId faceSprite) : 
    GameHolder(parent), ValueManager(data, charValues, valueCount, index * 354), _data(data), _index(index), face(faceSprite)
{
    if(enforce(index < Party::MAX_CHARACTERS))
    {
        
    }
    else
    {
        markInvalid("Character index %d out of range.", index);
    }
}

void XEEN::Character::giveDamage(uint16 amount, uint8 type)
{
    // TODO: Actual calculation!
    setValue<int16>(HP, getValue<int16>(HP) - amount);
}

const char* XEEN::Character::getName() const
{
    XEEN_VALID();
    
    return (const char*)_data->getPtrAt<const char>((_index * 354));
}

uint32 XEEN::Character::getAge() const
{
    XEEN_VALID();

    // TODO: This seems to update once a year and not account for birthday.

    const uint32 birthYear = getValue<uint16>(BIRTHYEAR);
    const uint32 currentYear = getGame()->getParty()->getValue<uint16>(Party::YEAR);

    return (currentYear - birthYear);
}


uint8 XEEN::Character::hasSkill(uint32 skill) const
{
    XEEN_VALID();
    return (enforce(skill < MAX_SKILL)) ? _data->get<uint8>(_index * 354 + OFF_SKILLS + skill) : 0;
}

void XEEN::Character::setSkill(uint32 skill, bool state)
{
    XEEN_VALID();
    
    if(enforce(skill < MAX_SKILL))
    {
        _data->set<uint8>(_index * 354 + OFF_SKILLS + skill, state ? 1 : 0);
    }
}

uint8 XEEN::Character::hasAward(uint32 award) const
{
    // TODO: Darkside awards are stored in high nibble
    return (enforce(award < MAX_AWARD)) ? _data->get<uint8>(_index * 354 + OFF_AWARDS + award) & 0xF : 0;
}

void XEEN::Character::setAward(uint32 award, bool state)
{
    XEEN_VALID();

    // TODO: Darkside awards are stored in high nibble
    if(enforce(award < MAX_AWARD))
    {
        uint8 awardByte = _data->get<uint8>(_index * 354 + OFF_AWARDS + award);
        awardByte &= 0xF0;
        awardByte |= state ? 1 : 0;
        _data->set<uint8>(_index * 354 + OFF_AWARDS + award, awardByte);
    }
}


XEEN::Statistic XEEN::Character::getStat(Stat stat) const
{
    XEEN_VALID();

    if(enforce(stat < STAT_COUNT))
    {
        if(stat < LEVEL)
        {
            int8* real = (int8*)_data->getPtrAt<int8>((_index * 354) + OFF_STATS + (2 * stat) + 0);
            int8* temp = (int8*)_data->getPtrAt<int8>((_index * 354) + OFF_STATS + (2 * stat) + 1);
            return Statistic(real, temp);
        }
        else if(stat == LEVEL)
        {
            int8* real = (int8*)_data->getPtrAt<int8>((_index * 354) + OFF_LEVEL + 0);
            int8* temp = (int8*)_data->getPtrAt<int8>((_index * 354) + OFF_LEVEL + 1);
            return Statistic(real, temp);            
        }
        else if(stat > LEVEL)
        {
            int8* real = (int8*)_data->getPtrAt<int8>((_index * 354) + OFF_RESIST + (2 * (stat - FIRE)) + 0);
            int8* temp = (int8*)_data->getPtrAt<int8>((_index * 354) + OFF_RESIST + (2 * (stat - FIRE)) + 1);
            return Statistic(real, temp);
        }
    }

    return Statistic(0, 0);
}

bool XEEN::Character::hasSpell(uint32 id) const
{
    XEEN_VALID();

    return (id < MAX_SPELLS) ? _data->get<uint8>(OFF_SPELLS + id) : false;
}

const char* XEEN::Character::getSpellName(uint32 id)
{
    return (id < MAX_SPELLS) ? spellNames[divineSpells[id]] : "None Ready";
}

uint32 XEEN::Character::getSkillCount() const
{
    const byte* skills = _data->getPtrAt<byte>(_index * 354 + OFF_SKILLS);
    uint32 result = 0;

    for(uint32 i = 0; i != MAX_SKILL; i ++)
    {
        result += skills[i] ? 1 : 0;
    }

    return result;
}

uint32 XEEN::Character::getAwardCount() const
{
    const byte* awards = _data->getPtrAt<byte>(_index * 354 + OFF_AWARDS);
    uint32 result = 0;

    for(uint32 i = 0; i != MAX_AWARD; i ++)
    {
        result += (awards[i] & 0xF) ? 1 : 0;
        result += (awards[i] & 0xF0) ? 1 : 0;
    }

    return result;
}
