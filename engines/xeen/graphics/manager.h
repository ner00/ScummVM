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
#ifndef XEEN_GRAPHICS_MANAGER_H
#define XEEN_GRAPHICS_MANAGER_H

#include "xeen/utility.h"
#include "xeen/archive/file.h"

namespace XEEN
{
    class Game;

    namespace Graphics
    {
        class Sprite;
        class ImageBuffer;
        class Font;
        
        class Manager : public Validateable, public GameHolder, public Common::NonCopyable
        {
            static const unsigned MAX_SPRITES = 65536;
            friend class XEEN::Game;
        
            private:
                Manager(Valid<Game> parent);
                ~Manager();
                
            public:
                void reset();
                void setClipArea(const Common::Rect& area);

                void fillRect(Common::Rect area, uint8 color);

                void draw(const CCSpriteId& id, const Common::Point& pen);
                void drawString(Common::Point pen, const char* text, uint32 flags = 0, uint32 width = 0);

                const byte* getScreenBitmap() const;
    
            private:
                ImageBuffer* _screen;
                Font* _font;
                Sprite* _sprites[MAX_SPRITES]; // TODO: <Use a hash table!
        };
    }   
}

#endif // XEEN_GRAPHICS_MANAGER_H
