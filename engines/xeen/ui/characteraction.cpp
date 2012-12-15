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

XEEN::CharacterActionWindow::CharacterActionWindow(Valid<Game> parent) : Window(parent, Common::Rect(50, 112, 50 + 216, 112 + 36))
{
}

            
const XEEN::Button* XEEN::CharacterActionWindow::getButtons() const
{
    XEEN_VALID();

    static const Button buttons[] = 
    {
        {"ESC.ICN",  0,  1, {175, 8, 24, 20}, 0, Common::KEYCODE_ESCAPE},

        {(uint16)0, 0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
    };
    
    return buttons;
}

void XEEN::CharacterActionWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    if(id == 0)
    {
        _parent->showWindow(Game::NONE);
    }
}



const XEEN::String* XEEN::CharacterActionWindow::getStrings() const
{
    XEEN_VALID();

    static const String strings[] = 
    {
        {"Dismiss Whom?", 0, 18, 13, 0}, // TODO: Allow customization
        {(uint16)0, 0, 0, 0, 0}
    };
    
    return strings;
}

void XEEN::CharacterActionWindow::produceString(unsigned id)
{
    XEEN_VALID();
}

