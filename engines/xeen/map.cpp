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

static struct
{
    uint8 frame;
    int16 x;
    int16 y;
    uint8 scale;
    uint16 flags;
}   drawS[] =
{
    {0, 8, 8, 0, 0},
    {1, 8, 25, 0, 0},
    {0, 8, 67, 0, 0}
};

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

void XEEN::Map::draw(byte* out)
{
}
