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

#ifndef XEEN_MAZE_EVENTLIST_H
#define XEEN_MAZE_EVENTLIST_H

#ifndef XEEN_MAZE_SOURCE
# error "Private header included"
#endif

#include "xeen/utility.h"
#include "xeen/archive/file.h"

namespace XEEN
{
    namespace Maze
    {
        const uint32 MAX_MAP_WIDTH = 256;
        const uint32 MAX_MAP_HEIGHT = 256;
    
        class Map;
    
        // Only accessible by Map
        class EventList : public Validateable
        {
            friend class Map;
    
            private:
                EventList(Map* parent, FilePtr data);
                void runEventAt(uint8 x, uint8 y, Direction facing);
    
            private:
                uint8 runEventLine(int32 off);
    
            private:
                Map* _parent;
                FilePtr _data;
                int32 _eventOffset[MAX_MAP_WIDTH * MAX_MAP_HEIGHT];
        };
    }
}

#endif // XEEN_MAZE_EVENTLIST_H
