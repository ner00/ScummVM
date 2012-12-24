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
#define XEEN_MAZE_SOURCE

#include "xeen/game.h"
#include "xeen/party.h"
#include "xeen/maze/events_.h"
#include "xeen/maze/eventlist_.h"

namespace XEEN
{
    class WhoWillWindow : public Window
    {
        public:
            bool canceled;

        public:
            WhoWillWindow(Valid<Game> parent, NonNull<const char> msg) : Window(parent, Common::Rect(225, 74, 319, 154)), _msg(msg), canceled(false) { }

        protected:
            const Button* getButtons() const
            {
                static const Button buttons[] = 
                {
                    // TODO: Fill proper values; check HP bar size
                    {"MAIN.ICN",  0,  0, { 10, 150, 32, 32}, 1, Common::KEYCODE_ESCAPE},
                    {(uint16)0, 0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
                };

                return buttons;            
            };

            void handleAction(unsigned id)
            {
                canceled = canceled || (id == 1);
            }

            const String* getStrings() const
            {
                XEEN_VALID();
            
                static const String strings[] = 
                {
                    {0, 1, 0, 8, Font::CENTER},
                    {0, 2, 0, 28, Font::CENTER},
                    {0, 3, 0, 63, Font::CENTER},
                    {0, 0, 0, 0, 0}
                };
                
                return strings;
            }

            void produceString(unsigned id)
            {
                XEEN_VALID();

                switch(id)
                {
                    case 1: fillStringBuffer("%s", (const char*)_msg); return;
                    case 2: fillStringBuffer("Who will %s?", "{WHAT}"); return; // TODO: Fill proper value
                    case 3: fillStringBuffer("F1 - F%d", 6); return; // TODO: Fill proper value 
                }
            }

        private:
            NonNull<const char> _msg;
    };

    class ChooseWindow : public CharacterWindow
    {
        public:
            int32 chara;

            ChooseWindow(Valid<Game> parent) : CharacterWindow(parent), chara(-1) { }

            void handleAction(unsigned id)
            {
                chara = id;
            }
    };
}

XEEN::Maze::WhoWill::WhoWill(Valid<Game> parent, const EventState& state, NonNull<const char> msg) : MazeEvent(parent, state)
{
    setCharacterWindow(new ChooseWindow(getGame()));
    addWindow(new WhoWillWindow(getGame(), msg));
}

void XEEN::Maze::WhoWill::process()
{
    Valid<WhoWillWindow> whowill = (WhoWillWindow*)(Window*)getWindows().back();
    Valid<ChooseWindow> choose = (ChooseWindow*)getCharacterWindow();

    if(choose->chara >= 0)
    {
        debug("%d", choose->chara); // TODO: Handle

        _state.runFrom(_state.line + 1);
        setFinished(true, true);
    }
    else if(whowill->canceled)
    {
        setFinished(true, true);
    }
}
