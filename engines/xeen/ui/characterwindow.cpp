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
#include "xeen/event/basicevents.h"
#include "xeen/party.h"

XEEN::CharacterWindow::CharacterWindow(Valid<Game> parent) : Window(parent, Common::Rect(0, 0, 0, 0))
{
    static const uint32 X = Button::NOACTION;

    static const Button buttons[] = 
    {
        // TODO: Fill proper values; check HP bar size
        {CCSpriteId("CHAR01.FAC", 0), CCSpriteId("CHAR01.FAC", 0), { 10, 150, 32, 32},  0, Common::KEYCODE_F1}, {CCSpriteId("HPBARS.ICN", 0), CCSpriteId("HPBARS.ICN", 0), { 14, 182, 23, 8}, X, Common::KEYCODE_INVALID},
        {CCSpriteId("CHAR01.FAC", 0), CCSpriteId("CHAR01.FAC", 0), { 45, 150, 32, 32},  1, Common::KEYCODE_F2}, {CCSpriteId("HPBARS.ICN", 0), CCSpriteId("HPBARS.ICN", 0), { 50, 182, 23, 8}, X, Common::KEYCODE_INVALID},
        {CCSpriteId("CHAR01.FAC", 0), CCSpriteId("CHAR01.FAC", 0), { 81, 150, 32, 32},  2, Common::KEYCODE_F3}, {CCSpriteId("HPBARS.ICN", 0), CCSpriteId("HPBARS.ICN", 0), { 87, 182, 23, 8}, X, Common::KEYCODE_INVALID},
        {CCSpriteId("CHAR01.FAC", 0), CCSpriteId("CHAR01.FAC", 0), {117, 150, 32, 32},  3, Common::KEYCODE_F4}, {CCSpriteId("HPBARS.ICN", 0), CCSpriteId("HPBARS.ICN", 0), {122, 182, 23, 8}, X, Common::KEYCODE_INVALID},
        {CCSpriteId("CHAR01.FAC", 0), CCSpriteId("CHAR01.FAC", 0), {153, 150, 32, 32},  4, Common::KEYCODE_F5}, {CCSpriteId("HPBARS.ICN", 0), CCSpriteId("HPBARS.ICN", 0), {159, 182, 23, 8}, X, Common::KEYCODE_INVALID},
        {CCSpriteId("CHAR01.FAC", 0), CCSpriteId("CHAR01.FAC", 0), {189, 150, 32, 32},  5, Common::KEYCODE_F6}, {CCSpriteId("HPBARS.ICN", 0), CCSpriteId("HPBARS.ICN", 0), {195, 182, 23, 8}, X, Common::KEYCODE_INVALID},

        {0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
    };

    memcpy(_buttons, buttons, sizeof(_buttons));
}

void XEEN::CharacterWindow::show()
{
    XEEN_VALID();

    // TODO: Update when needed
    Party* party = getGame()->getParty();
    
    for(unsigned i = 0; i != party->getValue(Party::PARTY_COUNT); i ++)
    {
        CCFileId file("CHAR%02d.FAC", party->getMemberIdFromSlot(i) + 1);
        _buttons[i * 2].normalFrame = CCSpriteId(file);
        _buttons[i * 2].pressedFrame = CCSpriteId(file);
    }
}

const XEEN::Button* XEEN::CharacterWindow::getButtons() const
{
    XEEN_VALID();

    return _buttons;
}

void XEEN::CharacterWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    getGame()->selectCharacter(id);
    getGame()->setEvent(new Event::CharacterStatus(getGame()));
}
