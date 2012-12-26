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
    class NPCWindow : public Window
    {
        public:
            NPCWindow(Valid<Game> parent, bool wantConfirm, NonNull<const char> name, NonNull<const char> msg) : 
                Window(parent, Common::Rect(8, 8, 224, 140), !wantConfirm), _name(name), _msg(msg) { }


        protected:
            const Button* getButtons() const
            {
                XEEN_VALID();

                static const Button buttons[] = 
                {
                    {CCSpriteId("FACE01.FAC", 0, 4), CCSpriteId("FACE01.FAC", 0, 4), {16, 16, 62, 40}, 0, Common::KEYCODE_INVALID},
                    {0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
                };

                return buttons;
            }

            const String* getStrings() const
            {
                XEEN_VALID();
            
                static const String strings[] = 
                {
//                    {0, 1, 0, 30, Font::CENTER},
                    {0, 2, 0, 70, Font::CENTER},
                    {0, 0, 0, 0, 0}
                };
                
                return strings;
            }

            void produceString(unsigned id)
            {
                XEEN_VALID();
            
                fillStringBuffer("%s", (id == 1) ? (const char*)_name : (const char*)_msg);
            }

        private:
            NonNull<const char> _name;
            NonNull<const char> _msg;
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

XEEN::Maze::NPC::NPC(Valid<Game> parent, const EventState& state, NonNull<const char> name, NonNull<const char> msg) : MazeEvent(parent, state), _done(false)
{
    const uint32 confirmType = _state.getByteAt(9);

    // TODO: Confirm type 2 = don't wait?

    addWindow(new NPCWindow(getGame(), confirmType == 0, name, msg));

    if(confirmType == 0)
    {
        setCommandWindow(new ConfirmWindow(parent));
    }
}

void XEEN::Maze::NPC::process()
{
    const uint32 confirmType = _state.getByteAt(9);
    ConfirmWindow* commands = (ConfirmWindow*)getCommandWindow();

    if(!_done)
    {
        if(valid(commands)) // Yes/No
        {
            switch(commands->action)
            {
                case 1: // YES
                {
                    _state.runFrom(_state.getByteAt(10));
                    setFinished(true, true);
                }
    
                case 2: // NO
                {
                    _state.runFrom(_state.line + 1);
                    setFinished(true, true);
                }
    
                return;
            }
        }
        else if(confirmType == 1) // Wait for key press
        {
            if(getWindows().back()->isFinished())
            {
                setFinished(true, true);
                _state.runFrom(_state.line + 1);
            }
        }
        else // No wait
        {
            _state.runFrom(_state.line + 1);
            _done = true;
        }
    }
    else
    {
        setFinished(true, true);
    }
}
