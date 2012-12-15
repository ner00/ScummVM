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

#ifndef XEEN_MAP_H
#define XEEN_MAP_H

#include "xeen/utility.h"

namespace XEEN
{
    namespace Graphics
    {
        class Manager;
    }

    namespace Maze
    {
        class Segment;
        class Text;
        class EventList;
        class Objects;
    
        struct ObjectEntry
        {
            uint8 id;
            uint8 facing;
        };    

        // Only constructible by MapManager
        class Map : public Validateable
        {
            friend class Manager;

            private:
                Map(uint16 mapNumber);
                ~Map();
    
            public:
                const char* getString(uint32 id) const;
                void runEventAt(uint8 x, uint8 y, uint32 facing);
    
                uint16 getTile(Common::Point position, uint32 direction = 0) const;
                uint8 getFlags(Common::Point position) const;
                uint16 getSurface(Common::Point position) const;

                bool getObjectAt(const Common::Point& position, ObjectEntry& data) const;
                    
                void fillDrawStruct(Common::Point position, uint16 direction);
                void draw(Valid<Graphics::Manager> sprites);
                void drawMini(const Common::Point& pen, const Common::Point& position, uint32 facing, Valid<Graphics::Manager> sprites);
    
            public:
                static Common::Point translatePoint(Common::Point position, int16 xOffset, int16 yOffset, uint16 direction);
    
            private:
                Segment* _base;
                Text* _text;
                EventList* _events;
                Objects* _objects;
        };
    }
}

#endif // XEEN_MAP_H
