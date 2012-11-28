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
#include "common/memstream.h"

#include "xeen/utility.h"
#include "xeen/sprite.h"
#include "xeen/map.h"

namespace XEEN
{
    struct CCFileEntry
    {
        uint16 id;
        uint32 offset;
        uint16 size;
        uint8 padding;
    };

    struct CCFileData : public Common::MemoryReadStream
    {
        public:
            CCFileData(CCFileId id, byte* data, uint32 size);
            ~CCFileData();    
            
            uint32 getSize() { return _size; }
            byte* getData() { return _data; }
    
        private:
            uint16 _id;
    
            uint32 _size;
            byte* _data;
    };
    
    class CCToc
    {
        public:
            CCToc();
            virtual ~CCToc();

            const CCFileEntry* getEntry(CCFileId id);

        protected:
            void readToc(Common::SeekableReadStream& data);
        
        private:
            template<int COUNT>
            uint32 readValue(Common::SeekableReadStream& data);

        protected:
            uint16 _entryCount;
            CCFileEntry* _entries;

        private:
            uint8 _key;
            
    };

    class CCSaveFile;

    class CCFile : public CCToc
    {
        public:
            CCFile(const char* name);
            virtual ~CCFile();
            
            CCFileData* getFile(CCFileId id);
            
            CCSaveFile& getSaveFile();
            
            SpriteManager& getSpriteManager() { return _spriteManager; }
            MapManager& getMapManager() {return _mapManager; }
            
        private:
            Common::File _file;            
            CCSaveFile* _saveGame;
            
            SpriteManager _spriteManager;
            MapManager _mapManager;
    };
    
    class CCSaveFile : public CCToc
    {
        public:
            CCSaveFile(CCFile& base);
            virtual ~CCSaveFile();
    
            CCFileData* getFile(CCFileId id);
    
        private:
            byte* _data;
            uint32 _size;
            
            Common::MemoryReadStream* _file;
    };
}

#endif // XEEN_CCFILE_H
