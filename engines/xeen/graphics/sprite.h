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
        struct Cell;    

        private:
            Sprite(FilePtr file);
            ~Sprite();
            
        public:
            void drawCell(ImageBuffer& out, const Common::Point& pen, uint16 frame, bool flip = false, uint32 scale = 0);

        protected:
            void cleanse();
            
        private:
            void cacheCell(Cell& cell, uint16 offset1, uint16 offset2);
            void cacheFrame(Cell& out, uint16 offset);
            uint32 cacheLine(Cell& out, uint16 line, uint16 offX);
            
        private:
            FilePtr _file;

            struct Cell
            {
                Cell() : width(0), height(0), pixels(0) {}
                ~Cell() { delete[] pixels; }

                uint16 width;
                uint16 height;

                byte* pixels;
            };
            
            uint32 _cellCount;
            Cell* _cells;
    };
}

#endif // XEEN_SPRITE_H
