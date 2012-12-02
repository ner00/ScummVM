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

#include "xeen/utility.h"

#include "common/scummsys.h"
#include "common/array.h"

namespace XEEN
{
    class EventList : public Validateable_Cleanable
    {
        public:        
            struct Line
            {
                uint8 length;
                uint8 x;
                uint8 y;
                uint8 facing;
                uint8 lineNumber;
                uint8 opcode;
                uint8 arguments[32];
            };
            
            struct Event
            {
                uint8 x;
                uint8 y;
                
                Common::Array<Line> lines;
            };

        public:
            EventList(uint16 mapNumber);
            
        protected:
            void cleanse();

        private:
            Common::Array<Event> _events;
    };
}

#endif // XEEN_MAZE_EVENTLIST_H
