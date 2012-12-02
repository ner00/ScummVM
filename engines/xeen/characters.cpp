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
#include "xeen/ccfile.h"
#include "xeen/game.h"
#include "xeen/utility.h"

#include "xeen/graphics/spritemanager.h"
#include "xeen/graphics/sprite.h"

///
/// Character
///
XEEN::Character::Character(Common::ScopedPtr<CCFileData>& data, Sprite* faceSprite) : face(0)
{
    if(data && valid(faceSprite))
    {
        face = faceSprite;
    
        data->read(name, 16);
    
        sex = (Sex)data->readByte();
        race = (Race)data->readByte();
        saveSide = (Side)data->readByte();
        profession = (Class)data->readByte();
        
        might = Statistic(*data);
        intellect = Statistic(*data);
        personality = Statistic(*data);
        endurance = Statistic(*data);
        speed = Statistic(*data);
        accuracy = Statistic(*data);
        luck = Statistic(*data);
    
    
        actemp = data->readByte();
        data->read(level, 2);
        dbday = data->readByte();
        
        agetemp = data->readByte();
        
        data->read(skills, 18);
        data->read(awards, 64);
        data->read(spells, 39);
    
        beaconMap = data->readByte();
        beaconX = data->readByte();
        beaconY = data->readByte();
        
        hasSpells = data->readByte();
        currentSpell = data->readByte();
        quickOption = data->readByte();
        
        data->read(weapons, 36);
        data->read(armor, 36);
        data->read(accessories, 36);
        data->read(misc, 36);
        
        beaconSide = (Side)data->readByte();
        
        data->read(resistences, 12);
        data->read(conditions, 16);
        data->read(unknown, 3);
        
        hp = data->readSint16LE();
        sp = data->readSint16LE();
        bday2 = data->readUint16LE();
        experience = data->readUint32LE();
        
        adventureSpell = data->readByte();
        combatSpell = data->readByte();
    }
    else
    {
        markInvalid();
    }
}
