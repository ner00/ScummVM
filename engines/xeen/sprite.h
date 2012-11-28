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

#include "ccfile.h"

namespace XEEN
{
    class Sprite
    {
        public:
            Sprite(CCFile& cc, CCFileId id);
            ~Sprite();
            
            void drawCell(byte* out, uint16 index, uint16 xOffset, uint16 yOffset);
            
        private:
            void drawFrame(byte* out, Common::MemoryReadStream& reader, uint16 xOffset, uint16 yOffset);
            uint32 drawLine(byte* out, Common::MemoryReadStream& reader, uint16 xOffset);
            
        private:
            CCFile& _cc;
            CCFileId _id;
            const CCFileData* _file;

            struct Cell
            {
                uint16 first;
                uint16 second;
            };
            
            uint32 _cellCount;
            Cell* _cells;
    };
}

#endif // XEEN_SPRITE_H
