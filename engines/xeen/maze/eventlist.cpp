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


#include "xeen/game.h"
#include "xeen/utility.h"

#include "xeen/maze/eventlist.h"

///
/// EventList
///
XEEN::EventList::EventList(uint16 mapNumber)
{
    FilePtr reader(XEENgame.getFile(CCFileId("MAZE%s%03d.EVT", (mapNumber < 100) ? "0" : "X", mapNumber), true));
    
    if(reader)
    {
        while(!reader->eos())
        {
            Line line;
            line.length = reader->readByte();
            
            if(line.length == 0)
            {
                break;
            }
            
            // Sanity
            if(line.length < 5)
            {
                markInvalidAndClean("EventList: Line too short.");
                return;
            }

            if(line.length > sizeof(line.arguments) + 5)
            {
                markInvalidAndClean("EventList: Line too long.");
                return;
            }
           
            // Read line data
            line.x = reader->readByte();
            line.y = reader->readByte();
            line.facing = reader->readByte();
            line.lineNumber = reader->readByte();
            line.opcode = reader->readByte();
            reader->read(line.arguments, line.length - 5);
                        
            // Start a new event
            if(line.lineNumber == 0)
            {
                Event event;
                event.x = line.x;
                event.y = line.y;
                event.lines.push_back(line);
                _events.push_back(event);
            }
            // Append an old one
            else
            {
                if(line.lineNumber != _events.back().lines.back().lineNumber + 1)
                {
                    markInvalidAndClean("EventList: Line misnumbered.");
                    return;
                }
            
                _events.back().lines.push_back(line);
            }
        }
    }
    else
    {
        markInvalid();
    }
}

void XEEN::EventList::cleanse()
{
    _events.clear();
}
