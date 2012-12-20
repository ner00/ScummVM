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

#ifndef XEEN_EVENT_BASICEVENTS_H
#define XEEN_EVENT_BASICEVENTS_H

#include "xeen/utility.h"
#include "xeen/event/event.h"

namespace XEEN
{
    class Game;

    namespace Event
    {
        class GameInfo : public Event
        {
            public:
                GameInfo(Valid<Game> parent);
                bool process();
        };

        class QuickReference : public Event
        {
            public:
                QuickReference(Valid<Game> parent);
                bool process();
        };

        class CastSpellWindow;
        class CastSpell : public Event
        {
            public:
                CastSpell(Valid<Game> parent);
                bool process();

            private:
                CastSpellWindow* _window;
        };

        class SelectSpell : public Event
        {
            public:
                SelectSpell(Valid<Game> parent);
                bool process();
        };
    }
}

#endif // XEEN_EVENT_BASICEVENTS_H
