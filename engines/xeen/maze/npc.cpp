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
            ConfirmWindow(Valid<Game> game, uint32 confirmType) : GameWindow_YesNo(game), action(0), type(confirmType) { }

        protected:
            void handleAction(unsigned id)
            {
                action = id;
            }

        public:
            uint32 action;
            const uint32 type;
    };
}

XEEN::Maze::NPC::NPC(Valid<Game> parent, const EventState& state, NonNull<const char> name, NonNull<const char> msg) : MazeEvent(parent, state)
{
    const uint32 confirmType = _state.getByteAt(9);
    const bool wantConfirm = confirmType != 1;

    addWindow(new NPCWindow(getGame(), wantConfirm, name, msg));

    if(wantConfirm)
    {
        setCommandWindow(new ConfirmWindow(parent, confirmType));
    }
}

void XEEN::Maze::NPC::process()
{
    ConfirmWindow* commands = (ConfirmWindow*)getCommandWindow();

    if(valid(commands))
    {
        switch(commands->action)
        {
            case 1: // YES
            {
                _state.runFrom((commands->type == 0) ? _state.getByteAt(10) : _state.line + 1);
                setFinished(true, true);
                removeCommandWindow();
                return;
            }

            case 2: // NO
            {
                if(commands->type == 0)
                {
                    _state.runFrom(_state.line + 1);
                }

                setFinished(true, true);
                removeCommandWindow();
                return;
            }
        }
    }
    else
    {
        setFinished(getWindows().back()->isFinished(), true);

        if(isFinished())
        {
            _state.runFrom(_state.line + 1);
        }
    }
}
