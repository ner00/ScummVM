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
#include "xeen/party.h"
#include "xeen/ui/basicwindows.h"

XEEN::MovementWindow::MovementWindow(Valid<Game> parent) : Window(parent, Common::Rect(0, 0, 0, 0))
{
    
}

const XEEN::Button* XEEN::MovementWindow::getButtons() const
{
    XEEN_VALID();

    static const Button buttons[] = 
    {
        {CCSpriteId("MAIN.ICN", 18), CCSpriteId("MAIN.ICN", 19), {109, 137, 24, 20}, 1, Common::KEYCODE_TAB}, //TODO: SIZE?
        {CCSpriteId("MAIN.ICN", 20), CCSpriteId("MAIN.ICN", 21), {235, 148, 24, 20}, 2, Common::KEYCODE_LEFT}, 
        {CCSpriteId("MAIN.ICN", 22), CCSpriteId("MAIN.ICN", 23), {260, 148, 24, 20}, 3, Common::KEYCODE_UP}, 
        {CCSpriteId("MAIN.ICN", 24), CCSpriteId("MAIN.ICN", 25), {286, 148, 24, 20}, 4, Common::KEYCODE_RIGHT}, 
        {CCSpriteId("MAIN.ICN", 26), CCSpriteId("MAIN.ICN", 27), {235, 169, 24, 20}, 5, Common::KEYCODE_INVALID}, //TODO: Direct left 
        {CCSpriteId("MAIN.ICN", 28), CCSpriteId("MAIN.ICN", 29), {260, 169, 24, 20}, 6, Common::KEYCODE_DOWN}, 
        {CCSpriteId("MAIN.ICN", 30), CCSpriteId("MAIN.ICN", 31), {286, 169, 24, 20}, 7, Common::KEYCODE_INVALID}, //TODO: Direct right

        {0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
    };
    
    return buttons;
}

void XEEN::MovementWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    Party* party = getGame()->getParty();
    switch(id)
    {
        case 1: /* TODO */ break;
        case 2: party->turn(true); break;
        case 3: party->moveRelative(Direction::NORTH); break;
        case 4: party->turn(false); break;
        case 5: party->moveRelative(Direction::WEST); break;
        case 6: party->moveRelative(Direction::SOUTH); break;
        case 7: party->moveRelative(Direction::EAST); break;
    }
}
