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
#include "xeen/characters.h"

#include "xeen/graphics/manager.h"

#include "xeen/ui/window.h"

XEEN::Window::Window(Valid<Game> parent, const Common::Rect& area, bool clickToFinish) :
    GameHolder(parent), _finished(false), _clickToFinish(clickToFinish), _area(area), _pressedButton(0), _pressedTime(0)
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

void XEEN::Window::draw()
{
    XEEN_VALID();

    const Common::Point location(_area.left, _area.top);
    Graphics::Manager* graphics = getGame()->getGraphicsManager();

    // Background: TODO: Get correct color; draw border.
    graphics->fillRect(_area, 21);
    
    // Draw buttons (TODO: Look only for actionID)
    for(const Button* button = getButtons(); button && (button->normalFrame || button->actionID); button ++)
    {
        if(button->normalFrame)
        {
            const CCSpriteId& frame = (button == _pressedButton) ? button->pressedFrame : button->normalFrame;
            graphics->draw(frame, location + button->area);
        }
    }
    
    // Draw strings
    for(const String* string = getStrings(); string && (string->text || string->stringID); string ++)
    {
        if(string->text)
        {
            graphics->drawString(location + Common::Point(string->x, string->y), string->text, string->flags, _area.width());
        }
        else
        {
            _stringBuffer[0] = 0;
            produceString(string->stringID);
            graphics->drawString(location + Common::Point(string->x, string->y), _stringBuffer, string->flags, _area.width());
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
    XEEN_VALID();

    Common::Point target = point - Common::Point(_area.left, _area.top);

    // Check buttons
    for(const Button* button = getButtons(); button && (button->normalFrame || button->actionID); button ++)
    {
        const Common::Rect r = button->area;
    
        if(button->actionID != Button::NOACTION && r.contains(target))
        {
            _pressedButton = button;
            _pressedTime = g_system->getMillis() + BUTTON_DELAY;
            return true;
        }
    }
    
    if(_clickToFinish)
    {
        _finished = true;
        return true;
    }
    
    return false;
}

bool XEEN::Window::key(Common::KeyCode code)
{
    XEEN_VALID();

    // Check buttons
    for(const Button* button = getButtons(); button && (button->normalFrame || button->actionID); button ++)
    {
        if(code == button->key)
        {
            handleAction(button->actionID);
            return true;
        }
    }
    
    if(_clickToFinish)
    {
        _finished = true;
        return true;
    }
    
    return false;
}

