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

namespace XEEN
{
    class ImageBuffer;
    class CCFile;
    
    struct Button
    {
        const char* const sprite;
        const uint32 normalFrame;
        const uint32 pressedFrame;

        const int32 x;
        const int32 y;
        
        const uint32 actionID;
    };
    
    struct String
    {
        const char* const text;

        const int32 x;
        const int32 y;        
    };

    class Window
    {
        protected:
            Window(const Common::Rect& area);
            virtual ~Window() {};

            virtual const Button* getButtons() const { return 0; }
            virtual const String* getStrings() const { return 0; }

        public:
            void draw(ImageBuffer& out, CCFile& assets);
        
        private:
            Common::Rect _area;
    };
    
    class CharacterStatusWindow : public Window
    {
        public:
            CharacterStatusWindow();
            
        protected:
            const Button* getButtons() const;
            const String* getStrings() const;
    };
}

#endif // XEEN_WINDOW_H
