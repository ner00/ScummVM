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

#include "xeen/window.h"
#include "xeen/ccfile.h"
#include "xeen/sprite.h"
#include "xeen/font.h"

XEEN::Window::Window(const Common::Rect& area) : _area(area)
{
}

void XEEN::Window::draw(ImageBuffer& out, CCFile& assets)
{
    const Common::Point location(_area.left, _area.top);
    
    // FILL RECT !!!
    
    // Draw buttons
    SpriteManager& sprites = assets.getSpriteManager();
    
    for(const Button* button = getButtons(); button && button->sprite; button ++)
    {
        Sprite* const icon = sprites.getSprite(button->sprite);
        
        if(enforce(icon))
        {
            icon->drawCell(out, location + Common::Point(button->x, button->y), button->normalFrame);
        }
    }
    
    // Draw strings
    Font& font = assets.getFont();
    
    for(const String* string = getStrings(); string && string->text; string ++)
    {
        font.drawString(out, location + Common::Point(string->x, string->y), string->text);
    }
}

XEEN::CharacterStatusWindow::CharacterStatusWindow() : Window(Common::Rect(0, 0, 320, 146))
{
}

            
const XEEN::Button* XEEN::CharacterStatusWindow::getButtons() const
{
    static const Button buttons[] = 
    {
        {"VIEW.ICN",  0,  1,  10,  24, 0}, {"VIEW.ICN", 10, 11,  61,  24, 0}, {"VIEW.ICN", 20, 21, 112,  24, 0}, {"VIEW.ICN", 30, 31, 177,  24, 0},
        {"VIEW.ICN",  2,  3,  10,  47, 0}, {"VIEW.ICN", 12, 13,  61,  47, 0}, {"VIEW.ICN", 22, 23, 112,  47, 0}, {"VIEW.ICN", 32, 33, 177,  47, 0},
        {"VIEW.ICN",  4,  5,  10,  70, 0}, {"VIEW.ICN", 14, 15,  61,  70, 0}, {"VIEW.ICN", 24, 25, 112,  70, 0}, {"VIEW.ICN", 34, 35, 177,  70, 0},
        {"VIEW.ICN",  6,  7,  10,  93, 0}, {"VIEW.ICN", 16, 17,  61,  93, 0}, {"VIEW.ICN", 26, 27, 112,  93, 0}, {"VIEW.ICN", 36, 37, 177,  93, 0},
        {"VIEW.ICN",  8,  9,  10, 116, 0}, {"VIEW.ICN", 18, 19,  61, 116, 0}, {"VIEW.ICN", 28, 29, 112, 116, 0}, {"VIEW.ICN", 38, 39, 177, 116, 0},

        {"VIEW.ICN", 40, 41, 285, 11, 0},  {"VIEW.ICN", 42, 43, 285, 43, 0},  {"VIEW.ICN", 44, 45, 285, 75, 0},  {"VIEW.ICN", 46, 47, 285, 107, 0}, 

        {0, 0, 0, 0, 0, 0}
    };
    
    return buttons;
}

const XEEN::String* XEEN::CharacterStatusWindow::getStrings() const
{
    static const String strings[] = 
    {
        {"Mgt", 37,  26}, {"Acy", 88,  26}, {"H.P.",   139,  26}, {"Experience", 204,  26},
        {"Int", 37,  49}, {"Lck", 88,  49}, {"S.P.",   139,  49}, {"Party Gold", 204,  49},
        {"Per", 37,  70}, {"Age", 88,  70}, {"Resis",  139,  70}, {"Party Gems", 204,  70},
        {"End", 37,  93}, {"Lvl", 88,  93}, {"Skills", 139,  93}, {"Party Food", 204,  93},
        {"Spd", 37, 116}, {"AC",  88, 116}, {"Awrds",  139, 116}, {"Condition",  204, 116},

        {0, 0, 0}
    };
    
    return strings;
}

