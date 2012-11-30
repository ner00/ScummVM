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
#include "xeen/drawlist.h"

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
/// MazeObjects
///
XEEN::MazeObjects::MazeObjects(CCFile& cc, uint16 mapNumber)
{
    memset(_objectTypes, 0xFF, sizeof(_objectTypes));
    memset(_monsterTypes, 0xFF, sizeof(_monsterTypes));
    memset(_wallObjectTypes, 0xFF, sizeof(_wallObjectTypes));

    CCFileData* reader = cc.getSaveFile().getFile(CCFileId("MAZE%s%03d.MOB", (mapNumber < 100) ? "0" : "X", mapNumber));
    
    if(reader)
    {
        reader->read(_objectTypes, 16);
        reader->read(_monsterTypes, 16);
        reader->read(_wallObjectTypes, 16);

        // Read object list
        while(!reader->eos())
        {
            Entry e;
            e.position.x = reader->readSByte();
            e.position.y = reader->readSByte();
            e.id = reader->readByte();
            e.facing = reader->readByte();
            
            if(e.id == 0xFF)
            {
                break;
            }
            
            _objects.push_back(e);
        }

        delete reader;
    }
}

XEEN::MazeObjects::~MazeObjects()
{
    // TODO: Store the values back into the save game
}

bool XEEN::MazeObjects::getObjectAt(const Common::Point& position, Entry& data)
{
    for(Common::List<Entry>::iterator i = _objects.begin(); i != _objects.end(); i ++)
    {
        if(i->position == position)
        {
            data = *i;
            data.id = _objectTypes[data.id];
            return true;
        }
    }
    
    return false;
}

///
/// MazeSegment
///
XEEN::MazeSegment::MazeSegment(CCFile& cc, uint16 mapNumber) : _north(0), _east(0), _objects(0)
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
    
    // Load objects
    _objects = new MazeObjects(cc, mapNumber);
    
    // Done
    delete data;
}


///
/// Map
///
XEEN::Map::Map(CCFile& cc, uint16 mapNumber) : MazeSegment(cc, mapNumber), _text(0), _width(0), _height(0)
{
    assert(mapNumber < 100 && "Loading map from extended maze segment.");
 
    // Load Maze Data
    _text = new MazeText(cc, mapNumber);
    
    // Calculate size
    for(MazeSegment* tag = this; tag; tag = tag->getEast())
    {
        _width += 16;
    }
    
    for(MazeSegment* tag = this; tag; tag = tag->getNorth())
    {
        _height += 16;
    }
}

XEEN::Map::~Map()
{
    delete _text;
}

uint16 XEEN::Map::getTile(Common::Point position, uint32 direction)
{
    MazeSegment* seg = resolveSegment(position);
    
    if(position.x < 0 || position.y < 0 || !seg)
    {
        return 0x8888;
    }
    else
    {
        uint16 result = seg->_wallData[position.y * 16 + position.x];
    
        switch(direction)
        {
            case 0: return result;
            case 1: return (result >> 12) | (result << 4);
            case 2: return (result >> 8) | (result << 8);
            case 3: return (result >> 4) | (result << 12);
        }

        return result;
    }
}

uint16 XEEN::Map::getSurface(Common::Point position)
{
    MazeSegment* seg = resolveSegment(position);

    if(position.x < 0 || position.y < 0 || !seg)
    {
        return 0;
    }
    else
    {
        return _surfaceMap[seg->_cellFlags[position.y * 16 + position.x] & 0x7];
    }
}

void XEEN::Map::fillDrawStruct(Common::Point position, uint16 direction)
{
    static const CCFileId surfaceMap[16] = 
    {
        0xFFFF, "DIRT.SRF", "GRASS.SRF", "SNOW.SRF", "SWAMP.SRF", "LAVA.SRF", "DESERT.SRF", "ROAD.SRF",
        "WATER.SRF", "TFLR.SRF", "SKY.SRF", "CROAD.SRF", "SEWER.SRF", "CLOUD.SRF", "SCOORTCH.SRF", "SPACE.SRF"
    };


    indoorDrawList[0].sprite = CCFileId("SKY.SKY");
    indoorDrawList[1].sprite = CCFileId("SKY.SKY");
    indoorDrawList[2].sprite = CCFileId("TOWN.GND");
    
    // Fill surface entries
    static const int16 xoff3[3] = {-1, 1, 0};
    static const int16 xoff5[5] = {-2, -1, 2, 1, 0};
    static const int16 xoff7[7] = {-3, -2, -1, 3, 2, 1, 0};
    static const int16* const xoffsets[5] = {xoff7, xoff7, xoff5, xoff3, xoff3};
    static const uint32 linelength[5] = {7, 7, 5, 3, 3};

    int surfaceTile = 3;
    
    for(uint32 i = 0; i != 5; i ++)
    {
        for(uint32 j = 0; j != linelength[i]; j ++)
        {
            Common::Point cell = translatePoint(position, xoffsets[i][j], 4 - i, direction);
            indoorDrawList[surfaceTile ++].sprite = surfaceMap[getSurface(cell)];
        }
    }

    // Facing walls
    static const unsigned wallmap[16] = {32, 9, 17, 11, 8, 0, 15, 16, 0, 10, 14, 6, 1, 8, 12, 13};
    static const unsigned wall1[3] = {143, 145, 147};
    static const unsigned wall2[3] = {119, 120, 121};
    static const unsigned wall3[5] = {87, 88, 89, 90, 91};
    static const unsigned wall4[9] = {37, 38, 39, 40, 41, 42, 43, 44, 45};

    static const int baseframe[4] = {0, 0, 17, 0};
    static const int wallcount[4] = {3, 3, 5, 9};
    static const int walloffset[4] = {1, 1, 2, 4};
    static const unsigned* const wallbase[] = {wall1, wall2, wall3, wall4};
    static const uint16 fwlids[4] = {0, CCFileId::fromString("FTOWN3.FWL"), CCFileId::fromString("FTOWN3.FWL"), CCFileId::fromString("FTOWN4.FWL")};
    
    // Nearest walls done separately to support being split between two sprites!
    for(int i = 0; i != 3; i ++)
    {
        Common::Point cell = translatePoint(position, i - 1, 0, direction);
        uint16 wallData = wallmap[getTile(cell, direction) >> 12];
        
        if(wallData != 32)
        {
            indoorDrawList[wall1[i]].sprite = (wallData < 8) ? CCFileId("FTOWN1.FWL") : CCFileId("FTOWN2.FWL");
            indoorDrawList[wall1[i]].frame = (wallData < 8) ? wallData : wallData - 8;            
        }
        else
        {
            indoorDrawList[wall1[i]].sprite = 0xFFFF;
        }
    }
    
    for(int i = 1; i != 4; i ++)
    {
        for(int j = 0; j != wallcount[i]; j ++)
        {
            Common::Point cell = translatePoint(position, j - walloffset[i], i, direction);
            uint16 wallData = wallmap[getTile(cell, direction) >> 12];
            
            indoorDrawList[wallbase[i][j]].sprite = (wallData != 32) ? fwlids[i] : 0xFFFF;
            indoorDrawList[wallbase[i][j]].frame = baseframe[i] + ((wallData > 6) ? wallData - 1 : wallData);
        }
    }
    
    // TODO: Distant wall?


    // Side walls
    static const unsigned leftpos[3] = {144, 133};
    static const unsigned rightpos[3] = {146, 134};
    
    for(int i = 0; i != 2; i ++)
    {
        Common::Point cell = translatePoint(position, 0, i, direction);
        uint16 wallData = (getTile(cell, direction) >> 0) & 0xF;        
        
        if(wallData)
        {
            indoorDrawList[leftpos[i]].sprite = CCFileId("STOWN.SWL");
        }
        else
        {
            indoorDrawList[leftpos[i]].sprite = 0xFFFF;
        }
    }

    for(int i = 0; i != 2; i ++)
    {
        Common::Point cell = translatePoint(position, 0, i, direction);
        uint16 wallData = (getTile(cell, direction) >> 8) & 0xF;        
        
        if(wallData)
        {
            indoorDrawList[rightpos[i]].sprite = CCFileId("STOWN.SWL");
        }
        else
        {
            indoorDrawList[rightpos[i]].sprite = 0xFFFF;
        }
    }


    // SIDE WALLS: 2 Steps forward
    static const int swl2_xoffset[4] = {-1, 0, 0, 1};
    static const int swl2_shift[4] = {0, 0, 8, 8};
    for(int i = 0; i != 4; i ++)
    {
        Common::Point cell = translatePoint(position, swl2_xoffset[i], 2, direction);
        uint16 wallData = (getTile(cell, direction) >> swl2_shift[i]) & 0xF;
        indoorDrawList[107 + i].sprite = wallData ? CCFileId("STOWN.SWL") : CCFileId(0xFFFF);
    }

    // SIDE WALLS: 3 Steps forward
    static const int swl3_xoffset[8] = {-3, 3, -2, 2, -1, 1, 0, 0};
    static const int swl3_shift[8] = {0, 8, 0, 8, 0, 8, 0, 8};
    for(int i = 0; i != 8; i ++)
    {
        Common::Point cell = translatePoint(position, swl3_xoffset[i], 3, direction);
        uint16 wallData = (getTile(cell, direction) >> swl3_shift[i]) & 0xF;
        indoorDrawList[71 + i].sprite = wallData ? CCFileId("STOWN.SWL") : CCFileId(0xFFFF);    
    }

    // HACK
    MazeObjects::Entry t;
    if(_objects->getObjectAt(position, t))
    {
        indoorDrawList[149].sprite = CCFileId("%03d.OBJ", t.id);
    }
    else
    {
        indoorDrawList[149].sprite = 0xFFFF;
    }
}

void XEEN::Map::draw(ImageBuffer& out, SpriteManager& sprites)
{
    for(int i = 0; i != sizeof(indoorDrawList) / sizeof(indoorDrawList[0]); i ++)
    {
        if(indoorDrawList[i].sprite != 0xFFFF)
        {
            Sprite* const sprite = sprites.getSprite(indoorDrawList[i].sprite);
            sprite->drawCell(out, Common::Point(indoorDrawList[i].x, indoorDrawList[i].y), indoorDrawList[i].frame, indoorDrawList[i].flags & 0x8000);
        }
    }
}

XEEN::MazeSegment* XEEN::Map::resolveSegment(Common::Point& position)
{
    MazeSegment* activeSegment = this;
    
    for(; activeSegment && position.x >= 16; position.x -= 16)
    {
        activeSegment = activeSegment->_east;
    }

    for(; activeSegment && position.y >= 16; position.y -= 16)
    {
        activeSegment = activeSegment->_north;
    }

    return activeSegment;
}

Common::Point XEEN::Map::translatePoint(Common::Point position, int16 xOffset, int16 yOffset, uint16 direction)
{
    switch(direction)
    {
        case 0:
        {
            position.x += xOffset;
            position.y += yOffset;
            return position;
        }
        
        case 1:
        {
            position.x += yOffset;
            position.y -= xOffset;
            return position;
        }
        
        case 2:
        {
            position.x -= xOffset;
            position.y -= yOffset;
            return position;
        }
        
        case 3:
        {
            position.x -= yOffset;
            position.y += xOffset;
            return position;
        }
    }
    
    return position;
}
