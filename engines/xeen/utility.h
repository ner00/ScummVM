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
#include "common/keyboard.h"
#include "common/system.h"
#include "common/stack.h"

namespace XEEN
{
    // Inline error checking
    inline void die()
    {
        assert(false);
    }

    // Like assert but returns true (and safe for side effects)
    #define enforce(c) enforce2(c, __FILE__, __LINE__)
    inline bool enforce2(bool cond, const char* file, uint32 line)
    {
        if(!cond)
        {
            debug("Enforce fail: %s %d", file, line);
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
            void markInvalid(const char* format, ...)
            {
                char buffer[256];
                va_list args;

                va_start(args, format);
                vsnprintf(buffer, 256, format, args);
                va_end(args);

                debug("%.256s", buffer);

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
            void markInvalidAndClean(const char* format, ...)
            {
                char buffer[256];
                va_list args;

                va_start(args, format);
                vsnprintf(buffer, 256, format, args);
                va_end(args);

                markInvalid(buffer);
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
    template <typename T, class C>
    class Contract
    {
        public:
            Contract(T _value) : value(contract(_value)) { }
            operator T&() { return contract(value); }
//            T& operator.() { return contract(value); }

            operator const T&() const { return contract(value); }
//            const T& operator->() const { return contract(value); }

        private:
            C contract;
            T value;
    };

    template <typename T, class C>
    class Contract <T*, C>
    {
        public:
            Contract(T* _value) : value(contract(_value)) { }
            operator T*() { return contract(value); }
            T* operator->() { return contract(value); }

            operator const T*() const { return contract(value); }
            const T* operator->() const { return contract(value); }

        private:
            C contract;
            T* value;
    };

    template<typename T>
    class NonNull_Contract
    {
        public:
            T* operator()(T* v) { enforce(v); return v; }
            const T* operator()(T* v) const { enforce(v); return v; }
    };

    template<typename T>
    class Valid_Contract
    {
        public:
            T* operator()(T* v) { enforce(valid(v)); return v; }
            const T* operator()(T* v) const { enforce(valid(v)); return v; }
    };

    template<typename T, uint32 U>
    class LessThan_Contract
    {
        public:
            T& operator()(T& v) { enforce(v < U); return v; }
            const T& operator()(const T& v) const { enforce(v < U); return v; }
    };

    // Contract indicating a pointer that may not be null.
    template <typename T>
    class NonNull : public Contract<T*, NonNull_Contract<T> > { public: NonNull(T* v) : Contract<T*, NonNull_Contract<T> >(v) { } };

    template <typename T>
    class Valid : public Contract<T*, Valid_Contract<T> > { public: Valid(T* v) : Contract<T*, Valid_Contract<T> >(v) { }};

    template <typename T, uint32 U>
    class LessThan : public Contract<T, LessThan_Contract<T, U> > { public: LessThan(const T& v) : Contract<T, LessThan_Contract<T, U> >(v) { }};

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

    class Game;
    class GameHolder
    {
        public:
            GameHolder(Valid<Game> game) : _heldGame(game) { }
            Valid<Game> getGame() { return _heldGame; }
            const Valid<Game> getGame() const { return _heldGame; }
        private:
            Valid<Game> _heldGame;
    };

    struct Direction
    {
        private:
            uint32 _direction;

        public:
            static const uint32 NORTH = 0;
            static const uint32 EAST = 1;
            static const uint32 SOUTH = 2;
            static const uint32 WEST = 3;

        public:
            Direction() : _direction(0) {}
            Direction(uint32 d) : _direction(d & 3) {}

            operator uint32() const { return _direction; }

            static Direction fromOffsets(int32 x, int32 y)
            {
                if(x && y) return 0;
                if(x && !y) return (x < 0) ? 3 : 1;
                if(!x && y) return (y < 0) ? 2 : 0;
            }

            // The number of times this would need to turn right to equal other.
            Direction relativeTo(Direction other) const { return _direction + other; }

            Common::Point move(const Common::Point pos, int32 x, int32 y) const
            {
                Common::Point out = pos;

                if(!isAlongX())
                {
                    out.x += (_direction & 2) ? 0 - x : x;
                    out.y += (_direction & 2) ? 0 - y : y;
                }
                else
                {
                    out.x += (_direction & 2) ? 0 - y : y;
                    out.y += (_direction & 2) ? x : 0 - x;
                }

                return out;
            }

            bool isAlongX() const   { return _direction & 1; }

            // These both modify and return the object. They are not 'const'
            Direction& turnLeft()         { _direction = (_direction - 1) & 3; return *this; }
            Direction& turnRight()        { _direction = (_direction + 1) & 3; return *this; }
            Direction& turnAround()       { _direction ^= 2; return *this; }
            Direction& turn(bool left)    { _direction = (_direction + (left ? -1 : 1)) & 3; return *this; }
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

    struct CCSpriteId : public CCFileId
    {
        CCSpriteId(CCFileId id, uint32 frame = 0, uint32 count = 1, bool flip = false, uint16 scale = 0) : CCFileId(id), _frame(frame), _count(count), _flip(flip), _scale(scale) { }
        CCSpriteId(uint16 id = 0) : CCFileId(id), _frame(0), _count(1), _flip(false), _scale(0) { }

        uint32 _frame;
        uint32 _count;
        bool _flip;
        uint16 _scale;
    };

    struct File : public Common::MemoryReadStream, public Validateable, public Common::NonCopyable
    {
        public:
            File(CCFileId id, byte* data, uint32 datasize, bool wantDelete) : Common::MemoryReadStream(data, datasize), _id(id), _size(datasize), _data(data), _wantDelete(wantDelete) { }
            ~File() { if(_wantDelete) delete[] _data; }
            
            uint32 getSize()                        { return _size; }
            byte* getData()                         { return _data; }
    
            const CCFileId& getID( )                { return _id; }
        
            template<typename T>
            T* getPtrAt(uint32 loc)                 { return (enforce(loc < _size)) ? (T*)&_data[loc] : 0; }

            byte getByteAt(uint16 loc) const        { return (enforce(loc < _size)) ? _data[loc] : 0; }
            void setByteAt(uint16 loc, byte val)    { if(enforce(loc < _size)) _data[loc] = val; }
            int8 getI8At(uint16 loc) const          { return (enforce(loc < _size)) ? *(int8*)&_data[loc] : 0; }
            byte* getBytePtrAt(uint16 loc) const    { return (enforce(loc < _size)) ? &_data[loc] : 0; }
            uint16 getU16At(uint16 loc) const       { return getByteAt(loc) | (getByteAt(loc + 1) << 8); }
            void setU16At(uint16 loc, uint16 val)   { setByteAt(loc, val & 0xFF); setByteAt(loc + 1, (val >> 8) & 0xFF); }
            uint32 getU32At(uint16 loc) const       { return getByteAt(loc) | (getByteAt(loc + 1) << 8) | (getByteAt(loc + 2) << 16) | (getByteAt(loc + 3) << 24); }
            void setU32At(uint16 loc, uint32 val)   { setByteAt(loc, val & 0xFF); setByteAt(loc + 1, (val >> 8) & 0xFF); setByteAt(loc + 2, (val >> 16) & 0xFF); setByteAt(loc + 3, (val >> 24) & 0xFF); }
    
        private:
            CCFileId _id;
    
            uint32 _size;
            byte* _data;
            bool _wantDelete;
    };

    typedef Common::SharedPtr<File> FilePtr;

    class ValueManager
    {
        public:
            struct ValueInfo { uint32 offset; uint32 size; bool isSigned; };

        public:
            ValueManager(FilePtr file, NonNull<const ValueInfo> values, uint32 valueCount, uint32 valueBase) : 
                _file(file), _values(values), _valueCount(valueCount), _valueBase(valueBase) { }

            template <typename T>
            T getValue(uint32 index) const
            {
                if(enforce(index < _valueCount))
                {
                    if(_values[index].size == 4)
                    {
                        return _file->getU32At(_valueBase + _values[index].offset);
                    }
                    else if(_values[index].size == 2)
                    {
                        return _file->getU16At(_valueBase + _values[index].offset);
                    }
                    else
                    {
                        return _file->getByteAt(_valueBase + _values[index].offset);
                    }
                }
                
                return 0;
            }

            template <typename T>
            void setValue(uint32 index, T data)
            {
                if(enforce(index < _valueCount))
                {
                    if(_values[index].size == 4)
                    {
                        _file->setU32At(_valueBase + _values[index].offset, data);
                    }
                    else if(_values[index].size == 2)
                    {
                        _file->setU16At(_valueBase + _values[index].offset, data);
                    }
                    else
                    {
                        _file->setByteAt(_valueBase + _values[index].offset, data);
                    }
                }
            }

            template <typename T, typename F>
            void modifyValue(uint32 index, T data)
            {
                F modify;
                setValue<T>(index, modify(getValue<T>(index), data));
            }


        private:
            FilePtr _file;
            const ValueInfo* const _values;
            const uint32 _valueCount;
            const uint32 _valueBase;
    };

}

#endif // XEEN_CCFILE_H
