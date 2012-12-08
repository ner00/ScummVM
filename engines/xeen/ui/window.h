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

#ifndef XEEN_WINDOW_H
#define XEEN_WINDOW_H

#include "common/scummsys.h"
#include "common/rect.h"
#include "common/keyboard.h"

#include "xeen/utility.h"

namespace XEEN
{
    class ImageBuffer;
    
    struct Button
    {
        static const uint32 NOACTION = 0xFFFFFFFF;
    
        CCFileId sprite;
        uint32 normalFrame;
        uint32 pressedFrame;

        XRect area;
        
        uint32 actionID;
    };
    
    struct String
    {
        const char* const text;
        uint32 stringID;

        int32 x;
        int32 y;

        uint32 flags;
    };

    class Window : public Validateable
    {
        static const unsigned BUTTON_DELAY = 100;
    
        protected:
            Window(const Common::Rect& area, bool clickToClose = false);
            virtual ~Window() {};

            virtual const Button* getButtons() const { return 0; }
            virtual const String* getStrings() const { return 0; }
            
            virtual void handleAction(unsigned id) { }
            virtual const char* produceString(unsigned id) { return 0; }

        public:
            virtual void show() { };        
        
            void heartbeat();
            void draw(ImageBuffer& out);
            bool click(const Common::Point& point);
        
        private:
            Common::Rect _area;
            bool _clickToClose;
            
            const Button* _pressedButton;
            uint32 _pressedTime;
    };
    
    class CharacterStatusWindow : public Window
    {
        public:
            CharacterStatusWindow();
            
        protected:
            const Button* getButtons() const;
            const String* getStrings() const;
            
            void handleAction(unsigned id);
            const char* produceString(unsigned id);
            
        private:
            char stringBuffer[128];
    };
}

#endif // XEEN_WINDOW_H
