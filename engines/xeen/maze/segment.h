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

#ifndef XEEN_MAZE_SEGMENT_H
#define XEEN_MAZE_SEGMENT_H

#include "xeen/utility.h"
#include "xeen/archive/file.h"

namespace XEEN
{
    class MazeObjects;

    // Only accessible by MapManager and Map
    class Segment : public Validateable
    {
        friend class MapManager;
        friend class Map;
    
        private:
            Segment(uint16 mapNumber);
            virtual ~Segment() { }

            uint16 getWall(uint8 x, uint8 y) const;
            uint8 getCellFlags(uint8 x, uint8 y) const;
            uint8 lookupSurface(uint8 id) const;

            Segment* getNorth() { return _north; }
            Segment* getEast() { return _east; }

        protected:
            FilePtr _data;

            Segment* _north;
            Segment* _east;
            
            MazeObjects* _objects;
    };
}

#endif // XEEN_MAZE_SEGMENT_H
