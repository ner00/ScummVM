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
#include "xeen/event/basicevents.h"

XEEN::GameWindow::GameWindow(Valid<Game> parent) : Window(parent, Common::Rect(235, 75, 309, 136))
{
    
}

const XEEN::Button* XEEN::GameWindow::getButtons() const
{
    XEEN_VALID();

    static const Button buttons[] = 
    {
        {"MAIN.ICN",  0,  1, {  0,   0, 24, 20}, 0, Common::KEYCODE_s},
        {"MAIN.ICN",  2,  3, { 25,   0, 24, 20}, 1, Common::KEYCODE_c}, 
        {"MAIN.ICN",  4,  5, { 51,   0, 24, 20}, 2, Common::KEYCODE_r}, 
        {"MAIN.ICN",  6,  7, {  0,  21, 24, 20}, 3, Common::KEYCODE_b}, 
        {"MAIN.ICN",  8,  9, { 25,  21, 24, 20}, 4, Common::KEYCODE_d}, 
        {"MAIN.ICN", 10, 11, { 51,  21, 24, 20}, 5, Common::KEYCODE_v}, 
        {"MAIN.ICN", 12, 13, {  0,  42, 24, 20}, 6, Common::KEYCODE_m}, 
        {"MAIN.ICN", 14, 15, { 25,  42, 24, 20}, 7, Common::KEYCODE_i},
        {"MAIN.ICN", 16, 17, { 51,  42, 24, 20}, 8, Common::KEYCODE_q},  

        {(uint16)0, 0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
    };
    
    return buttons;
}

void XEEN::GameWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    switch(id)
    {
        case  1: getGame()->setEvent(new Event::CastSpell(getGame())); break;
        case  3: getGame()->getParty()->bash(); break;
//        case  4: getGame()->showWindow(Game::CHARACTION); break;
        case  7: getGame()->setEvent(new Event::GameInfo(getGame())); break;
        case  8: getGame()->setEvent(new Event::QuickReference(getGame())); break;
    }
}

//

XEEN::GameWindow_YesNo::GameWindow_YesNo(Valid<Game> parent) : Window(parent, Common::Rect(235, 75, 309, 136))
{
    
}

const XEEN::Button* XEEN::GameWindow_YesNo::getButtons() const
{
    XEEN_VALID();

    static const Button buttons[] = 
    {
        {"MAIN.ICN",  0,  1, {  0,   0, 24, 20}, 1, Common::KEYCODE_y},
        {"MAIN.ICN",  2,  3, { 25,   0, 24, 20}, 2, Common::KEYCODE_n},
        {(uint16)0, 0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
    };
    
    return buttons;
}
