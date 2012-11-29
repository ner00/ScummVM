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

#ifndef XEEN_UTILITY_H
#define XEEN_UTILITY_H

#include <stdarg.h>
#include "common/scummsys.h"
#include "common/rect.h"

namespace XEEN
{
    struct CCFileId
    {
        public:        
            CCFileId(uint16 id) : _id(id)
            {
            }
                        
            CCFileId(const char* format, ...) : _id(0)
            {
                char buf[256];
                
                va_list args;
                va_start(args, format);
                vsnprintf(buf, 256, format, args);
                va_end(args);
                
                buf[255] = 0;
                
                _id = fromString(buf);
            }
            
            operator uint16() const { return _id; }
        
        public:
            static uint16 fromString(const char* name)
            {
                uint16 result = 0;
            
                for(; *name; name ++)
                {
                    result = (result & 0x7F) << 9 | (result & 0xFF80) >> 7;            
                    result += *name;
                }
                
                return result;
            }
        
            uint16 _id;
    };
    
    struct ImageBuffer
    {
        public:
            ImageBuffer() : _pen(0, 0), _penOffset(1, 0)
            {
            
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
            
            void readPixels(Common::ReadStream& input, uint32 length)
            {
                for(uint32 i = 0; i != length; i ++)
                {
                    putPixel(input.readByte());
                }
            }
            
            void putPixel(uint8 color)
            {
                if(_pen.x >= 0 && _pen.x < 320 && _pen.y >= 0 && _pen.y < 200)
                {
                    buffer[_pen.y * 320 + _pen.x] = color;
                }
                
                _pen += _penOffset;
            }
            
        public:    
            byte buffer[320 * 200];
            
        private:
            Common::Point _pen;
            Common::Point _penOffset;
    };

    inline bool enforce(bool cond)
    {
        assert(cond);
        return cond;
    }
    
    template <typename T>
    void DELETE(T*& v)
    {
        delete v;
        v = 0;
    }
    
    template <typename T>
    void DELETE_ARRAY(T*& v)
    {
        delete[] v;
        v = 0;
    }
}

#endif // XEEN_CCFILE_H
