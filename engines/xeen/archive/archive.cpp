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

#include "xeen/archive/archive.h"

static const uint16 SAVEID[6] = {0x2A0C, 0x2A1C, 0x2A2C, 0x2A3C, 0x284C, 0x2A5C};
static const uint32 SAVEID_COUNT = 6;


XEEN::Archive::Archive(const char* name) : _saveData(0), _saveSize(0)
{
    if(_file.open(name))
    {
        _mainToc.read(_file);
        
        // Calculate save file size
        _saveSize = 0;
        for(uint32 i = 0; i != SAVEID_COUNT; i ++)
        {
            const Toc::Entry* entry = _mainToc.getEntry(SAVEID[i]);
            _saveSize += entry ? entry->size : 0;
        }
        
        // Allocate and read save file
        _saveData = new byte[_saveSize];
        uint32 offset = 0;
        
        for(uint32 i = 0; i != SAVEID_COUNT; i ++)
        {
            FilePtr file = getFile(SAVEID[i], false);
            if(file)
            {
                file->read(&_saveData[offset], file->size());
                offset += file->size();
            }
        }
        
        // Make Stream
        _save = new Common::MemoryReadStream(_saveData, _saveSize);
        _saveToc.read(*_save);
    }
    else
    {
        markInvalid();
    }
}

XEEN::Archive::~Archive()
{
    delete[] _saveData;
}

XEEN::FilePtr XEEN::Archive::getFile(CCFileId id, bool inSave)
{
    XEEN_VALID_RET(FilePtr((File*)0));

    Toc& usedToc = (inSave) ? _saveToc : _mainToc;
    Common::SeekableReadStream& usedStream = (inSave) ? (Common::SeekableReadStream&)(*_save) : (Common::SeekableReadStream&)_file;

    const Toc::Entry* entry = usedToc.getEntry(id);
    
    if(entry)
    {
        // Read bytes
        byte* data = new byte[entry->size];
        usedStream.seek(entry->offset);
        usedStream.read(data, entry->size);        
        
        // De-obfuscate
        if(!inSave)
        {
            for(uint32 i = 0; i != entry->size; i ++)
            {
                data[i] ^= 0x35;
            }
        }
    
        return FilePtr(new File(id, data, entry->size));
    }
    else
    {
        return FilePtr((File*)0);
    }
}
