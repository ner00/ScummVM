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

#ifndef XEEN_MAZE_MAPMANAGER_H
#define XEEN_MAZE_MAPMANAGER_H

#include "xeen/utility.h"

namespace XEEN
{
    class MapManager : public Validateable
    {
        friend class Game;
    
        private:
            MapManager();
            ~MapManager();
            
        public:
            Map* getMap(uint16 id);
            Segment* getSegment(uint16 id);
            
        private:
            Map* _maps[256]; // TODO: <Use a hash table!
            Segment* _segments[256];
    };
}

#endif // XEEN_MAZE_MAPMANAGER_H
