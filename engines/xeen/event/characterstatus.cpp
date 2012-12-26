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

namespace XEEN
{
    class CharacterStatusWindow : public Window
    {
        public:
            uint32 action;

        public:
            CharacterStatusWindow(Valid<Game> parent) : Window(parent, Common::Rect(0, 0, 320, 146)), action(0) { }
            
        protected:
            const Button* getButtons() const
            {
                static const Button buttons[] = 
                {
                    {CCSpriteId("VIEW.ICN",  0), CCSpriteId("VIEW.ICN",  1), { 10,  24, 24, 20},  1, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 10), CCSpriteId("VIEW.ICN", 11), { 61,  24, 24, 20},  2, Common::KEYCODE_INVALID},
                    {CCSpriteId("VIEW.ICN", 20), CCSpriteId("VIEW.ICN", 21), {112,  24, 24, 20},  3, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 30), CCSpriteId("VIEW.ICN", 31), {177,  24, 24, 20},  4, Common::KEYCODE_INVALID},
                    {CCSpriteId("VIEW.ICN",  2), CCSpriteId("VIEW.ICN",  3), { 10,  47, 24, 20},  5, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 12), CCSpriteId("VIEW.ICN", 13), { 61,  47, 24, 20},  6, Common::KEYCODE_INVALID},
                    {CCSpriteId("VIEW.ICN", 22), CCSpriteId("VIEW.ICN", 23), {112,  47, 24, 20},  7, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 32), CCSpriteId("VIEW.ICN", 33), {177,  47, 24, 20},  8, Common::KEYCODE_INVALID},
                    {CCSpriteId("VIEW.ICN",  4), CCSpriteId("VIEW.ICN",  5), { 10,  70, 24, 20},  9, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 14), CCSpriteId("VIEW.ICN", 15), { 61,  70, 24, 20}, 10, Common::KEYCODE_INVALID},
                    {CCSpriteId("VIEW.ICN", 24), CCSpriteId("VIEW.ICN", 25), {112,  70, 24, 20}, 11, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 34), CCSpriteId("VIEW.ICN", 35), {177,  70, 24, 20}, 12, Common::KEYCODE_INVALID},
                    {CCSpriteId("VIEW.ICN",  6), CCSpriteId("VIEW.ICN",  7), { 10,  93, 24, 20}, 13, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 16), CCSpriteId("VIEW.ICN", 17), { 61,  93, 24, 20}, 14, Common::KEYCODE_INVALID},
                    {CCSpriteId("VIEW.ICN", 26), CCSpriteId("VIEW.ICN", 27), {112,  93, 24, 20}, 15, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 36), CCSpriteId("VIEW.ICN", 37), {177,  93, 24, 20}, 16, Common::KEYCODE_INVALID},
                    {CCSpriteId("VIEW.ICN",  8), CCSpriteId("VIEW.ICN",  9), { 10, 116, 24, 20}, 17, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 18), CCSpriteId("VIEW.ICN", 19), { 61, 116, 24, 20}, 18, Common::KEYCODE_INVALID},
                    {CCSpriteId("VIEW.ICN", 28), CCSpriteId("VIEW.ICN", 29), {112, 116, 24, 20}, 19, Common::KEYCODE_INVALID}, {CCSpriteId("VIEW.ICN", 38), CCSpriteId("VIEW.ICN", 39), {177, 116, 24, 20}, 20, Common::KEYCODE_INVALID},
            
                    {CCSpriteId("VIEW.ICN", 40), CCSpriteId("VIEW.ICN", 41), {285,  11, 24, 20}, 21, Common::KEYCODE_i},       {CCSpriteId("VIEW.ICN", 42), CCSpriteId("VIEW.ICN", 43), {285,  43, 24, 20}, 22, Common::KEYCODE_q},
                    {CCSpriteId("VIEW.ICN", 44), CCSpriteId("VIEW.ICN", 45), {285,  75, 24, 20}, 23, Common::KEYCODE_e},       {CCSpriteId("VIEW.ICN", 46), CCSpriteId("VIEW.ICN", 47), {285, 107, 24, 20}, 24, Common::KEYCODE_ESCAPE},
            
                    {0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
                };
                
                return buttons;
            }

            const String* getStrings() const
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
            
            void handleAction(unsigned id)
            {
                XEEN_VALID();
                action = id;
            }

            void produceString(unsigned id)
            {
                XEEN_VALID();
            
                Party* party = getGame()->getParty();
                Character* character = getGame()->getActiveCharacter();
            
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
    };
}

XEEN::Event::CharacterStatus::CharacterStatus(Valid<Game> parent) : Event(parent)
{
    addWindow(new CharacterStatusWindow(parent));
}

void XEEN::Event::CharacterStatus::process()
{
    CharacterStatusWindow* window = (CharacterStatusWindow*)(Window*)getWindows().back();

    switch(window->action)
    {
        case 22:  getGame()->setEvent(new QuickReference(getGame())); break;
        case 24: setFinished(true, true); break;
    }

    window->action = 0;
}
