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
#include "common/ptr.h"
#include "common/file.h"
#include "common/array.h"
#include "common/stream.h"
#include "common/memstream.h"

namespace XEEN
{
    // Inline error checking
    inline void die()
    {
        assert(false);
    }

    // Like assert but returns true (and safe for side effects)
    inline bool enforce(bool cond)
    {
        if(!cond)
        {
            die();
        }

        return true;
    }

    // Base classes for object validation. It is illegal to call any method on an invalid
    // object.
    class Validateable
    {
        public:
            Validateable() : _valid(true) { }
        
            bool isValid() const { return _valid; }
            operator const void*() const { return _valid ? this : 0; }
    
        protected:
            void markValid() { _valid = true; }
            void markInvalid(const char* msg = 0)
            {
                if(msg)
                {
                    debug("%s", msg);
                }

                _valid = false;
            }

        private:
            bool _valid;
    };
    
    class Validateable_Cleanable : public Validateable
    {
        public:
            virtual ~Validateable_Cleanable() { };
    
        public:
            void markInvalidAndClean(const char* msg = 0)
            {
                markInvalid(msg);
                cleanse();
            }
    
        protected:
            virtual void cleanse() = 0;
    };

    // Return true is specified object is non-null and isValid returns true.
    inline bool valid(const Validateable& a)
    {
        return a.isValid();
    }
    
    inline bool valid(const Validateable* a)
    {
        return a && a->isValid();
    }

    template <typename T>
    inline bool valid(const Common::SharedPtr<T>& a)
    {
        return a && a->isValid();
    }

    // Aborts is the current object is not valid
    #define XEEN_VALID() enforce(valid(this))
    
    #define XEEN_DELETE(T) delete T; T = 0
    #define XEEN_DELETE_ARRAY(T) delete[] T; T = 0
    
    // Type attributes
    // Contract indicating a pointer that may not be null.
    template <typename T>
    class NonNull
    {
        public:
            NonNull(T* _value) : value(check(_value)) { }
            operator T*() { return check(value); }
            T* operator->() { return check(value); }

            T* check(T* _value) const { enforce(_value); return _value; }

        private:
            T* value;
    };

    // Contract indicating a pointer that must be valid.
    template <typename T>
    class Valid
    {
        public:
            Valid(T* _value) : value(check(_value)) { }
            operator T*() { return check(value); }
            T* operator->() { return check(value); }

            T* check(T* _value) const { enforce(valid(_value)); return _value; }

        private:
            T* value;
    };

    // POD rect wrapper
    struct XRect
    {
        int16 x;
        int16 y;
        uint16 w;
        uint16 h;

        // Helper to build rect with xywh
        static Common::Rect cr(int16 x, int16 y, uint16 w, uint16 h)
        {
            return Common::Rect(x, y, x + w, y + h);
        }

        operator Common::Rect() const
        {
            return Common::Rect(x, y, x + w, y + h);
        }
        
        operator Common::Point() const
        {
            return Common::Point(x, y);
        }
    };

    //
    struct CCFileId
    {
        public:        
            CCFileId() : _id(0)
            {
                memset(_name, 0, sizeof(_name));
            }
        
            CCFileId(uint16 id) : _id(id)
            {
                memset(_name, 0, sizeof(_name));
            }
                        
            CCFileId(const char* format, ...) : _id(0)
            {                
                va_list args;
                va_start(args, format);
                vsnprintf(_name, 16, format, args);
                va_end(args);
                
                _name[15] = 0;
                
                _id = fromString(_name);
            }
            
            operator uint16() const { return _id; }
            operator const char*() const { return _name; }
        
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
            char _name[16];
    };

    struct CCFileData;
    typedef Common::SharedPtr<CCFileData> CCFilePtr;
}

#endif // XEEN_CCFILE_H
