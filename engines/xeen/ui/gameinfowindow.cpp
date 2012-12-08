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
#include "xeen/ui/gameinfowindow.h"
#include "xeen/party.h"

XEEN::GameInfoWindow::GameInfoWindow() : Window(Common::Rect(88, 20, 88 + 160, 20 + 92), true)
{
}

const XEEN::String* XEEN::GameInfoWindow::getStrings() const
{
    XEEN_VALID_RET(0);

    static const String strings[] = 
    {
        {"World of Xeen", 0, 40, 9}, // TODO: Yellow
        {"Game Information", 0, 29, 19},
        {"Today is", 0, 30, 39}, {0, 1, 83, 39}, // TODO: Should be one centered string, day should be yellow

        {"Time", 0, 28, 59}, {"Day", 0, 70, 59}, {"Year", 0, 106, 59},

        // TODO: Current Time day and year

        {(uint16)0, 0, 0, 0}
    };
    
    return strings;
}

const char* XEEN::GameInfoWindow::produceString(unsigned id)
{
    XEEN_VALID_RET("");

    // TODO

    return "";
}
