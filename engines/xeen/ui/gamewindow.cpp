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
    XEEN_VALID_RET(0);

    static const Button buttons[] = 
    {
        {"MAIN.ICN",  0,  1, {235,  75, 24, 20}, 0}, {"MAIN.ICN", 16, 17, {286, 117, 24, 20}, 8}, 
        {"MAIN.ICN",  2,  3, {260,  75, 24, 20}, 1}, {"MAIN.ICN", 18, 19, {109, 137, 24, 20}, 9}, //TODO: SIZE?
        {"MAIN.ICN",  4,  5, {286,  75, 24, 20}, 2}, {"MAIN.ICN", 20, 21, {235, 148, 24, 20}, 10}, 
        {"MAIN.ICN",  6,  7, {235,  96, 24, 20}, 3}, {"MAIN.ICN", 22, 23, {260, 148, 24, 20}, 11}, 
        {"MAIN.ICN",  8,  9, {260,  96, 24, 20}, 4}, {"MAIN.ICN", 24, 25, {286, 148, 24, 20}, 12}, 
        {"MAIN.ICN", 10, 11, {286,  96, 24, 20}, 5}, {"MAIN.ICN", 26, 27, {235, 169, 24, 20}, 13}, 
        {"MAIN.ICN", 12, 13, {235, 117, 24, 20}, 6}, {"MAIN.ICN", 28, 29, {260, 169, 24, 20}, 14}, 
        {"MAIN.ICN", 14, 15, {260, 117, 24, 20}, 7}, {"MAIN.ICN", 30, 31, {286, 169, 24, 20}, 15}, 

        {0, 0, 0, {0, 0, 0, 0}, 0}
    };
    
    return buttons;
}

void XEEN::GameWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    if(valid(XEENgame))
    {
        Party& party = XEENgame.getParty();
        
        if(valid(party))
        {
            switch(id)
            {
                // MOVEMENT
                case 10: party.facing --; break;
                case 11: party.position = Map::translatePoint(party.position, 0, 1, party.facing & 3); break;
                case 12: party.facing ++; break;
                case 13: party.position = Map::translatePoint(party.position, -1, 0, party.facing & 3); break;
                case 14: party.position = Map::translatePoint(party.position, 0, -1, party.facing & 3); break;
                case 15: party.position = Map::translatePoint(party.position, 1, 0, party.facing & 3); break;                
            }
        }
    }
}
