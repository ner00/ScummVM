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

#include "xeen/ccfile.h"

#include "common/file.h"
#include "common/debug.h"

XEEN::CCTocReader::CCTocReader(Common::File& file) : _file(file), _key(0xAC)
{

}

template<int COUNT>
uint32 XEEN::CCTocReader::readValue()
{
    uint32 result = 0;

    for(int i = 0; i != COUNT; i ++)
    {
        uint8 nextbyte = _file.readByte();
        nextbyte = ((nextbyte << 2) | (nextbyte >> 6)) + _key;
        
        result |= nextbyte << (i * 8);
        
        _key += 0x67;
    }
    
    return result;    
}

XEEN::CCFile::CCFile(const char* name) : _entryCount(0), _entries(0), _obfuscated(false)
{
    Common::File f;

    if(f.open(name))
    {
        _entryCount = f.readUint16LE();
        _entries = new CCFileEntry[_entryCount];
 
        CCTocReader tocreader(f);
        
        for(int i = 0; i != _entryCount; i ++)
        {
            _entries[i].id = tocreader.readValue<2>();
            _entries[i].offset = tocreader.readValue<3>();
            _entries[i].size = tocreader.readValue<2>();
            _entries[i].padding = tocreader.readValue<1>();
        }
        
        // If the file is named INTRO.CC or contains a save game record it is obfuscated
        // (every byte past the TOC is XORed by 0x35.
        // TODO: Check
        _obfuscated = (_entries[0].id == 0x2A0C) || strcmp(name, "INTRO.CC");
    }
    else
    {
        debug("Failed to open %s", name);
    }
}

XEEN::CCFile::~CCFile()
{
    delete[] _entries;
}

const XEEN::CCFileEntry* XEEN::CCFile::getEntry(CCFileId id)
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
