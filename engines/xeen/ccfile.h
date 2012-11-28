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

#ifndef XEEN_CCFILE_H
#define XEEN_CCFILE_H

#include "common/scummsys.h"
#include "common/file.h"
#include "common/hashmap.h"
#include "common/memstream.h"

namespace XEEN
{
    struct CCFileId
    {
        public:        
            CCFileId(uint16 id) : _id(id)
            {
            }
            
            CCFileId(const char* name) : _id(0)
            {
                for(; *name; name ++)
                {
                    _id = (_id & 0x7F) << 9 | (_id & 0xFF80) >> 7;            
                    _id += *name;
                }
            }
            
            operator uint16() { return _id; }
        
        private:
            uint16 _id;
    };

    struct CCFileEntry
    {
        uint16 id;
        uint32 offset;
        uint16 size;
        uint8 padding;
    };

    struct CCFileData
    {
        CCFileData();
    
        uint16 id;
        uint32 openCount;

        uint32 size;
        byte* data;
    };
    
    class CCTocReader
    {
        public:
            CCTocReader(Common::File& file);
            
            template<int COUNT>
            uint32 readValue();
            
        private:
            Common::File& _file;
            uint8 _key;
            
    };

    class CCFile
    {
        public:
            CCFile(const char* name);
            ~CCFile();
            
            const CCFileEntry* getEntry(CCFileId id);
            Common::MemoryReadStream getFile(CCFileId id);
            
        private:
            Common::File _file;
        
            uint16 _entryCount;
            CCFileEntry* _entries;
            
            bool _obfuscated;
            
            Common::HashMap<uint16, CCFileData> _openFiles;
    };
}

#endif // XEEN_CCFILE_H
