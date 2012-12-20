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
#include "xeen/ui/basicwindows.h"
#include "xeen/event/basicevents.h"
#include "xeen/party.h"
#include "xeen/characters.h"

XEEN::CharacterStatusWindow::CharacterStatusWindow(Valid<Game> parent) : Window(parent, Common::Rect(0, 0, 320, 146))
{
}

            
const XEEN::Button* XEEN::CharacterStatusWindow::getButtons() const
{
    static const Button buttons[] = 
    {
        {"VIEW.ICN",  0,  1, { 10,  24, 24, 20},  0, Common::KEYCODE_INVALID}, {"VIEW.ICN", 10, 11, { 61,  24, 24, 20},  1, Common::KEYCODE_INVALID},
        {"VIEW.ICN", 20, 21, {112,  24, 24, 20},  2, Common::KEYCODE_INVALID}, {"VIEW.ICN", 30, 31, {177,  24, 24, 20},  3, Common::KEYCODE_INVALID},
        {"VIEW.ICN",  2,  3, { 10,  47, 24, 20},  4, Common::KEYCODE_INVALID}, {"VIEW.ICN", 12, 13, { 61,  47, 24, 20},  5, Common::KEYCODE_INVALID},
        {"VIEW.ICN", 22, 23, {112,  47, 24, 20},  6, Common::KEYCODE_INVALID}, {"VIEW.ICN", 32, 33, {177,  47, 24, 20},  7, Common::KEYCODE_INVALID},
        {"VIEW.ICN",  4,  5, { 10,  70, 24, 20},  8, Common::KEYCODE_INVALID}, {"VIEW.ICN", 14, 15, { 61,  70, 24, 20},  9, Common::KEYCODE_INVALID},
        {"VIEW.ICN", 24, 25, {112,  70, 24, 20}, 10, Common::KEYCODE_INVALID}, {"VIEW.ICN", 34, 35, {177,  70, 24, 20}, 11, Common::KEYCODE_INVALID},
        {"VIEW.ICN",  6,  7, { 10,  93, 24, 20}, 12, Common::KEYCODE_INVALID}, {"VIEW.ICN", 16, 17, { 61,  93, 24, 20}, 13, Common::KEYCODE_INVALID},
        {"VIEW.ICN", 26, 27, {112,  93, 24, 20}, 14, Common::KEYCODE_INVALID}, {"VIEW.ICN", 36, 37, {177,  93, 24, 20}, 15, Common::KEYCODE_INVALID},
        {"VIEW.ICN",  8,  9, { 10, 116, 24, 20}, 16, Common::KEYCODE_INVALID}, {"VIEW.ICN", 18, 19, { 61, 116, 24, 20}, 17, Common::KEYCODE_INVALID},
        {"VIEW.ICN", 28, 29, {112, 116, 24, 20}, 18, Common::KEYCODE_INVALID}, {"VIEW.ICN", 38, 39, {177, 116, 24, 20}, 19, Common::KEYCODE_INVALID},

        {"VIEW.ICN", 40, 41, {285,  11, 24, 20}, 20, Common::KEYCODE_i},       {"VIEW.ICN", 42, 43, {285,  43, 24, 20}, 21, Common::KEYCODE_q},
        {"VIEW.ICN", 44, 45, {285,  75, 24, 20}, 22, Common::KEYCODE_e},       {"VIEW.ICN", 46, 47, {285, 107, 24, 20}, 23, Common::KEYCODE_ESCAPE},

        {(uint16)0, 0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
    };
    
    return buttons;
}

const XEEN::String* XEEN::CharacterStatusWindow::getStrings() const
{
    static const String strings[] = 
    {
        {"Mgt",    0,  37,  26, 0}, {0,  1,  37,  34, 0}, {"Acy",        0,  88,  26, 0}, {0,  2,  88,  34, 0}, 
        {"H.P.",   0, 139,  26, 0}, {0,  3, 139,  34, 0}, {"Experience", 0, 204,  26, 0}, {0,  4, 204,  34, 0}, 
        {"Int",    0,  37,  49, 0}, {0,  5,  37,  57, 0}, {"Lck",        0,  88,  49, 0}, {0,  6,  88,  57, 0}, 
        {"S.P.",   0, 139,  49, 0}, {0,  7, 139,  57, 0}, {"Party Gold", 0, 204,  49, 0}, {0,  8, 204,  57, 0}, 
        {"Per",    0,  37,  70, 0}, {0,  9,  37,  78, 0}, {"Age",        0,  88,  70, 0}, {0, 10,  88,  78, 0}, 
        {"Resis",  0, 139,  70, 0}, {0, 11, 139,  78, 0}, {"Party Gems", 0, 204,  70, 0}, {0, 12, 204,  78, 0}, 
        {"End",    0,  37,  93, 0}, {0, 13,  37, 101, 0}, {"Lvl",        0,  88,  93, 0}, {0, 14,  88, 101, 0}, 
        {"Skills", 0, 139,  93, 0}, {0, 15, 139, 101, 0}, {"Party Food", 0, 204,  93, 0}, {0, 16, 204, 101, 0}, 
        {"Spd",    0,  37, 116, 0}, {0, 17,  37, 124, 0}, {"AC",         0,  88, 116, 0}, {0, 18,  88, 124, 0}, 
        {"Awrds",  0, 139, 116, 0}, {0, 19, 139, 124, 0}, {"Condition",  0, 204, 116, 0}, {0, 20, 204, 124, 0}, 

        {0, 21, 7, 12, 0},

        {(uint16)0, 0, 0, 0, 0}
    };
    
    return strings;
}

void XEEN::CharacterStatusWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    switch(id)
    {
        //TODO: Make QUICKREF retrun to STATUS window
        case 21: _parent->setEvent(new Event::QuickReference(_parent)); break;
//        case 23: _parent->showWindow(Game::NONE); break;
    }
}

void XEEN::CharacterStatusWindow::produceString(unsigned id)
{
    XEEN_VALID();

    Party* party = _parent->getParty();
    Character* character = _parent->getActiveCharacter();

    if(valid(character))
    {
        switch(id)
        {
            case  1: fillStringBuffer("%d", character->getStat(MIGHT).getValue()); break;
            case  2: fillStringBuffer("%d", character->getStat(ACCURACY).getValue()); break;
            case  5: fillStringBuffer("%d", character->getStat(INTELLECT).getValue()); break;
            case  6: fillStringBuffer("%d", character->getStat(LUCK).getValue()); break;
            case  9: fillStringBuffer("%d", character->getStat(PERSONALITY).getValue()); break;
            case 13: fillStringBuffer("%d", character->getStat(ENDURANCE).getValue()); break;
            case 17: fillStringBuffer("%d", character->getStat(SPEED).getValue()); break;                
            case 14: fillStringBuffer("%d", character->getStat(LEVEL).getValue()); break;

            case  3: fillStringBuffer("%d", character->getValue(Character::HP)); break;
            case  4: fillStringBuffer("%d", character->getValue(Character::EXPERIENCE)); break;
            case  7: fillStringBuffer("%d", character->getValue(Character::SP)); break;
            case 10: fillStringBuffer("DANG"); break;
            case 11: fillStringBuffer("DANG"); break;

            case 15: fillStringBuffer("DANG"); break;

            case  8: fillStringBuffer("%d", party->getValue(Party::GOLD)); break;
            case 12: fillStringBuffer("%d", party->getValue(Party::GEMS)); break;
            case 16: fillStringBuffer("%d", party->getValue(Party::FOOD)); break; // TODO: / 3 / PARTY_COUNT: needs to be shown in days!
            

            case 18: fillStringBuffer("DANG"); break;
            case 19: fillStringBuffer("DANG"); break;
            case 20: fillStringBuffer("DANG"); break;
            
            case 21:
            {
                fillStringBuffer("%s : %s %s %s", character->getName(), getSexName(character->getSex()),
                         getRaceName(character->getRace()), getClassName(character->getClass()));
                break;
            }
        }
    }
}

