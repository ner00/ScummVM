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

#include "xeen/event/basicevents.h"
#include "xeen/game.h"
#include "xeen/party.h"
#include "xeen/characters.h"

namespace XEEN
{
    namespace Event
    {
        class CastSpellWindow : public Window
        {
            public:
                uint32 action;
    
            public:
                CastSpellWindow(Valid<Game> parent) : Window(parent, XRect::cr(226, 0, 94, 146)), action(0)
                {
                }
                
            protected:
                const Button* getButtons() const
                {
                    XEEN_VALID();
                
                    static const Button buttons[] = 
                    {
                        {CCSpriteId("CAST.ICN", 0), CCSpriteId("CAST.ICN", 1), { 8, 108, 24, 20}, 1, Common::KEYCODE_c},
                        {CCSpriteId("CAST.ICN", 2), CCSpriteId("CAST.ICN", 3), {35, 108, 24, 20}, 2, Common::KEYCODE_n},
                        {CCSpriteId("CAST.ICN", 4), CCSpriteId("CAST.ICN", 5), {62, 108, 24, 20}, 3, Common::KEYCODE_ESCAPE},
                
                        {0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
                    };
                    
                    return buttons;
                }
    
                void handleAction(unsigned id)
                {
                    XEEN_VALID();
                    action = id;
                }
    
    
                const String* getStrings() const
                {
                    XEEN_VALID();
                
                    static const String strings[] = 
                    {
                        {"Cast Spell", 0, 0, 8, Font::CENTER},
                        {0, 1, 0, 28, Font::CENTER},
                        {"Spell Ready:", 0, 8, 48, 0},
                        {0, 2, 0, 68, Font::CENTER},
                
                        {"Cost", 0, 8, 90, Font::SMALL}, {0, 3, 0, 90, Font::SMALL | Font::ALIGN_RIGHT},
                        {"Cur SP", 0, 8, 99, Font::SMALL},  {0, 4, 0, 99, Font::SMALL | Font::ALIGN_RIGHT},
                
                        {"Cast", 0, 9, 130, Font::SMALL}, {"New", 0, 38, 130, Font::SMALL}, {"ESC", 0, 66, 130, Font::SMALL}, // TODO: Highlight 'C' and 'N'
                
                        {(uint16)0, 0, 0, 0, 0}
                    };
                    
                    return strings;
                }
        
                void produceString(unsigned id)
                {
                    XEEN_VALID();
                
                    Character* character = getGame()->getActiveCharacter();
                
                    if(valid(character))
                    {
                        switch(id)
                        {
                            case  1: fillStringBuffer("%s", character->getName()); return;
                            case  2: fillStringBuffer("Light"); return; // TODO: Use real spell name, make yellow
                            case  3: fillStringBuffer("1/0"); return; //TODO: Use real cost
                            case  4: fillStringBuffer("2"); return; //TODO: Use real cur sp
                        }
                    }
                }
        };
    }
}


XEEN::Event::CastSpell::CastSpell(Valid<Game> parent) : Event(parent)
{
    addWindow(new CastSpellWindow(parent));
}

void XEEN::Event::CastSpell::process()
{
    CastSpellWindow* window = (CastSpellWindow*)(Window*)getWindows().back();
        
    switch(window->action)
    {
        case 1: setFinished(true, true); /* TODO: Cast spell */ break;
        case 2: getGame()->setEvent(new SelectSpell(getGame())); break;
        case 3: setFinished(true, true); break;
    }

    window->action = 0;
}
