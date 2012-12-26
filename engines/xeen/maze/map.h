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
        class EventList;
        class Objects;
        class Manager;
    
        struct ObjectEntry
        {
            uint8 id;
            Common::Point pos;
            Direction dir;
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
                void runEventAt(const Common::Point& pos, Direction dir, bool autoExec, uint32 line = 0);

                bool canMove(const Common::Point& pos, Direction dir) const;
                bool tryBash(const Common::Point& pos, Direction dir);

                uint16 getTile(const Common::Point& pos, Direction dir = Direction::NORTH) const;
                void setTile(const Common::Point& pos, Direction dir, uint16 value);

                uint8 getFlags(const Common::Point& pos) const;
                void setFlags(const Common::Point& pos, uint8 value);
                uint16 getSurface(const Common::Point& pos) const;

                Objects* getObjects() const { return _objects; }
                bool getObjectAt(const Common::Point& pos, ObjectEntry& data) const;
                void moveObject(uint32 id, const Common::Point& pos);
                void moveMonster(uint32 id, const Common::Point& pos, bool spawn);
                    
                void fillDrawStruct(const Common::Point& pos, Direction dir);
                void draw(Valid<Graphics::Manager> sprites);
                void drawMini(const Common::Point& pen, const Common::Point& pos, Direction dir, Valid<Graphics::Manager> sprites);
                void setSignMessage(const char* message, const Common::Rect& area, uint32 fontFlags);
    
            private:
                void processSurface(const Common::Point& pos, Direction dir, bool facingFlip, DrawListItem** index);
                void processObjects(const Common::Point& pos, Direction dir, DrawListItem** index);
                void processSideWallList(const Common::Point& pos, Direction dir, uint32 distance, uint32 count, NonNull<const int32> ids);
    
            private:
                Valid<Manager> _parent;

                Segment* _base;
                EventList* _events;
                Objects* _objects;

                FilePtr _text;

                char* _message;
                Common::Rect _messageArea;
                uint32 _messageFlags;

                const char* _wallType;
        };
    }
}

#endif // XEEN_MAP_H
