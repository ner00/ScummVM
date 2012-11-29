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

#ifndef XEEN_SPRITE_H
#define XEEN_SPRITE_H

#include "common/scummsys.h"
#include "common/file.h"
#include "common/hashmap.h"
#include "common/memstream.h"

#include "xeen/utility.h"

namespace XEEN
{
    class CCFile;
    class CCFileData;
    class SpriteManager;

    class Sprite
    {
        friend class SpriteManager;
    
        private:
            Sprite(CCFileData* file);
            ~Sprite();
            
            bool initialize();
            void cleanse();
            
        public:
            void drawCell(ImageBuffer& out, const Common::Point& pen, uint16 frame, bool flip = false);
            
        private:
            void drawFrame(ImageBuffer& out, const Common::Point& pen, bool flip);
            uint32 drawLine(ImageBuffer& out);
            
        private:
            CCFileData* _file;

            struct Cell
            {
                uint16 offset[2];
            };
            
            uint32 _cellCount;
            Cell* _cells;
    };
    
    class SpriteManager
    {
        static const unsigned MAX_SPRITES = 65536;
    
        public:
            SpriteManager(CCFile& parent);
            ~SpriteManager();
            
            Sprite* getSprite(CCFileId id);
            
        private:
            CCFile& _cc;
            Sprite* _sprites[MAX_SPRITES]; // TODO: <Use a hash table!
    };
}

#endif // XEEN_SPRITE_H
