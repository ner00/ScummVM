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
#include "common/system.h"

#include "xeen/game.h"
#include "xeen/ui/quickreference.h"
#include "xeen/party.h"
#include "xeen/characters.h"

XEEN::QuickReferenceWindow::QuickReferenceWindow() : Window(Common::Rect(0, 0, 320, 146))
{
}

const XEEN::String* XEEN::QuickReferenceWindow::getStrings() const
{
    XEEN_VALID_RET(0);

    static const String strings[] = 
    {
        {"Quick Reference Chart", 0, 92, 8},
    
        {"#",    0,  15, 20}, {0,  1,  15, 32}, {0,  2,  15, 42}, {0,  3,  15, 52}, {0,   4,  15, 62}, {0,  5,  15, 72}, {0,  6,  15, 82}, 
        {"Name", 0,  35, 20}, {0,  7,  35, 32}, {0,  8,  35, 42}, {0,  9,  35, 52}, {0,  10,  35, 62}, {0, 11,  35, 72}, {0, 12,  35, 82},         
        {"Cls",  0, 118, 20}, {0, 13, 118, 32}, {0, 14, 118, 42}, {0, 15, 118, 52}, {0,  16, 118, 62}, {0, 17, 118, 72}, {0, 18, 118, 82},         

        {(uint16)0, 0, 0, 0}
    };
    
    return strings;
}

const char* XEEN::QuickReferenceWindow::produceString(unsigned id)
{
    XEEN_VALID_RET("");

    stringBuffer[0] = 0;

    if(valid(XEENgame) && valid(XEENgame.getParty()))
    {
        Party& party = XEENgame.getParty();
    
        // #
        if(id >= 1 && id <= 6)
        {
            snprintf(stringBuffer, sizeof(stringBuffer), "%d)", id);
            return stringBuffer;            
        }
        
        // Name
        if(id >= 7 && id <= 12)
        {
            Character* character = party.getMemberInSlot(id - 7);
            snprintf(stringBuffer, sizeof(stringBuffer), "%s", valid(character) ? character->getName() : "");
            return stringBuffer;
        }
        
        // Class
        if(id >= 13 && id <= 18)
        {
            Character* character = party.getMemberInSlot(id - 13);
            snprintf(stringBuffer, sizeof(stringBuffer), "%.3s", valid(character) ? getClassName(character->getClass()) : "");
            return stringBuffer;
        }
    }
    
    return "";
}
