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

#include "xeen/characters.h"
#include "xeen/game.h"
#include "xeen/party.h"
#include "xeen/utility.h"

#include "xeen/graphics/spritemanager.h"
#include "xeen/graphics/sprite.h"

static const int OFF_SEX        = 0x010;
static const int OFF_RACE       = 0x011;
static const int OFF_CLASS      = 0x012;
static const int OFF_STATS      = 0x014;
static const int OFF_LEVEL      = 0x023;
static const int OFF_SPELLS     = 0x079;
static const int OFF_HP         = 0x156;
static const int OFF_SP         = 0x158;
static const int OFF_EXPERIENCE = 0x15C;

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
XEEN::Character::Character(FilePtr data, uint8 index, Sprite* faceSprite) : _data(data), _index(index), face(0)
{
    if(enforce(index < Party::MAX_CHARACTERS))
    {
        
    }
    else
    {
        markInvalid();
    }
}

uint32 XEEN::Character::getValue(Value val) const
{
    XEEN_VALID_RET(0);

    static const struct
    {
        int offset;
        int size;
    }   values[] =
    {
        { OFF_HP, 2 },
        { OFF_SP, 2 },
        { OFF_EXPERIENCE, 4 }
    };
        
    if(enforce(val < VALUE_MAX))
    {
        if(values[val].size == 4)
        {
            return _data->getU32At((_index * 354) + values[val].offset);
        }
        else if(values[val].size == 2)
        {
            return _data->getU16At((_index * 354) + values[val].offset);
        }
        else
        {
            return _data->getByteAt((_index * 354) + values[val].offset);
        }
    }
    
    return 0;
}


const char* XEEN::Character::getName() const
{
    XEEN_VALID_RET("");
    
    return (const char*)_data->getBytePtrAt((_index * 354));
}

XEEN::Statistic XEEN::Character::getStat(Stat stat) const
{
    static int8 errorStatP;
    static const Statistic errorStat(&errorStatP, &errorStatP);

    XEEN_VALID_RET(errorStat);

    if(enforce(stat < STAT_COUNT))
    {
        if(stat < LEVEL)
        {
            int8* real = (int8*)_data->getBytePtrAt((_index * 354) + OFF_STATS + (2 * stat) + 0);
            int8* temp = (int8*)_data->getBytePtrAt((_index * 354) + OFF_STATS + (2 * stat) + 1);
            return Statistic(real, temp);
        }
        else if(stat == LEVEL)
        {
            int8* real = (int8*)_data->getBytePtrAt((_index * 354) + OFF_LEVEL + 0);
            int8* temp = (int8*)_data->getBytePtrAt((_index * 354) + OFF_LEVEL + 1);
            return Statistic(real, temp);            
        }
        
        return errorStat;
    }
    else
    {
        return errorStat;
    }
}

XEEN::Sex XEEN::Character::getSex() const
{
    XEEN_VALID_RET(MALE);    
    return (Sex)_data->getByteAt((_index * 354) + OFF_SEX);
}

XEEN::Class XEEN::Character::getClass() const
{
    XEEN_VALID_RET(KNIGHT);    
    return (Class)_data->getByteAt((_index * 354) + OFF_CLASS);
}

XEEN::Race XEEN::Character::getRace() const
{
    XEEN_VALID_RET(HUMAN);
    return (Race)_data->getByteAt((_index * 354) + OFF_RACE);
}

bool XEEN::Character::hasSpell(uint32 id) const
{
    XEEN_VALID_RET(false);

    return (id < MAX_SPELLS) ? _data->getByteAt(OFF_SPELLS + id) : false;
}

const char* XEEN::Character::getSpellName(uint32 id)
{
    return (id < MAX_SPELLS) ? spellNames[divineSpells[id]] : "None Ready";
}


