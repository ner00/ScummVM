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
#include "common/system.h"

#include "xeen/game.h"
#include "xeen/ui/castwindow.h"
#include "xeen/party.h"
#include "xeen/characters.h"

XEEN::CastWindow::CastWindow() : Window(Common::Rect(226, 0, 226 + 94, 146), false)
{
}

const XEEN::Button* XEEN::CastWindow::getButtons() const
{
    XEEN_VALID_RET(0);

    static const Button buttons[] = 
    {
        {"CAST.ICN",  0,  1, { 8, 108, 24, 20}, 0},
        {"CAST.ICN",  2,  3, {35, 108, 24, 20}, 1},
        {"CAST.ICN",  4,  5, {62, 108, 24, 20}, 2},

        {(uint16)0, 0, 0, {0, 0, 0, 0}, 0}
    };
    
    return buttons;
}

void XEEN::CastWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    switch(id)
    {
        case 2: XEENgame.showWindow(Game::NONE); return;
    }
}

const XEEN::String* XEEN::CastWindow::getStrings() const
{
    XEEN_VALID_RET(0);

    static const String strings[] = 
    {
        {"Cast Spell", 0, 18, 8},

        {0, 1, 8, 28}, // TODO: Center

        {"Spell Ready:", 0, 8, 48},
        {"Cost", 0, 8, 90}, // TODO: Small font
        {"Cur SP", 0, 8, 99}, // TODO: Small font

        {"Cast", 0, 9, 130}, {"New", 0, 38, 130}, {"ESC", 0, 66, 130}, // TODO: Small font, Highlight 'C' and 'N'

        {(uint16)0, 0, 0, 0}
    };
    
    return strings;
}

const char* XEEN::CastWindow::produceString(unsigned id)
{
    XEEN_VALID_RET("");

    stringBuffer[0] = 0;

    if(valid(XEENgame) && valid(XEENgame.getParty()))
    {
        Party& party = XEENgame.getParty();
        Character* character = XEENgame.getActiveCharacter();

        if(valid(character))
        {
            switch(id)
            {
                case  1: snprintf(stringBuffer, sizeof(stringBuffer), "%s", character->getName()); break;
            }

            return stringBuffer;
        }
    }

    return "";
}
