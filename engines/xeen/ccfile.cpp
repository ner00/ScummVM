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

XEEN::CCFileData::CCFileData() : id(0), openCount(0), size(0), data(0)
{

}

XEEN::CCToc::CCToc() : _entryCount(0), _entries(0), _key(0xAC)
{

}

XEEN::CCToc::~CCToc()
{
    delete[] _entries;
}

const XEEN::CCFileEntry* XEEN::CCToc::getEntry(CCFileId id)
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

void XEEN::CCToc::readToc(Common::SeekableReadStream& data)
{
    _entryCount = data.readUint16LE();
    _entries = new CCFileEntry[_entryCount];
    
    for(int i = 0; i != _entryCount; i ++)
    {
        _entries[i].id = readValue<2>(data);
        _entries[i].offset = readValue<3>(data);
        _entries[i].size = readValue<2>(data);
        _entries[i].padding = readValue<1>(data);
    }
}

template<int COUNT>
uint32 XEEN::CCToc::readValue(Common::SeekableReadStream& data)
{
    uint32 result = 0;

    for(int i = 0; i != COUNT; i ++)
    {
        uint8 nextbyte = data.readByte();
        nextbyte = ((nextbyte << 2) | (nextbyte >> 6)) + _key;
        
        result |= nextbyte << (i * 8);
        
        _key += 0x67;
    }
    
    return result;    
}

XEEN::CCFile::CCFile(const char* name) : _saveGame(0)
{
    if(_file.open(name))
    {
        readToc(_file);
        
        _saveGame = new CCSaveFile(*this);
    }
    else
    {
        debug("Failed to open %s", name);
    }
}

XEEN::CCFile::~CCFile()
{
    delete _saveGame;

    // TODO: Close files
}

Common::MemoryReadStream XEEN::CCFile::getFile(CCFileId id)
{
    const XEEN::CCFileData* file = getFileRaw(id);
    
    if(!file)
    {
        static byte fakestream[8];
        return Common::MemoryReadStream(fakestream, 0);    
    }
    else
    {
        return Common::MemoryReadStream(file->data, file->size);
    }
}

const XEEN::CCFileData* XEEN::CCFile::getFileRaw(CCFileId id)
{
    XEEN::CCFileData& file = _openFiles[id];
    
    if(file.openCount == 0)
    {
        const CCFileEntry* entry = getEntry(id);
        
        if(entry == 0)
        {
            debug("File not found: %d", (short)id);
            return 0;
        }
        
        file.id = id;
        file.openCount = 1;
        file.size = entry->size;
        file.data = new byte[file.size];
        
        // Read bytes
        _file.seek(entry->offset);
        _file.read(file.data, file.size);

        // De-obfuscate        
        for(uint32 i = 0; i != file.size; i ++)
        {
            file.data[i] ^= 0x35;
        }
    }
    else
    {
        file.openCount ++;
    }
    
    return &file;
}

XEEN::CCSaveFile& XEEN::CCFile::getSaveFile()
{
    return *_saveGame;
}

XEEN::CCSaveFile::CCSaveFile(CCFile& base) : _data(0), _size(0), _file(0)
{
    static const uint16 saveIDs[6] = {0x2A0C, 0x2A1C, 0x2A2C, 0x2A3C, 0x284C, 0x2A5C};
    static const uint32 saveIDcount = 6;
    
    // Calculate size
    for(uint32 i = 0; i != saveIDcount; i ++)
    {
        const CCFileEntry* entry = base.getEntry(saveIDs[i]);
        _size += entry ? entry->size : 0;
    }
    
    // Allocate
    _data = new byte[_size];
    
    // Read
    uint32 offset = 0;
    
    for(uint32 i = 0; i != saveIDcount; i ++)
    {
        const CCFileData* file = base.getFileRaw(saveIDs[i]);
        
        if(file)
        {
            memcpy(&_data[offset], file->data, file->size);
            offset += file->size;
        }    
    }
    
    // Make Stream
    _file = new Common::MemoryReadStream(_data, _size);
    readToc(*_file);
}

XEEN::CCSaveFile::~CCSaveFile()
{
    delete[] _data;
    delete _file;
}

Common::MemoryReadStream XEEN::CCSaveFile::getFile(CCFileId id)
{
    const XEEN::CCFileData* file = getFileRaw(id);
    
    if(!file)
    {
        static byte fakestream[8];
        return Common::MemoryReadStream(fakestream, 0);    
    }
    else
    {
        return Common::MemoryReadStream(file->data, file->size);
    }
}

const XEEN::CCFileData* XEEN::CCSaveFile::getFileRaw(CCFileId id)
{
    XEEN::CCFileData& file = _openFiles[id];
    
    if(file.openCount == 0)
    {
        const CCFileEntry* entry = getEntry(id);
        
        if(entry == 0)
        {
            debug("File not found: %d", (short)id);
            return 0;
        }
        
        file.id = id;
        file.openCount = 1;
        file.size = entry->size;
        file.data = new byte[file.size];
        
        // Read bytes
        _file->seek(entry->offset);
        _file->read(file.data, file.size);

        // De-obfuscate        
        for(uint32 i = 0; i != file.size; i ++)
        {
            file.data[i] ^= 0x35;
        }
    }
    else
    {
        file.openCount ++;
    }
    
    return &file;
}

