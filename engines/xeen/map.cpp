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


#include "xeen/ccfile.h"
#include "xeen/map.h"
#include "xeen/sprite.h"

#include "common/file.h"
#include "common/debug.h"

#include "xeen/utility.h"

/*static const uint16 s_surfaceMap[16] = 
{
    0,
    XEEN::CCFileId::fromString("DIRT.SRF"),
    XEEN::CCFileId::fromString("GRASS.SRF"),
    XEEN::CCFileId::fromString("SNOW.SRF"),
    XEEN::CCFileId::fromString("SWAMP.SRF"),
    XEEN::CCFileId::fromString("LAVA.SRF"),
    XEEN::CCFileId::fromString("DESERT.SRF"),
    XEEN::CCFileId::fromString("ROAD.SRF"),
    XEEN::CCFileId::fromString("WATER.SRF"),
    XEEN::CCFileId::fromString("TFLR.SRF"),
    XEEN::CCFileId::fromString("SKY.SRF"),
    XEEN::CCFileId::fromString("CROAD.SRF"),
    XEEN::CCFileId::fromString("SEWER.SRF"),
    XEEN::CCFileId::fromString("CLOUD.SRF"),
    XEEN::CCFileId::fromString("SCOORTCH.SRF"), //?
    XEEN::CCFileId::fromString("SPACE.SRF")
};*/

///
/// MapManager
///
XEEN::MapManager::MapManager(CCFile& parent) : _cc(parent)
{
    memset(_maps, 0, sizeof(_maps));
    memset(_segments, 0, sizeof(_segments));
}

XEEN::MapManager::~MapManager()
{
    for(int i = 0; i != 256; i ++)
    {
        delete _maps[i];
        delete _segments[i];
    }
}

XEEN::Map* XEEN::MapManager::getMap(uint16 id)
{
    if(!_maps[id])
    {
        _maps[id] = new Map(_cc, id);
    }
    
    return _maps[id];
}

XEEN::MazeSegment* XEEN::MapManager::getSegment(uint16 id)
{
    if(!_segments[id])
    {
        _segments[id] = new MazeSegment(_cc, id);
    }
    
    return _segments[id];
}

///
/// MazeSegment
///
XEEN::MazeSegment::MazeSegment(CCFile& cc, uint16 mapNumber) : _north(0), _east(0)
{
    CCFileData* data = cc.getSaveFile().getFile(CCFileId("MAZE%s%03d.DAT", (mapNumber < 100) ? "0" : "X", mapNumber));    
    assert(data->size() && "Failed to open maze segment chunk");

    // Parse data
    for(int i = 0; i != 16 * 16; i ++)
    {
        _wallData[i] = data->readUint16LE();
    }
    
    data->read(_cellFlags, 16 * 16);
    
    _mazeNumber = data->readUint16LE();
    
    for(int i = 0; i != 4; i ++)
    {
        _mazeExtensions[i] = data->readUint16LE();
    }
    
    _mazeFlags[0] = data->readUint16LE();
    _mazeFlags[1] = data->readUint16LE();
    
    data->read(_wallMap, 16);
    data->read(_surfaceMap, 16);
    
    _floorType = data->readByte();
    _runX = data->readByte();
    _wallNoPass = data->readByte();
    _surfNoPass = data->readByte();
    _unlockDoor = data->readByte();
    _unlockBox = data->readByte();
    _bashDoor = data->readByte();
    _bashGrate = data->readByte();
    _bashWall = data->readByte();
    _changeToRun = data->readByte();
    _runY = data->readByte();
    _trapDamage = data->readByte();
    _wallKind = data->readByte();
    _tavernType = data->readByte();
    
    data->read(_seenTiles, 32);
    data->read(_steppedTiles, 32);
    
    // Load extensions
    if(_mazeExtensions[0])
    {
        assert(_mazeExtensions[0] >= 100 && "Indoor map extension index issue.");
        _north = cc.getMapManager().getSegment(_mazeExtensions[0]);
    }
    
    if(_mazeExtensions[1])
    {
        assert(_mazeExtensions[1] >= 100 && "Indoor map extension index issue.");    
        _east = cc.getMapManager().getSegment(_mazeExtensions[1]);
    }
    
    // Done
    delete data;
}

uint16 XEEN::MazeSegment::getTile(uint16 x, uint16 y)
{
    return _wallData[y * 16 + x];
}

uint16 XEEN::MazeSegment::getSurface(uint16 x, uint16 y)
{
    return _surfaceMap[_cellFlags[y * 16 + x] & 0x7];
}

///
/// MazeText
///
XEEN::MazeText::MazeText(CCFile& cc, uint16 mapNumber) : _data(0)
{
    // Get ID
    _data = cc.getFile(CCFileId("AAZE%04d.TXT", mapNumber));
    memset(_stringOffsets, 0xFF, sizeof(_stringOffsets));

    if(_data)
    {    
        uint32 foundStrings = 1;
        _stringOffsets[0] = 0;
        
        for(int32 offset = 1; offset < _data->size() - 1; offset ++)
        {
            if(_data->getData()[offset] == 0)
            {
                _stringOffsets[foundStrings ++] = offset + 1;
            }
        }
    }
}

XEEN::MazeText::~MazeText()
{
    delete _data;
}


///
/// Map
///
static struct
{
    uint16 sprite;      //< The CCFileId of the sprite to draw
    uint8 frame;        //< Sprite cell index to draw
    const int16 x;      //< X position to draw at
    const int16 y;      //< Y position to draw at
    uint8 scale;        //< ?
    const uint16 flags; //< ?
}   indoorDrawList[] =
{
    {0xFFFF,	 0,	 8,	     8,	     0,	    0x0000}, //Top half of sky
    {0xFFFF,	 1,	 8,	     25,	 0,	    0x0000}, //Bottom half of sky
    {0xFFFF,	 0,	 8,	     67,	 0,	    0x0000}, //Ground
    {0xFFFF,	 18, 8,	     67,	 0,	    0x0000}, //Surface tile 4 steps forward, 3 steps left
    {0xFFFF,	 19, 38,	 67,	 0,	    0x0000}, //Surface tile 4 steps forward, 2 steps left
    {0xFFFF,	 20, 84,	 67,	 0,	    0x0000}, //Surface tile 4 steps forward, 1 steps left
    {0xFFFF,	 24, 134,	 67,	 0,	    0x0000}, //Surface tile 4 steps forward, 3 steps right
    {0xFFFF,	 23, 117,	 67,	 0,	    0x0000}, //Surface tile 4 steps forward, 2 steps right
    {0xFFFF,	 22, 117,	 67,	 0,	    0x0000}, //Surface tile 4 steps forward, 1 steps right
    {0xFFFF, 	 21, 103,	 67,	 0,	    0x0000}, //Surface tile directly 4 steps forward
    {0xFFFF, 	 11, 8,	     73,     0,	    0x0000}, //Surface tile 3 steps forward, 3 steps left
    {0xFFFF, 	 12, 8,	     73,	 0,	    0x0000}, //Surface tile 3 steps forward, 2 steps left
    {0xFFFF, 	 13, 30,	 73,	 0,	    0x0000}, //Surface tile 3 steps forward, 1 steps left
    {0xFFFF, 	 17, 181,	 73,	 0,	    0x0000}, //Surface tile 3 steps forward, 3 steps right
    {0xFFFF, 	 16, 154,	 73,	 0,	    0x0000}, //Surface tile 3 steps forward, 2 steps right
    {0xFFFF, 	 15, 129,	 73,	 0,	    0x0000}, //Surface tile 3 steps forward, 1 steps right
    {0xFFFF, 	 14, 87,	 73,	 0,	    0x0000}, //Surface tile directly 3 steps forward
    {0xFFFF, 	 6,  8,	     81,	 0,	    0x0000}, //Surface tile 2 steps forward, 2 left
    {0xFFFF, 	 7,  8,	     81,	 0,	    0x0000}, //Surface tile 2 steps forward, 1 left
    {0xFFFF, 	 10, 202,	 81,	 0,	    0x0000}, //Surface tile 2 steps forward, 2 right
    {0xFFFF, 	 9,  145,	 81,	 0,	    0x0000}, //Surface tile 2 steps forward, 1 right
    {0xFFFF, 	 8,  63,	 81,	 0,	    0x0000}, //Surface tile directly 2 steps forward
    {0xFFFF, 	 3,  8,	     93,	 0,	    0x0000}, //Surface tile 1 step forward, 1 left
    {0xFFFF, 	 5,  169,	 93,	 0,	    0x0000}, //Surface tile 1 step forward, 1 right
    {0xFFFF, 	 4,  31,	 93,	 0,	    0x0000}, //Surface tile directly 1 step forward
    {0xFFFF, 	 0,  8,	     109,	 0,	    0x0000}, //Surface tile directly 1 step left
    {0xFFFF, 	 2,  201,	 109,	 0,	    0x0000}, //Surface tile directly 1 step right
    {0xFFFF, 	 1,  8,	     109,	 0,	    0x0000}, //Surface tile player is currently on
    {0xFFFF, 	 7,	 8,	     64,	 0,	    0x0000}, //Far distant wall
    {0xFFFF, 	 22, 32,	 60,	 0,	    0x0000}, //Side wall for tile 4 steps forward, 4 steps left
    {0xFFFF, 	 20, 56,	 60,	 0,	    0x0000}, //Side wall for tile 4 steps forward, 3 steps left
    {0xFFFF, 	 18, 80,	 60,	 0,	    0x0000}, //Side wall for tile 4 steps forward, 2 steps left
    {0xFFFF, 	 16, 104,	 60,	 0,	    0x0000}, //Side wall for tile 4 steps forward, 1 steps left
    {0xFFFF, 	 23, 152,	 60,	 0,	    0x8000}, //Side wall for tile 4 steps forward, 4 steps right
    {0xFFFF, 	 21, 144,	 60,	 0,	    0x8000}, //Side wall for tile 4 steps forward, 3 steps right
    {0xFFFF, 	 19, 131,	 60,	 0,	    0x8000}, //Side wall for tile 4 steps forward, 2 steps right
    {0xFFFF, 	 17, 120,	 60,	 0,	    0x8000}, //Side wall for tile 4 steps forward, 1 steps right
    {0xFFFF, 	 14, 8,	     60,	 0,	    0x0000}, //Facing wall for tile 4 steps forward, 4 steps left
    {0xFFFF, 	 12, 32,	 60,	 0,	    0x0000}, //Facing wall for tile 4 steps forward, 3 steps left
    {0xFFFF, 	 10, 56,	 60,	 0,	    0x0000}, //Facing wall for tile 4 steps forward, 2 steps left
    {0xFFFF, 	 14, 80,	 60,	 0,	    0x0000}, //Facing wall for tile 4 steps forward, 1 steps left
    {0xFFFF, 	 14, 104,	 60,	 0,	    0x0000}, //Facing wall for tile directly 4 steps forward
    {0xFFFF, 	 14, 128,	 60,	 0,	    0x0000}, //Facing wall for tile 4 steps forward, 1 steps right
    {0xFFFF, 	 14, 152,	 60,	 0,	    0x0000}, //Facing wall for tile 4 steps forward, 2 steps right
    {0xFFFF, 	 8,	 176,	 60,	 0,	    0x0000}, //Facing wall for tile 4 steps forward, 3 steps right
    {0xFFFF, 	 8,	 200,	 60,	 0,	    0x0000}, //Facing wall for tile 4 steps forward, 4 steps right
    {0xFFFF, 	 0,	 -64,	 61,	 14,	0x0000},
    {0xFFFF, 	 0,	 -40,	 61,	 14,	0x0000},
    {0xFFFF, 	 0,	 -16,	 61,	 14,	0x0000},	
    {0xFFFF, 	 0,	 8,	     61,	 14,	0x0000},	
    {0xFFFF, 	 0,	 32,	 61,	 14,	0x0000},	
    {0xFFFF, 	 0,	 56,	 61,	 14,    0x0000},	
    {0xFFFF, 	 0,	 80,	 61,	 14,	0x0000},	
    {0xFFFF, 	 0,	 104,	 61,	 14,    0x0000},	
    {0xFFFF, 	 0,	 128,	 61,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -9,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -34,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 16,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -58,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 40,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -41,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -26,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -34,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -16,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 23,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 16,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -58,	 58,	 14,    0x0000},	
    {0xFFFF, 	 0,	 40,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -17,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -1,	 58,	 14,	0x0000},	
    {0xFFFF, 	 0,	 -9,	 58,	 14,	0x0000},	
    {0xFFFF, 	 14, 8,	     58,	 0,	    0x0000}, //Side wall for tile 3 steps forward, 4 steps left
    {0xFFFF, 	 12, 8,	     55,	 0,	    0x0000}, //Side wall for tile 3 steps forward, 3 steps left
    {0xFFFF, 	 10, 32,	 52,	 0,	    0x0000}, //Side wall for tile 3 steps forward, 2 steps left
    {0xFFFF, 	 14, 88,	 52,	 0,	    0x0000}, //Side wall for tile 3 steps forward, 1 steps left
    {0xFFFF, 	 14, 128,	 52,	 0,	    0x8000}, //Side wall for tile 3 steps forward, 1 steps right
    {0xFFFF, 	 14, 152,	 52,	 0,	    0x8000}, //Side wall for tile 3 steps forward, 2 steps right
    {0xFFFF, 	 0,	 176,	 55,	 0,	    0x8000}, //Side wall for tile 3 steps forward, 3 steps right
    {0xFFFF, 	 0,	 200,	 58,	 0,	    0x8000}, //Side wall for tile 3 steps forward, 4 steps right
    {0xFFFF, 	 0,	 72,	 58,	 12,	0x0000}, //POW? sprite 4 steps forward
    {0xFFFF, 	 0,	 72,	 58,	 12,	0x8000}, //POW? sprite 4 steps forward
    {0xFFFF, 	 0,	 69,	 63,	 12,	0x0000}, //POW? sprite 4 steps forward
    {0xFFFF, 	 0,	 75,	 63,	 12,	0x8000}, //POW? sprite 4 steps forward
    {0xFFFF, 	 0,	 73,	 53,	 12,    0x0000}, //POW? sprite 4 steps forward
    {0xFFFF, 	 0,	 71,	 53,	 12,	0x8000}, //POW? sprite 4 steps forward
    {0xFFFF, 	 0,	 80,	 57,	 12,	0x0000}, //POW? sprite 4 steps forward
    {0xFFFF, 	 0,	 64,	 57,	 12,	0x8000}, //POW? sprite 4 steps forward
    {0xFFFF, 	 7,	 -24,	 52,	 0,	    0x2000}, //Facing wall for tile 3 steps forward, 2 steps left
    {0xFFFF, 	 7,	 32,	 52,	 0,	    0x0000}, //Facing wall for tile 3 steps forward, 1 step left
    {0xFFFF, 	 7,	 88,	 52,	 0,	    0x0000}, //Facing wall for tile directly 3 steps forward
    {0xFFFF, 	 0,	 144,	 52,	 0,	    0x0000}, //Facing wall for tile 3 steps forward, 1 step right
    {0xFFFF, 	 0,	 200,	 52,	 0,	    0x2000}, //Facing wall for tile 3 steps forward, 2 steps right
    {0xFFFF, 	 0,	 -79,	 52,	 11,	0x2000},	
    {0xFFFF, 	 0,	 -27,	 52,	 11,	0x0000},	
    {0xFFFF, 	 0,	 32,	 52,	 11,	0x0000},	
    {0xFFFF, 	 0,	 89,	 52,	 11,	0x0000},	
    {0xFFFF, 	 0,	 145,	 52,	 11,	0x2000},	
    {0xFFFF, 	 0,	 -8,	 50,	 12,	0x0000},	
    {0xFFFF, 	 0,	 -65,	 50,	 12,	0x0000},	
    {0xFFFF, 	 0,	 49,	 50,	 12,	0x0000},	
    {0xFFFF, 	 0,	 -65,	 50,	 12,	0x0000},	
    {0xFFFF, 	 0,	 -81,	 50,	 12,	0x0000},	
    {0xFFFF, 	 0,	 49,	 50,	 12,	0x0000},	
    {0xFFFF, 	 0,	 65,	 50,	 12,	0x0000},	
    {0xFFFF, 	 0,	 -24,	 50,	 12,	0x0000},	
    {0xFFFF, 	 0,	 9,	     50,     12,	0x0000},	
    {0xFFFF, 	 0,	 -8,	 50,	 12,	0x0000},	
    {0xFFFF, 	 7,	 8,      48,	 0,	    0x0000}, //Side wall for tile 2 steps forward, 2 steps left
    {0xFFFF, 	 7,	 64,	 40,	 0,	    0x0000}, //Side wall for tile 2 steps forward, 1 steps left
    {0xFFFF, 	 6,	 144,	 40,	 0,	    0x8000}, //Side wall for tile 2 steps forward, 1 steps right
    {0xFFFF, 	 6,	 200,	 48,	 0,	    0x8000}, //Side wall for tile 2 steps forward, 2 steps right
    {0xFFFF, 	 0,	 72,	 53,	 8,	    0x0000}, //POW? sprite 3 steps forward
    {0xFFFF, 	 0,	 72,	 53,	 8,	    0x8000}, //POW? sprite 3 steps forward
    {0xFFFF, 	 0,	 77,	 58,	 8,	    0x0000}, //POW? sprite 3 steps forward
    {0xFFFF, 	 0,	 67,	 58,	 8,	    0x8000}, //POW? sprite 3 steps forward
    {0xFFFF, 	 0,	 81,	 47,	 8,	    0x0000}, //POW? sprite 3 steps forward
    {0xFFFF, 	 0,	 63,	 47,	 8,	    0x8000}, //POW? sprite 3 steps forward
    {0xFFFF, 	 0,	 94,	 52,	 8,	    0x0000}, //POW? sprite 3 steps forward
    {0xFFFF, 	 0,	 50,	 52,	 8,	    0x8000}, //POW? sprite 3 steps forward
    {0xFFFF, 	 6,	 -40,	 40,	 0,	    0x2000}, //Facing wall for tile 2 steps forward, 1 step left
    {0xFFFF, 	 6,	 64,	 40,	 0,	    0x0000}, //Facing wall for tile directly 2 steps forward
    {0xFFFF, 	 0,	 168,	 40,	 0,	    0x2000}, //Facing wall for tile 2 steps forward, 1 step right
    {0xFFFF, 	 0,	 -72,	 40,	 6,	    0x2000},	
    {0xFFFF, 	 0,	 32,	 40,	 6,	    0x0000},	
    {0xFFFF, 	 0,	 137,	 40,	 6,	    0x2000},	
    {0xFFFF, 	 0,	 -7,	 25,	 7,	    0x0000},	
    {0xFFFF, 	 0,	 -112,	 25,	 7,	    0x2000},	
    {0xFFFF, 	 0,	 98,	 25,	 7,	    0x2000},	
    {0xFFFF, 	 0,	 -112,	 29,	 8,	    0x2000},	
    {0xFFFF, 	 0,	 98,	 29,	 8,	    0x2000},	
    {0xFFFF, 	 0,	 -38,	 29,	 8,	    0x0000},	
    {0xFFFF, 	 0,	 25,	 29,	 8,	    0x0000},	
    {0xFFFF, 	 0,	 -7,	 29,	 8,	    0x0000},	
    {0xFFFF, 	 6,	 32,	 24,	 0,	    0x0000}, //Side wall for tile 1 step forward, 1 steps left
    {0xFFFF, 	 0,	 168,	 24,	 0,	    0x8000}, //Side wall for tile 1 step forward, 1 steps right
    {0xFFFF, 	 0,	 72,	 48,	 4,	    0x0000}, //POW? sprite 2 steps forward
    {0xFFFF, 	 0,	 72,	 48,	 4,	    0x8000}, //POW? sprite 2 steps forward
    {0xFFFF, 	 0,	 85,	 53,	 4,	    0x0000}, //POW? sprite 2 steps forward
    {0xFFFF, 	 0,	 59,	 53,	 4,	    0x8000}, //POW? sprite 2 steps forward
    {0xFFFF, 	 0,	 89,	 41,	 4,	    0x0000}, //POW? sprite 2 steps forward
    {0xFFFF, 	 0,	 55,	 41,	 4,	    0x8000}, //POW? sprite 2 steps forward
    {0xFFFF, 	 0,	 106,	 47,	 4,	    0x0000}, //POW? sprite 2 steps forward
    {0xFFFF, 	 0,	 38,	 47,	 4,	    0x8000}, //POW? sprite 2 steps forward
    {0xFFFF, 	 0,	 -136,	 24,	 0,	    0x2000}, //Facing wall for tile 1 step forward, 1 step left
    {0xFFFF, 	 0,	 8,	     12,	 0,	    0x0000}, //Side wall for tile directly 1 step left
    {0xFFFF, 	 0,	 32,	 24,	 0,	    0x0000}, //Facing wall for tile 1 step forward, 1 step right
    {0xFFFF, 	 0,	 200,	 12,	 0,	    0x8000}, //Side wall for tile directly 1 step right
    {0xFFFF, 	 0,	 200,	 24,	 0,	    0x2000}, //Facing wall for tile directly 1 step forward
    {0xFFFF, 	 0,	 32,	 24,	 0,	    0x0000},	
    {0xFFFF, 	 0,	 -5,	 2, 	 0,	    0x6000},	
    {0xFFFF, 	 0,	 -67,	 10,	 0,	    0x6000},	
    {0xFFFF, 	 0,	 44,	 73,	 0,	    0x0000},	
    {0xFFFF, 	 0,	 44,	 73,	 0,	    0x0000},	
    {0xFFFF, 	 0,	 58,	 14,	 0,	    0x6000},	
    {0xFFFF, 	 0,	 169,	 73,	 0,	    0x0000},	
    {0xFFFF, 	 0,	 169,	 73,	 0,	    0x0000},	
    {0xFFFF, 	 0,	 -5,	 14,	 0,	    0x6000},	
    {0xFFFF, 	 0,	 110,	 73,	 0,	    0x0000},	
    {0xFFFF, 	 0,	 110,	 73,	 0,	    0x0000},	
    {0xFFFF, 	 0,	 -5,	 14,	 0,	    0x6000},	
    {0xFFFF, 	 0,	 110,	 73,	 0,	    0x0000},	
    {0xFFFF, 	 0,	 110,	 73,	 0,	    0x0000},	
    {0xFFFF, 	 0,	 72,	 43,	 0,	    0x0000}, //POW? sprite 1 steps forward
    {0xFFFF, 	 0,	 72,	 43,	 0,	    0x8000}, //POW? sprite 1 steps forward
    {0xFFFF, 	 0,	 93,	 48,	 0,	    0x0000}, //POW? sprite 1 steps forward
    {0xFFFF, 	 0,	 51,	 48,	 0,	    0x8000}, //POW? sprite 1 steps forward
    {0xFFFF, 	 0,	 97,	 36,	 0,	    0x0000}, //POW? sprite 1 steps forward
    {0xFFFF, 	 0,	 47,	 36,	 0,	    0x8000}, //POW? sprite 1 steps forward
    {0xFFFF, 	 0,	 118,	 42,	 0,	    0x0000}, //POW? sprite 1 steps forward
    {0xFFFF, 	 0,	 26,	 42,	 0,	    0x8000}	 //POW? sprite 1 steps forward
};

XEEN::Map::Map(CCFile& cc, uint16 mapNumber) : _baseSegment(0), _text(0), _width(0), _height(0)
{    
    // Load Maze Data
    _baseSegment = cc.getMapManager().getSegment(mapNumber);
    _text = new MazeText(cc, mapNumber);
    
    // Calculate size
    for(MazeSegment* tag = _baseSegment; tag; tag = tag->getEast())
    {
        _width += 16;
    }
    
    for(MazeSegment* tag = _baseSegment; tag; tag = tag->getNorth())
    {
        _height += 16;
    }
}

XEEN::Map::~Map()
{
    delete _text;
}

void XEEN::Map::draw(byte* out, SpriteManager& sprites)
{
    for(int i = 0; i != sizeof(indoorDrawList) / sizeof(indoorDrawList[0]); i ++)
    {
        if(indoorDrawList[i].sprite != 0xFFFF)
        {
            Sprite* const sprite = sprites.getSprite(indoorDrawList[i].sprite);
            sprite->drawCell(out, indoorDrawList[i].frame, indoorDrawList[i].x, indoorDrawList[i].y);
        }
    }
}
