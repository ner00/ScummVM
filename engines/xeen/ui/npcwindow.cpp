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

XEEN::NPCWindow::NPCWindow(Valid<Game> parent, NonNull<const char> name, NonNull<const char> msg) :
    Window(parent, Common::Rect(8, 8, 324, 140), true), _name(name), _msg(msg)
{
    _clickToFinish = true;
}

const XEEN::String* XEEN::NPCWindow::getStrings() const
{
    XEEN_VALID();

    static const String strings[] = 
    {
        {0, 1, 0, 30, Font::CENTER},
        {0, 2, 0, 70, Font::CENTER},
        {0, 0, 0, 0, 0}
    };
    
    return strings;
}

void XEEN::NPCWindow::produceString(unsigned id)
{
    XEEN_VALID();

    fillStringBuffer("%s", (id == 1) ? _name : _msg);
}
