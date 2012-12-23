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

#include "xeen/archive/toc.h"

namespace
{
    template<int COUNT>
    uint32 readValue(Common::SeekableReadStream& data, byte& key)
    {
        uint32 result = 0;
    
        for(int i = 0; i != COUNT; i ++)
        {
            uint8 nextbyte = data.readByte();
            nextbyte = ((nextbyte << 2) | (nextbyte >> 6)) + key;
            
            result |= nextbyte << (i * 8);
            
            key += 0x67;
        }
        
        return result;    
    }
}

XEEN::Toc::Toc() : _entryCount(0), _entries(0)
{
    markInvalid("XEEN Toc object created.");
}

XEEN::Toc::~Toc()
{
    delete[] _entries;
}

const XEEN::Toc::Entry* XEEN::Toc::getEntry(CCFileId id) const
{
    for(int i = 0; i != _entryCount; i ++)
    {
        if(_entries[i].id == id)
        {
            return &_entries[i];
        }
    }
    
    return 0;
}

void XEEN::Toc::read(Common::SeekableReadStream& data)
{
    _entryCount = data.readUint16LE();
    _entries = new Entry[_entryCount];
    
    if(_entryCount && _entries)
    {
        byte key = 0xAC;
    
        for(int i = 0; i != _entryCount; i ++)
        {
            _entries[i].id = readValue<2>(data, key);
            _entries[i].offset = readValue<3>(data, key);
            _entries[i].size = readValue<2>(data, key);
            _entries[i].padding = readValue<1>(data, key);
        }
        
        markValid();
    }
    else
    {
        XEEN_DELETE_ARRAY(_entries);
        markInvalid("Could not read TOC information.");
    }   
}
