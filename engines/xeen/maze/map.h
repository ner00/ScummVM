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
        class Manager;
    
        struct ObjectEntry
        {
            uint8 id;
            Direction facing;
        };    

        struct DrawListItem;

        // Only constructible by MapManager
        class Map : public Validateable, public GameHolder, public Common::NonCopyable
        {
            friend class Manager;

            private:
                Map(Valid<Manager> parent, uint16 mapNumber);
                ~Map();

            public:
                const char* getString(uint32 id) const;
                void runEventAt(const Common::Point& pos, Direction facing, uint32 line = 0);
    
                bool canMove(const Common::Point& position, Direction dir) const;

                uint16 getTile(Common::Point position, Direction facing = Direction::NORTH) const;
                uint8 getFlags(Common::Point position) const;
                uint16 getSurface(Common::Point position) const;

                bool getObjectAt(const Common::Point& position, ObjectEntry& data) const;
                    
                void fillDrawStruct(Common::Point position, Direction facing);
                void draw(Valid<Graphics::Manager> sprites);
                void drawMini(const Common::Point& pen, const Common::Point& position, Direction facing, Valid<Graphics::Manager> sprites);
    
            private:
                void processSurface(const Common::Point& position, Direction facing, bool facingFlip, DrawListItem** index);
                void processObjects(const Common::Point& position, Direction facing, DrawListItem** index);
                void processSideWallList(const Common::Point& position, Direction facing, uint32 distance, uint32 count, NonNull<const int32> ids);
    
            private:
                Valid<Manager> _parent;

                Segment* _base;
                Text* _text;
                EventList* _events;
                Objects* _objects;
        };
    }
}

#endif // XEEN_MAP_H
