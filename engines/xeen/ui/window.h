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

#ifndef XEEN_UI_WINDOW_H
#define XEEN_UI_WINDOW_H

#include "xeen/utility.h"

namespace XEEN
{
    class Game;

    struct Button
    {
        static const uint32 NOACTION = 0xFFFFFFFF;
    
        CCFileId sprite;
        uint32 normalFrame;
        uint32 pressedFrame;

        XRect area;
        
        uint32 actionID;
        Common::KeyCode key;
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
    
        //HACK
        public:
            struct Font
            {
                static const uint32 SMALL = 1;
                static const uint32 CENTER = 2;
                static const uint32 ALIGN_RIGHT = 4;
            };


        protected:
            Window(Valid<Game> parent, const Common::Rect& area, bool clickToFinish = false);

        public:
            virtual ~Window() {};

        protected:
            virtual const Button* getButtons() const { return 0; }
            virtual const String* getStrings() const { return 0; }
            
            virtual void handleAction(unsigned id) { }
            virtual void produceString(unsigned id) { }

            void fillStringBuffer(const char* fmt, ...);

        public:
            virtual void show() { };        
        
            const bool isFinished() { return _finished; }
            void heartbeat();
            void draw();
            bool click(const Common::Point& point);
            bool key(Common::KeyCode code);

        protected:
            Valid<Game> _parent;
            bool _finished;
            bool _clickToFinish;

        private:
            char _stringBuffer[128];

            Common::Rect _area;
            
            const Button* _pressedButton;
            uint32 _pressedTime;
    };
}

#endif // XEEN_UI_WINDOW_H
