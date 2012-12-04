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

#ifndef XEEN_MAZEOBJECTS_H
#define XEEN_MAZEOBJECTS_H

#include "xeen/utility.h"

#include "common/scummsys.h"

namespace XEEN
{
    class CCFileData;

    class MazeObjects : public Validateable_Cleanable
    {
        public:
            struct Entry
            {
                Common::Point position;
                uint8 id;
                uint8 facing;
            };    
    
        public:
            MazeObjects(uint16 mapNumber);
            ~MazeObjects();
            
            bool getObjectAt(const Common::Point& position, Entry& data);
            
        protected:
            void cleanse();
            
        private:
            CCFileData* _data;
        
            uint16 _offsets[3];
            uint16 _counts[3];
    };
}

#endif // XEEN_MAZEOBJECTS_H
