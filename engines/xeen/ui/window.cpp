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
#include "xeen/characters.h"

#include "xeen/graphics/imagebuffer.h"
#include "xeen/graphics/spritemanager.h"

#include "xeen/ui/window.h"

XEEN::Window::Window(const Common::Rect& area, bool clickToClose) : _area(area), _clickToClose(clickToClose), _pressedButton(0), _pressedTime(0)
{
}

void XEEN::Window::fillStringBuffer(const char* fmt, ...)
{
    XEEN_VALID();

    va_list args;
    va_start(args, fmt);
    vsnprintf(_stringBuffer, sizeof(_stringBuffer), fmt, args);
    va_end(args);
}

void XEEN::Window::draw(ImageBuffer& out)
{
    XEEN_VALID();

    const Common::Point location(_area.left, _area.top);

    // Background: TODO: Get correct color; draw border.    
    out.fillRect(_area, 21);
    
    // Draw buttons
    SpriteManager* sprites = XEENgame.getSpriteManager();
    
    for(const Button* button = getButtons(); button && (button->sprite || button->actionID); button ++)
    {
        if(button->sprite)
        {
            const uint32 frame = (button == _pressedButton) ? button->pressedFrame : button->normalFrame;
            sprites->draw(button->sprite, out, location + button->area, frame);
        }
    }
    
    // Draw strings
    for(const String* string = getStrings(); string && (string->text || string->stringID); string ++)
    {
        if(string->text)
        {
            XEENgame.drawString(out, location + Common::Point(string->x, string->y), string->text, string->flags, _area.width());
        }
        else
        {
            _stringBuffer[0] = 0;
            produceString(string->stringID);
            XEENgame.drawString(out, location + Common::Point(string->x, string->y), _stringBuffer, string->flags, _area.width());
        }
    }
}

void XEEN::Window::heartbeat()
{
    XEEN_VALID();

    if(_pressedButton && (g_system->getMillis() >= _pressedTime))
    {
        handleAction(_pressedButton->actionID);
        _pressedButton = 0;
    }
}

bool XEEN::Window::click(const Common::Point& point)
{

    XEEN_VALID_RET(false);

    Common::Point target = point - Common::Point(_area.left, _area.top);

    // Check buttons
    for(const Button* button = getButtons(); button && (button->sprite || button->actionID); button ++)
    {
        const Common::Rect r = button->area;
    
        if(button->actionID != Button::NOACTION && r.contains(target))
        {
            _pressedButton = button;
            _pressedTime = g_system->getMillis() + BUTTON_DELAY;
            return true;
        }
    }
    
    if(_clickToClose && valid(XEENgame))
    {
        XEENgame.showWindow(Game::NONE);
        return true;
    }
    
    return false;
}

XEEN::CharacterStatusWindow::CharacterStatusWindow() : Window(Common::Rect(0, 0, 320, 146))
{
}

            
const XEEN::Button* XEEN::CharacterStatusWindow::getButtons() const
{
    static const Button buttons[] = 
    {
        {"VIEW.ICN",  0,  1, { 10,  24, 24, 20},  0}, {"VIEW.ICN", 10, 11, { 61,  24, 24, 20},  1},
        {"VIEW.ICN", 20, 21, {112,  24, 24, 20},  2}, {"VIEW.ICN", 30, 31, {177,  24, 24, 20},  3},
        {"VIEW.ICN",  2,  3, { 10,  47, 24, 20},  4}, {"VIEW.ICN", 12, 13, { 61,  47, 24, 20},  5},
        {"VIEW.ICN", 22, 23, {112,  47, 24, 20},  6}, {"VIEW.ICN", 32, 33, {177,  47, 24, 20},  7},
        {"VIEW.ICN",  4,  5, { 10,  70, 24, 20},  8}, {"VIEW.ICN", 14, 15, { 61,  70, 24, 20},  9},
        {"VIEW.ICN", 24, 25, {112,  70, 24, 20}, 10}, {"VIEW.ICN", 34, 35, {177,  70, 24, 20}, 11},
        {"VIEW.ICN",  6,  7, { 10,  93, 24, 20}, 12}, {"VIEW.ICN", 16, 17, { 61,  93, 24, 20}, 13},
        {"VIEW.ICN", 26, 27, {112,  93, 24, 20}, 14}, {"VIEW.ICN", 36, 37, {177,  93, 24, 20}, 15},
        {"VIEW.ICN",  8,  9, { 10, 116, 24, 20}, 16}, {"VIEW.ICN", 18, 19, { 61, 116, 24, 20}, 17},
        {"VIEW.ICN", 28, 29, {112, 116, 24, 20}, 18}, {"VIEW.ICN", 38, 39, {177, 116, 24, 20}, 19},

        {"VIEW.ICN", 40, 41, {285,  11, 24, 20}, 20}, {"VIEW.ICN", 42, 43, {285,  43, 24, 20}, 21},
        {"VIEW.ICN", 44, 45, {285,  75, 24, 20}, 22}, {"VIEW.ICN", 46, 47, {285, 107, 24, 20}, 23}, 

        {(uint16)0, 0, 0, {0, 0, 0, 0}, 0}
    };
    
    return buttons;
}

const XEEN::String* XEEN::CharacterStatusWindow::getStrings() const
{
    static const String strings[] = 
    {
        {"Mgt",    0,  37,  26, 0}, {0,  1,  37,  34, 0}, {"Acy",        0,  88,  26, 0}, {0,  2,  88,  34, 0}, 
        {"H.P.",   0, 139,  26, 0}, {0,  3, 139,  34, 0}, {"Experience", 0, 204,  26, 0}, {0,  4, 204,  34, 0}, 
        {"Int",    0,  37,  49, 0}, {0,  5,  37,  57, 0}, {"Lck",        0,  88,  49, 0}, {0,  6,  88,  57, 0}, 
        {"S.P.",   0, 139,  49, 0}, {0,  7, 139,  57, 0}, {"Party Gold", 0, 204,  49, 0}, {0,  8, 204,  57, 0}, 
        {"Per",    0,  37,  70, 0}, {0,  9,  37,  78, 0}, {"Age",        0,  88,  70, 0}, {0, 10,  88,  78, 0}, 
        {"Resis",  0, 139,  70, 0}, {0, 11, 139,  78, 0}, {"Party Gems", 0, 204,  70, 0}, {0, 12, 204,  78, 0}, 
        {"End",    0,  37,  93, 0}, {0, 13,  37, 101, 0}, {"Lvl",        0,  88,  93, 0}, {0, 14,  88, 101, 0}, 
        {"Skills", 0, 139,  93, 0}, {0, 15, 139, 101, 0}, {"Party Food", 0, 204,  93, 0}, {0, 16, 204, 101, 0}, 
        {"Spd",    0,  37, 116, 0}, {0, 17,  37, 124, 0}, {"AC",         0,  88, 116, 0}, {0, 18,  88, 124, 0}, 
        {"Awrds",  0, 139, 116, 0}, {0, 19, 139, 124, 0}, {"Condition",  0, 204, 116, 0}, {0, 20, 204, 124, 0}, 

        {0, 21, 7, 12, 0},

        {(uint16)0, 0, 0, 0, 0}
    };
    
    return strings;
}

void XEEN::CharacterStatusWindow::handleAction(unsigned id)
{
    XEEN_VALID();

    if(valid(XEENgame))
    {
        switch(id)
        {
            //TODO: Make QUICKREF retrun to STATUS window
            case 21: XEENgame.showWindow(Game::QUICKREF); break;
            case 23: XEENgame.showWindow(Game::NONE); break;
        }
    }
}

void XEEN::CharacterStatusWindow::produceString(unsigned id)
{
    XEEN_VALID();

    if(valid(XEENgame) && valid(XEENgame.getParty()))
    {
        Party* party = XEENgame.getParty();
        Character* character = XEENgame.getActiveCharacter();
    
        if(valid(character))
        {
            switch(id)
            {
                case  1: fillStringBuffer("%d", character->getStat(MIGHT).getValue()); break;
                case  2: fillStringBuffer("%d", character->getStat(ACCURACY).getValue()); break;
                case  5: fillStringBuffer("%d", character->getStat(INTELLECT).getValue()); break;
                case  6: fillStringBuffer("%d", character->getStat(LUCK).getValue()); break;
                case  9: fillStringBuffer("%d", character->getStat(PERSONALITY).getValue()); break;
                case 13: fillStringBuffer("%d", character->getStat(ENDURANCE).getValue()); break;
                case 17: fillStringBuffer("%d", character->getStat(SPEED).getValue()); break;                
                case 14: fillStringBuffer("%d", character->getStat(LEVEL).getValue()); break;

                case  3: fillStringBuffer("%d", character->getValue(Character::HP)); break;
                case  4: fillStringBuffer("%d", character->getValue(Character::EXPERIENCE)); break;
                case  7: fillStringBuffer("%d", character->getValue(Character::SP)); break;
                case 10: fillStringBuffer("DANG"); break;
                case 11: fillStringBuffer("DANG"); break;

                case 15: fillStringBuffer("DANG"); break;

                case  8: fillStringBuffer("%d", party->getValue(Party::GOLD)); break;
                case 12: fillStringBuffer("%d", party->getValue(Party::GEMS)); break;
                case 16: fillStringBuffer("%d", party->getValue(Party::FOOD)); break; // TODO: / 3 / PARTY_COUNT: needs to be shown in days!
                

                case 18: fillStringBuffer("DANG"); break;
                case 19: fillStringBuffer("DANG"); break;
                case 20: fillStringBuffer("DANG"); break;
                
                case 21:
                {
                    fillStringBuffer("%s : %s %s %s", character->getName(), getSexName(character->getSex()),
                             getRaceName(character->getRace()), getClassName(character->getClass()));
                    break;
                }
            }
        }
    }
}

