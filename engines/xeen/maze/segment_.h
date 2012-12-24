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

#ifndef XEEN_MAZE_SEGMENT_H
#define XEEN_MAZE_SEGMENT_H

#ifndef XEEN_MAZE_SOURCE
# error "Private header included"
#endif

#include "xeen/utility.h"
#include "xeen/archive/file.h"
#include "xeen/maze/objects_.h"

namespace XEEN
{
    namespace Maze
    {
        class Map;
        class Manager;
    
        // Only accessible by MapManager and Map
        class Segment : public Validateable, public Common::NonCopyable
        {
            friend class Map;
            friend class Manager;

            private:
                static const uint32 INSIDE      = 0x08;
                static const uint32 AUTOEXECUTE = 0x10;
                static const uint32 WATER       = 0x40;
                static const uint32 GRATE       = 0x80;

                static const uint32 MAP_OUTDOORS        = 0x80000000;
                static const uint32 MAP_ISDARK          = 0x40000000;
                static const uint32 MAP_BLOCKSAVE       = 0x00008000;
                static const uint32 MAP_BLOCKREST       = 0x00004000;
                static const uint32 MAP_UNKNOWN1        = 0x00002000;
                static const uint32 MAP_BLOCKTELEPORT   = 0x00001000;
                static const uint32 MAP_BLOCKBEACON     = 0x00000800;
                static const uint32 MAP_BLOCKTIMEDIST   = 0x00000400;
                static const uint32 MAP_BLOCKSUPERSHLTR = 0x00000200;
                static const uint32 MAP_BLOCKTOWNPORTAL = 0x00000100;
                static const uint32 MAP_UNKNOWN2        = 0x00000080;
                static const uint32 MAP_BLOCKETHERALIZE = 0x00000040;

                enum SegmentValue
                {
                    FLOORTYPE, RUNX, WALLNOPASS, SURFACENOPASS, UNLOCKDOOR, UNLOCKBOX,
                    BASHDOOR, BASHGRATE, BASHWALL, CHANCETORUN, RUNY, TRAPDAMAGE, WALLTYPE,
                    TAVERNTIPS, MAX_VALUE
                };
        
            private:
                Segment(Valid<Manager> parent, FilePtr data);
                void loadSurrounding();
                uint16 getSurrounding(Direction dir) const;
    
                void setWall(Common::Point pos, Direction dir, LessThan<uint32, 16> type);

                uint16 getTile(Common::Point pos, Direction dir) const;
                void setTile(Common::Point pos, Direction dir, uint16 value);
                uint8 getCellFlags(Common::Point pos) const;

                uint32 getMapFlags() const;

                uint8 lookupSurface(uint8 id) const;
                uint8 lookupWall(uint8 id) const;

                uint32 getValue(SegmentValue val) const;

                Segment* resolveSegment(Common::Point& pos) const;
    
            protected:
                Valid<Manager> _parent;
                FilePtr _data;
    
                Segment* _surrMazes[4];
        };
    }
}

#endif // XEEN_MAZE_SEGMENT_H
