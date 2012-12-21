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

#ifndef XEEN_ARCHIVE_ARCHIVE_H
#define XEEN_ARCHIVE_ARCHIVE_H

#include "xeen/utility.h"

#include "xeen/archive/file.h"
#include "xeen/archive/toc.h"

namespace XEEN
{
    // Only accessible from Game
    class Game;

    class Archive : public Validateable, public Common::NonCopyable
    {
        friend class Game;

        private:
            Archive(const char* name);
            virtual ~Archive();
            
            FilePtr getFile(CCFileId id, bool inSave);
                        
        private:
            Toc _mainToc;
            Common::File _file;

            Toc _saveToc;
            Common::MemoryReadStream* _save;
            byte* _saveData;
            uint32 _saveSize;
    };
}

#endif // XEEN_ARCHIVE_ARCHIVE_H
