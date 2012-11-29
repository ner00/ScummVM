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

#include "common/scummsys.h"
#include "common/file.h"
#include "common/list.h"
#include "common/memstream.h"

#include "xeen/sprite.h"

namespace XEEN
{
    class MapManager;
    class Map;

    class MazeText
    {
        public:
            MazeText(CCFile& cc, uint16 mapNumber);
            ~MazeText();
            
        private:
            CCFileData* _data;        
            uint16 _stringOffsets[256];
    };
    
    class MazeObjects
    {
        public:
            struct Entry
            {
                int8 x;
                int8 y;
                uint8 id;
                uint8 facing;
            };    
    
        public:
            MazeObjects(CCFile& cc, uint16 mapNumber);
            ~MazeObjects();
            
            bool getObjectAt(int8 x, int8 y, Entry& facing);
                        
        private:
            uint8 _objectTypes[16];
            uint8 _monsterTypes[16];
            uint8 _wallObjectTypes[16];

            Common::List<Entry> _objects;
            Common::List<Entry> _monsters;
            Common::List<Entry> _wallObjects;            
    };

    class MazeSegment
    {
        friend class MapManager;
        friend class Map;
    
        private:
            MazeSegment(CCFile& cc, uint16 mapNumber);
            virtual ~MazeSegment() { }

        public:
            MazeSegment* getNorth() { return _north; }
            MazeSegment* getEast() { return _east; }
               
        protected:        
            MazeSegment* _north;
            MazeSegment* _east;
            
            MazeObjects* _objects;
        
            uint16 _wallData[16 * 16];
            uint8 _cellFlags[16 * 16];
            
            uint16 _mazeNumber;
            uint16 _mazeExtensions[4];
            uint16 _mazeFlags[2];
            
            uint8 _wallMap[16];
            uint8 _surfaceMap[16];
            
            uint8 _floorType;
            uint8 _runX;
            uint8 _wallNoPass;
            uint8 _surfNoPass;
            uint8 _unlockDoor;
            uint8 _unlockBox;
            uint8 _bashDoor;
            uint8 _bashGrate;
            uint8 _bashWall;
            uint8 _changeToRun;
            uint8 _runY;
            uint8 _trapDamage;
            uint8 _wallKind;
            uint8 _tavernType;
            
            uint8 _seenTiles[32];
            uint8 _steppedTiles[32];
    };
    
    class Map : public MazeSegment
    {
        friend class MapManager;
        friend class MazeSegment;
    
        private:
            Map(CCFile& cc, uint16 mapNumber);
            ~Map();

        public:
            uint16 getTile(int16 x, int16 y);
            uint16 getSurface(int16 x, int16 y);
                
            void fillDrawStruct(int16 x, int16 y, uint16 direction);
            void draw(byte* out, SpriteManager& sprite);
                        
        private:
            MazeSegment* resolveSegment(int16& x, int16& y);        

        public:
            static void translatePoint(int16& x, int16& y, int16 xOffset, int16 yOffset, uint16 direction);

        private:
            MazeText* _text;
            
            uint32 _width;
            uint32 _height;
    };
    
    class MapManager
    {
        public:
            MapManager(CCFile& parent);
            ~MapManager();
            
            Map* getMap(uint16 id);
            MazeSegment* getSegment(uint16 id);
            
        private:
            CCFile& _cc;
            Map* _maps[256]; // TODO: <Use a hash table!
            MazeSegment* _segments[256];
    };
}

#endif // XEEN_CCFILE_H
