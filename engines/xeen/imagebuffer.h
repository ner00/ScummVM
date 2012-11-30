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

#ifndef XEEN_IMAGEBUFFER_H
#define XEEN_IMAGEBUFFER_H

#include "common/scummsys.h"
#include "common/rect.h"
#include "common/stream.h"

#include "xeen/utility.h"

namespace XEEN
{    
    struct ImageBuffer
    {
        public:
            ImageBuffer() : _origin(0, 0), _pen(0, 0), _penOffset(1, 0), _clip(0, 0, 320, 200), _scale(0)
            {
            
            }
        
            ImageBuffer& setOrigin(const Common::Point& origin)
            {
                _origin = origin;
                return *this;
            }
        
            Common::Point getOrigin() const
            {
                return _origin;
            }
        
            ImageBuffer& setPen(const Common::Point& pen)
            {
                _pen = pen;
                return *this;
            }
            
            ImageBuffer& movePen(const Common::Point& pen)
            {
                _pen.x = _pen.x + (_penOffset.x * pen.x);
                _pen.y = _pen.y + (_penOffset.y * pen.y);
                return *this;
            }
            
            ImageBuffer& clear(uint8 color)
            {
                memset(buffer, color, sizeof(buffer));
                return *this;
            }
            
            ImageBuffer& setPenOffset(const Common::Point& offset)
            {
                _penOffset = offset;
                return *this;
            }
            
            ImageBuffer& setClipArea(const Common::Rect& clip)
            {
                _clip = clip;
                return *this;
            }
            
            ImageBuffer& resetClipArea()
            {
                _clip = Common::Rect(0, 0, 320, 200);
                return *this;
            }
            
            ImageBuffer& setScale(uint32 amount)
            {
                _scale = ((float)(amount >> 2)) + (.25f * (amount & 3));
                return *this;
            }
            
            // Draw Pixels KEYED UNSCALED NOFLIP
            template <byte KEY>
            void drawPixels_K_U_NF(const byte* pixels, unsigned length)
            {            
                for(unsigned i = 0; i != length; i ++, pixels++)
                {
                    const Common::Point pos = _origin + _pen;
                
                    if(*pixels != KEY && _clip.contains(pos))
                    {
                        buffer[pos.y * 320 + pos.x] = *pixels;
                    }
                    
                    _pen.x ++;
                }
            }
            
            void readPixels(Common::ReadStream& input, uint32 length)
            {
                for(uint32 i = 0; i != length; i ++)
                {
                    putPixel(input.readByte());
                }
            }
            
            void putPixel(uint8 color)
            {
                const int ppenX = (_scale > 0.1f) ? (int)(((float)_pen.x) / _scale) : _pen.x;
                const int ppenY = (_scale > 0.1f) ? (int)(((float)_pen.y) / _scale) : _pen.y;

                if(_clip.contains(_origin + Common::Point(ppenX, ppenY)))
                {
                    buffer[(_origin.y + ppenY) * 320 + (_origin.x + ppenX)] = color;
                }
                
                _pen += _penOffset;
            }
            
        public:    
            byte buffer[320 * 200];
            
        private:
            Common::Point _origin;
            Common::Point _pen;
            Common::Point _penOffset;
            
            Common::Rect _clip;
            float _scale;
    };
}

#endif // XEEN_IMAGEBUFFER_H
