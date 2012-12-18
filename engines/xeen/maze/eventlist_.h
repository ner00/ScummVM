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
#include "xeen/ui/window.h"

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
                void pumpEvent();

            private:
                int32 runEventLine(int32 off);
                int32 evNOP(uint32 offset);
                int32 evMAPTEXT(uint32 offset);
                int32 evMESSAGE(uint32 offset);
                int32 evNPC(uint32 offset);
                int32 evTELEPORT(uint32 offset);
                int32 evIF(uint32 offset);
    
                uint32 produceValue(uint32 id);

            private:
                struct Event
                {
                    Common::Array<uint32> lines;
                };

            private:
                Map* _parent;
                FilePtr _data;

                Event* _runningEvent;
                uint32 _runningLine;
                Window* _waitingWindow;

                Common::HashMap<uint32, Event> _events;
        };
    }
}

#endif // XEEN_MAZE_EVENTLIST_H
