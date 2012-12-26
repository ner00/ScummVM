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

#ifndef XEEN_ARCHIVE_FILE_H
#define XEEN_ARCHIVE_FILE_H

#include "xeen/utility.h"

namespace XEEN
{
    struct File : public Common::MemoryReadStream, public Validateable, public Common::NonCopyable
    {
        public:
            File(CCFileId id, byte* data, uint32 datasize) : Common::MemoryReadStream(data, datasize), _id(id), _size(datasize), _data(data) { }
            ~File() { delete[] _data; }
            
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
    };

    typedef Common::SharedPtr<File> FilePtr;
}

#endif // XEEN_ARCHIVE_FILE_H
