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

#ifndef XEEN_GRAPHICS_SPRITE_H
#define XEEN_GRAPHICS_SPRITE_H

#include "xeen/utility.h"

namespace XEEN
{
    class Game;
    class SpriteManager;
    class ImageBuffer;

    class Sprite : public Validateable_Cleanable
    {
        friend class SpriteManager;
    
        private:
            Sprite(FilePtr file);
            ~Sprite();
            
        public:
            void drawCell(ImageBuffer& out, const Common::Point& pen, uint16 frame, bool flip = false);

        protected:
            void cleanse();
            
        private:
            void drawFrame(ImageBuffer& out, const Common::Point& pen, bool flip);
            uint32 drawLine(ImageBuffer& out);
            
        private:
            FilePtr _file;

            struct Cell
            {
                uint16 offset[2];
            };
            
            uint32 _cellCount;
            Cell* _cells;
    };
}

#endif // XEEN_SPRITE_H
