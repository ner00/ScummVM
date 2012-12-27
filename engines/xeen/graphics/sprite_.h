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

#ifndef XEEN_GRAPHICS_SOURCE
# error "Private header included"
#endif

#include "xeen/utility.h"

namespace XEEN
{
    namespace Graphics
    {
        class Manager;
        class ImageBuffer;
    
        class Sprite : public Validateable, public Common::NonCopyable
        {
            friend class Manager;
        
            private:
                Sprite(FilePtr file);
                
            private:
                void drawCell(NonNull<ImageBuffer> out, const Common::Point& pen, uint16 frame, bool flip = false, uint32 scale = 0);
                void drawFrame(NonNull<ImageBuffer> out, uint32 offset, int32 x, int32 y, bool flip, uint32 scale);
                uint32 drawLine(NonNull<uint8> out);
                
            private:
                FilePtr _file;
        };
    }
}

#endif // XEEN_SPRITE_H
