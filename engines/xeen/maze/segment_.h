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

#ifndef XEEN_MAZE_SOURCE
# error "Private header included"
#endif

#include "xeen/utility.h"
#include "xeen/archive/file.h"
#include "xeen/maze/objects_.h"

namespace XEEN
{
    namespace Maze
    {
        class Map;
        class Manager;
    
        // Only accessible by MapManager and Map
        class Segment : public Validateable
        {
            friend class Map;
            friend class Manager;

            private:
                static const uint32 INSIDE      = 0x08;
                static const uint32 AUTOEXECUTE = 0x10;
                static const uint32 WATER       = 0x40;
        
            private:
                Segment(Valid<Manager> parent, FilePtr data);
                void loadSurrounding();
    
                uint16 getWall(uint8 x, uint8 y) const;
                uint8 getCellFlags(uint8 x, uint8 y) const;
                uint8 lookupSurface(uint8 id) const;

                Segment* resolveSegment(Common::Point& position);
    
            protected:
                Valid<Manager> _parent;
                FilePtr _data;
    
                Segment* _surrMazes[4];
        };
    }
}

#endif // XEEN_MAZE_SEGMENT_H
