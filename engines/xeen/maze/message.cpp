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
    class SmallMessageWindow : public Window
    {
        public:
            SmallMessageWindow(Valid<Game> parent, NonNull<const char> msg) : Window(parent, Common::Rect(225, 140, 318, 198)), _msg(msg) { }

        protected:
            const String* getStrings() const
            {
                XEEN_VALID();
            
                static const String strings[] = 
                {
                    {0, 1, 0, 8, Font::CENTER},
                    {0, 0, 0, 0, 0}
                };
                
                return strings;
            }

            void produceString(unsigned id)
            {
                XEEN_VALID();
                fillStringBuffer("%s", (const char*)_msg);
            }

        private:
            NonNull<const char> _msg;
    };
}

XEEN::Maze::Message::Message(Valid<Game> parent, const EventState& state, NonNull<const char> msg) : MazeEvent(parent, state), _done(false)
{
    // TODO: Use proper window size
    addWindow(new SmallMessageWindow(getGame(), msg));
}

void XEEN::Maze::Message::process()
{
    if(!_done)
    {
        _state.runFrom(_state.line + 1);
        _done = true;
    }
    else
    {
        setFinished(true, true);
    }
}
