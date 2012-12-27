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

        class EventState;
    
        // Only accessible by Map
        class EventList : public Validateable, public Common::NonCopyable
        {
            friend class Map;
            friend class EventState;
    
            private:
                EventList(Valid<Map> parent, FilePtr data);
                void runEventAt(const Common::Point& pos, Direction facing, uint32 line = 0);

            private:
                bool runEventLine(const EventState& state, int32 offset);
                bool evMAPTEXT(const EventState& state, int32 offset);
                bool evMESSAGE(const EventState& state, int32 offset);
                bool evNPC(const EventState& state, int32 offset);
                bool evTELEPORT(const EventState& state, int32 offset);
                bool evIF(const EventState& state, int32 offset);
                bool evMOVEOBJ(const EventState& state);
                bool evSPAWN(const EventState& state);
                bool evSETCELLFLAGS(const EventState& state);
                bool evIFMAPFLAG(const EventState& state);
                bool evGIVETAKE(const EventState& state);
                bool evREMOVE(const EventState& state);
                bool evALTEREVENT(const EventState& state);
                bool evWHOWILL(const EventState& state);
                bool evCALL(const EventState& state);
                bool evALTERMAP(const EventState& state);

            private:
                struct Event
                {
                    Common::Array<uint32> lines;
                };

            private:
                Valid<Map> _parent;
                FilePtr _data;

                Common::HashMap<uint32, Event> _events;
        };

        struct EventState
        {
            Valid<EventList> parent;
            Common::Point pos;
            Direction facing;
            uint32 line;
            uint32 offset;

            EventState(Valid<EventList> aparent, const Common::Point& apos, Direction afacing, uint32 aline, uint32 aoffset) :
                parent(aparent), pos(apos), facing(afacing), line(aline), offset(aoffset)
            {
                
            }

            uint8  getByteAt(uint32 off) const { return parent->_data->getByteAt(offset + off); }
            uint16 getU16At(uint32 off) const { return parent->_data->getU16At(offset + off); }
            uint32 getU32At(uint32 off) const { return parent->_data->getU32At(offset + off); }

            void runFrom(uint32 fromLine)
            {
                parent->runEventAt(pos, facing, fromLine);
            }
        };
    }
}

#endif // XEEN_MAZE_EVENTLIST_H
