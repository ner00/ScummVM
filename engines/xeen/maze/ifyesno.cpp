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
    class DummyWindow : public Window
    {
        public:
            DummyWindow(Valid<Game> parent) : Window(parent, Common::Rect(320, 0, 321, 1), true) { }
    };

    class ConfirmWindow : public GameWindow_YesNo
    {
        public:
            ConfirmWindow(Valid<Game> game) : GameWindow_YesNo(game), action(0) { }

        protected:
            void handleAction(unsigned id)
            {
                action = id;
            }

        public:
            uint32 action;
    };
}

XEEN::Maze::IfYesNo::IfYesNo(Valid<Game> parent, const EventState& state) : MazeEvent(parent, state)
{
    const uint32 type = _state.getByteAt(7);

    if(enforce(type < 2))
    {
        if(type == 1) // Just Wait
        {
            addWindow(new DummyWindow(parent));
        }
        else
        {
            setCommandWindow(new ConfirmWindow(parent));
        }
    }
}

void XEEN::Maze::IfYesNo::process()
{
    ConfirmWindow* commands = (ConfirmWindow*)getCommandWindow();

    if(valid(commands))
    {
        switch(commands->action)
        {
            case 1: // YES
            {
                _state.runFrom(_state.getByteAt(8));
                setFinished(true, true);
                removeCommandWindow();
                return;
            }

            case 2: // NO
            {
                _state.runFrom(_state.line + 1);
                setFinished(true, true);
                removeCommandWindow();
                return;
            }
        }
    }
    else if(getWindows().back()->isFinished())
    {
        setFinished(true, true);
        _state.runFrom(_state.getByteAt(8));
    }
}
