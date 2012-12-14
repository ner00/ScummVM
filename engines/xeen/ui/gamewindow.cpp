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
#include "xeen/party.h"

#include "xeen/maze/map.h"

#include "xeen/ui/gamewindow.h"

XEEN::GameWindow::GameWindow() : Window(Common::Rect(0, 0, 0, 0))
{
    
}

const XEEN::Button* XEEN::GameWindow::getButtons() const
{
    XEEN_VALID();

    static const Button buttons[] = 
    {
        {"MAIN.ICN",  0,  1, {235,  75, 24, 20}, 0, Common::KEYCODE_s}, {"MAIN.ICN", 16, 17, {286, 117, 24, 20}, 8, Common::KEYCODE_q}, 
        {"MAIN.ICN",  2,  3, {260,  75, 24, 20}, 1, Common::KEYCODE_c}, {"MAIN.ICN", 18, 19, {109, 137, 24, 20}, 9, Common::KEYCODE_TAB}, //TODO: SIZE?
        {"MAIN.ICN",  4,  5, {286,  75, 24, 20}, 2, Common::KEYCODE_r}, {"MAIN.ICN", 20, 21, {235, 148, 24, 20}, 10, Common::KEYCODE_LEFT}, 
        {"MAIN.ICN",  6,  7, {235,  96, 24, 20}, 3, Common::KEYCODE_b}, {"MAIN.ICN", 22, 23, {260, 148, 24, 20}, 11, Common::KEYCODE_UP}, 
        {"MAIN.ICN",  8,  9, {260,  96, 24, 20}, 4, Common::KEYCODE_d}, {"MAIN.ICN", 24, 25, {286, 148, 24, 20}, 12, Common::KEYCODE_RIGHT}, 
        {"MAIN.ICN", 10, 11, {286,  96, 24, 20}, 5, Common::KEYCODE_v}, {"MAIN.ICN", 26, 27, {235, 169, 24, 20}, 13, Common::KEYCODE_INVALID}, //TODO: Direct left 
        {"MAIN.ICN", 12, 13, {235, 117, 24, 20}, 6, Common::KEYCODE_m}, {"MAIN.ICN", 28, 29, {260, 169, 24, 20}, 14, Common::KEYCODE_DOWN}, 
        {"MAIN.ICN", 14, 15, {260, 117, 24, 20}, 7, Common::KEYCODE_i}, {"MAIN.ICN", 30, 31, {286, 169, 24, 20}, 15, Common::KEYCODE_INVALID}, //TODO: Direct right

        {(uint16)0, 0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
    };
    
    return buttons;
}

void XEEN::GameWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    if(valid(XEENgame))
    {
        Party* party = XEENgame.getParty();
        
        if(valid(party))
        {
            switch(id)
            {
                case  1: XEENgame.showWindow(Game::CASTSPELL); break;
                case  4: XEENgame.showWindow(Game::CHARACTION); break;
                case  7: XEENgame.showWindow(Game::GAMEINFO); break;
                case  8: XEENgame.showWindow(Game::QUICKREF); break;

                // MOVEMENT
                case 10: party->turn(true); break;
                case 11: party->moveRelative(Common::Point(0, 1)); break;
                case 12: party->turn(false); break;
                case 13: party->moveRelative(Common::Point(-1, 0)); break;
                case 14: party->moveRelative(Common::Point(0, -1)); break;
                case 15: party->moveRelative(Common::Point(1, 0)); break;
            }
        }
    }
}
