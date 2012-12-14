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
#include "xeen/ui/scrollwindow.h"
#include "xeen/party.h"
#include "xeen/characters.h"

#include "xeen/graphics/font.h"

XEEN::ScrollWindow::ScrollWindow() : Window(Common::Rect(27, 6, 27 + 180, 6 + 136)), _selected(0)
{
}

const XEEN::Button* XEEN::ScrollWindow::getButtons() const
{
    XEEN_VALID();

    static Button buttons[] = 
    {
        // TODO: Add keycodes
        {"SCROLL.ICN", 0, 1, {160,  20,  11,  10}, 0, Common::KEYCODE_INVALID},
        {"SCROLL.ICN", 2, 3, {160, 105,  11,  10}, 4, Common::KEYCODE_INVALID},
        {"SCROLL.ICN", 4, 4, { 12,  20, 100, 100}, 3, Common::KEYCODE_INVALID},
        {"SCROLL.ICN", 5, 5, {105, 117,  36,  10}, 1, Common::KEYCODE_INVALID},
        {(uint16)0,    0, 0, {147, 117,  24,  10}, 2, Common::KEYCODE_INVALID},
        {(uint16)0, 0, 0, {0, 0, 0, 0}, 0, Common::KEYCODE_INVALID}
    };
    
    return buttons;
}

void XEEN::ScrollWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    switch(id)
    {
        case 1: accept(); XEENgame.showWindow(Game::NONE); return;
        case 2: cancel(); XEENgame.showWindow(Game::NONE); return;
    }
}

const XEEN::String* XEEN::ScrollWindow::getStrings() const
{
    XEEN_VALID();

    static const String strings[] = 
    {
        {0,   11,   0,  10, Font::CENTER | Font::SMALL},
        {"1",  0,   8,  23, Font::SMALL}, {0,  1,  18,  23, Font::SMALL},
        {"2",  0,   8,  32, Font::SMALL}, {0,  2,  18,  32, Font::SMALL},
        {"3",  0,   8,  41, Font::SMALL}, {0,  3,  18,  41, Font::SMALL},
        {"4",  0,   8,  50, Font::SMALL}, {0,  4,  18,  50, Font::SMALL},
        {"5",  0,   8,  59, Font::SMALL}, {0,  5,  18,  59, Font::SMALL},
        {"6",  0,   8,  68, Font::SMALL}, {0,  6,  18,  68, Font::SMALL},
        {"7",  0,   8,  77, Font::SMALL}, {0,  7,  18,  77, Font::SMALL},
        {"8",  0,   8,  86, Font::SMALL}, {0,  8,  18,  86, Font::SMALL},
        {"9",  0,   8,  95, Font::SMALL}, {0,  9,  18,  95, Font::SMALL},
        {"0",  0,   8, 104, Font::SMALL}, {0, 10,  18, 104, Font::SMALL},
        {0,   12,  12, 118, Font::SMALL},
        {(uint16)0, 0, 0, 0, 0}
    };
    
    return strings;
}

void XEEN::ScrollWindow::produceString(unsigned id)
{
    XEEN_VALID();

    if(id >= 1 && id <= 10)
    {
        fillItemText(id - 1);
    }
    else if(id == 11)
    {
        fillHeader();
    }
    else if(id == 12)
    {
        fillFooter();
    }
}

///
/// SpellSelectWindow
///
XEEN::SpellSelectWindow::SpellSelectWindow()
{

}

unsigned XEEN::SpellSelectWindow::itemCount() const
{
    XEEN_VALID();
    return 3;
}

void XEEN::SpellSelectWindow::selectItem(unsigned item)
{
    XEEN_VALID();
}

void XEEN::SpellSelectWindow::fillItemText(unsigned item)
{
    XEEN_VALID();

    static const char* const spellhack[] = {"Awaken", "First Aid", "Light"};
    if(item < 3)
    {
        fillStringBuffer("%s", spellhack[item]);
    }
}

void XEEN::SpellSelectWindow::fillHeader()
{
    XEEN_VALID();

    if(valid(XEENgame))
    {
        Character* chara = XEENgame.getActiveCharacter();

        if(valid(chara))
        {
            fillStringBuffer("Spells for %s", chara->getName()); //TODO: Fill proper name
        }
    }
}

void XEEN::SpellSelectWindow::fillFooter()
{
    XEEN_VALID();

    // TODO
    fillStringBuffer("Test");
}

void XEEN::SpellSelectWindow::accept()
{
    XEEN_VALID();
}
