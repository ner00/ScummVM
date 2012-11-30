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
}

#endif // XEEN_CCFILE_H
