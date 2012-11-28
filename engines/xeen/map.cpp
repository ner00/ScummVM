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

XEEN::MazeSegment::MazeSegment(CCFile& cc, uint16 mapNumber) : _cc(cc), _id((uint16)0), _north(0), _east(0)
{
    // Get maze file ID
    char buf[32];
    sprintf(buf, "MAZE%s%03d.DAT", (mapNumber < 100) ? "0" : "X", mapNumber);
    _id = buf;

    //
    CCFileData* data = _cc.getSaveFile().getFile(_id);
    
    assert(data->size() && "Failed to open maze segment chunk");


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
    
    // TODO: Close File
    
    if(_mazeExtensions[0])
    {
        _north = new MazeSegment(cc, _mazeExtensions[0]);
    }
    
    if(_mazeExtensions[1])
    {
        _east = new MazeSegment(cc, _mazeExtensions[1]);
    }
}

XEEN::MazeSegment::~MazeSegment()
{
    delete _north;
    delete _east;
}

XEEN::MazeText::MazeText(CCFile& cc, uint16 mapNumber) : _cc(cc), _id((uint16)0), _data(0)
{
    // Get ID
    char buf[32];
    sprintf(buf, "AAZE%04d.TXT", mapNumber);
    _id = buf;
    
    //
    _data = _cc.getFile(_id);
    memset(_stringOffsets, 0xFF, sizeof(_stringOffsets));

    if(_data)
    {    
        uint32 foundStrings = 1;
        _stringOffsets[0] = 0;
        
        for(uint32 offset = 1; offset < _data->size() - 1; offset ++)
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
    // TODO: Close file
}

XEEN::Map::Map(CCFile& cc, uint16 mapNumber) : _cc(cc), _baseSegment(0), _text(0), _width(0), _height(0)
{    
    // TODO: CLOSE FILE

    // Load Maze Data
    _baseSegment = new MazeSegment(cc, mapNumber);
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
    delete _baseSegment;
}

void XEEN::Map::draw(byte* out)
{
}
