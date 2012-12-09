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
#include "xeen/ui/characterwindow.h"
#include "xeen/party.h"

XEEN::CharacterWindow::CharacterWindow() : Window(Common::Rect(0, 0, 0, 0))
{
    static const uint32 X = Button::NOACTION;

    static const Button buttons[] = 
    {
        // TODO: Fill proper values; check HP bar size
        {"CHAR01.FAC",  0,  0, { 10, 150, 32, 32},  0}, {"HPBARS.ICN", 0, 0, { 14, 182, 23, 8}, X},
        {"CHAR02.FAC",  0,  0, { 45, 150, 32, 32},  1}, {"HPBARS.ICN", 0, 0, { 50, 182, 23, 8}, X},
        {"CHAR03.FAC",  0,  0, { 81, 150, 32, 32},  2}, {"HPBARS.ICN", 0, 0, { 87, 182, 23, 8}, X},
        {"CHAR04.FAC",  0,  0, {117, 150, 32, 32},  3}, {"HPBARS.ICN", 0, 0, {122, 182, 23, 8}, X},
        {"CHAR05.FAC",  0,  0, {153, 150, 32, 32},  4}, {"HPBARS.ICN", 0, 0, {159, 182, 23, 8}, X},
        {"CHAR06.FAC",  0,  0, {189, 150, 32, 32},  5}, {"HPBARS.ICN", 0, 0, {195, 182, 23, 8}, X},

        {(uint16)0, 0, 0, {0, 0, 0, 0}, 0}
    };

    memcpy(_buttons, buttons, sizeof(_buttons));
}

void XEEN::CharacterWindow::show()
{
    // TODO: Update when needed
    if(valid(XEENgame))
    {
        Party* party = XEENgame.getParty();
        
        if(valid(party))
        {
            for(unsigned i = 0; i != party->getValue(Party::PARTY_COUNT); i ++)
            {
                _buttons[i * 2].sprite = CCFileId("CHAR%02d.FAC", party->getMemberIdFromSlot(i) + 1);
            }
        }
    }
}

const XEEN::Button* XEEN::CharacterWindow::getButtons() const
{
    XEEN_VALID_RET(0);

    return _buttons;
}

void XEEN::CharacterWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    if(valid(XEENgame))
    {
        XEENgame.selectCharacter(id);
        XEENgame.showWindow(Game::STATUS);
    }
}
