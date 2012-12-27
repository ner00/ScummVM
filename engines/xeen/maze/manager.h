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

#ifndef XEEN_MAZE_MANAGER_H
#define XEEN_MAZE_MANAGER_H

#include "xeen/utility.h"

namespace XEEN
{
    class Game;

    namespace Maze
    {
        class Map;
        class Segment;
        class MonsterData;

        class Manager : public Validateable, public GameHolder, public Common::NonCopyable
        {
            friend class XEEN::Game;
        
            public:
                static const uint32 MAX_OBJECTS = 121;
                static const uint32 OBJECT_RECORD_SIZE = 12;
                struct ObjectData
                {
                    uint8 start;
                    uint8 flipped;
                    uint8 end;
                };

            private:
                Manager(Valid<Game> parent);
                ~Manager();
                
            public:
                Map* getMap(uint16 id);
                Segment* getSegment(uint16 id);

                ObjectData getObjectData(uint32 id, Direction dir) const;
                const MonsterData* getMonsterData() const { return _monsterData; }
                
            private:
                Map* _maps[256]; // TODO: <Use a hash table!
                Segment* _segments[256];

                uint8 _objectData[MAX_OBJECTS * OBJECT_RECORD_SIZE];

                MonsterData* _monsterData;
        };
    }
}

#endif // XEEN_MAZE_MANAGER_H
