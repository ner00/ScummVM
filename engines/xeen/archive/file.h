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
#include "common/memstream.h"
#include "common/file.h"

#include "xeen/utility.h"
#include "xeen/archive/toc.h"

namespace XEEN
{
    class File : public Common::MemoryReadStream, public Validateable
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
}

#endif // XEEN_CCFILE_H
