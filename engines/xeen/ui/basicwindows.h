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

#ifndef XEEN_UI_BASICWINDOWS_H
#define XEEN_UI_BASICWINDOWS_H

#include "xeen/utility.h"
#include "xeen/ui/window.h"

namespace XEEN
{
    class Game;

    class CharacterActionWindow : public Window
    {
        public:
            CharacterActionWindow(Valid<Game> parent);
            
        protected:
            const Button* getButtons() const;            
            void handleAction(unsigned id);

            const String* getStrings() const;
            void produceString(unsigned id);
    };

    class CharacterWindow : public Window
    {
        public:
            CharacterWindow(Valid<Game> parent);
            
            void show();
            
        protected:
            const Button* getButtons() const;
            void handleAction(unsigned id);
            
        private:
            Button _buttons[13];
    };

    class GameWindow : public Window
    {
        public:
            GameWindow(Valid<Game> parent);
            
        protected:
            const Button* getButtons() const;
            void handleAction(unsigned id);
    };

    class GameWindow_YesNo : public Window
    {
        public:
            GameWindow_YesNo(Valid<Game> parent);

        protected:
            const Button* getButtons() const;
    };

    class MovementWindow : public Window
    {
        public:
            MovementWindow(Valid<Game> parent);
            
        protected:
            const Button* getButtons() const;
            void handleAction(unsigned id);
    };
}

#endif // XEEN_UI_BASICWINDOWS_H
