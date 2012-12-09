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

#ifndef XEEN_MAZETEXT_H
#define XEEN_MAZETEXT_H

#include "common/scummsys.h"

namespace XEEN
{
    class MazeText : public Validateable
    {
        static const unsigned MAX_STRINGS = 256;
    
        public:
            MazeText(uint32 mapNumber);
            
            const char* getString(uint32 id) const;
            
        private:
            FilePtr _data;
            uint32 _stringOffsets[MAX_STRINGS];
    };
}

#endif // XEEN_MAZETEXT_H
