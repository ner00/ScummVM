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
#define XEEN_MAZE_SOURCE

#include "xeen/game.h"

#include "xeen/utility.h"

#include "xeen/graphics/manager.h"

#include "xeen/maze/map.h"
#include "xeen/maze/eventlist_.h"
#include "xeen/maze/text_.h"
#include "xeen/maze/segment_.h"
#include "xeen/maze/objectdata_.h"
#include "xeen/maze/monsterdata_.h"

namespace XEEN
{
    namespace Maze
    {
        #include "xeen/maze/drawlist_.h"
    }
}

XEEN::Maze::Map::Map(Valid<Manager> parent, uint16 mapNumber) : GameHolder(parent->getGame()), _parent(parent), _base(0), _text(0), _events(0), _objects(0), _message(0), _messageFlags(0), _wallType("TOWN")
{
    _base = _parent->getSegment(mapNumber);
 
    if(valid(_base))
    {
        // Load Maze Data
        _text = new Text(_parent->getGame()->getFile(CCFileId("AAZE%s%03d.TXT", (mapNumber < 100) ? "0" : "X", mapNumber), false));
        _events = new EventList(this, _parent->getGame()->getFile(CCFileId("MAZE%s%03d.EVT", (mapNumber < 100) ? "0" : "X", mapNumber), true));
        _objects = new Objects(_parent->getGame()->getFile(CCFileId("MAZE%s%03d.MOB", (mapNumber < 100) ? "0" : "X", mapNumber), true));

        if(!(valid(_text) && valid(_events) && valid(_objects)))
        {
            markInvalid("Failed to open maps required objects.");
            delete _text;
            delete _events;
            delete _objects;
        }

        // Get wall type
        static const char* const types[6] = {"TOWN", "CAVE", "TOWN", "CSTL", "TOWN", "TOWN"};
        const uint32 wallType = _base->getValue(Segment::WALLTYPE);

        if(wallType < 6)
        {
            _wallType = types[wallType];
        }
        else
        {
            markInvalid("Map has unknown wall type.");
        }
    }
    else
    {
        markInvalid("Failed to open map file");
    }
}

XEEN::Maze::Map::~Map()
{
    delete _text;
    delete _events;
    delete _objects;
}

const char* XEEN::Maze::Map::getString(uint32 id) const
{
    XEEN_VALID();
    return valid(_text) ? _text->getString(id) : "";
}

void XEEN::Maze::Map::runEventAt(const Common::Point& pos, Direction dir, bool autoExec, uint32 line)
{
    XEEN_VALID();

    if(valid(_events))
    {
        if(autoExec)
        {
            // Traverse off side of outdoor map.
            if(_base->getMapFlags() & Segment::MAP_OUTDOORS)
            {
                // TODO!
                if(pos.x < 0)
                {
                    getGame()->getParty()->changeMap(_base->getSurrounding(Direction::WEST));
                    getGame()->getParty()->moveTo(pos + Common::Point(16, 0), 4);
                }
                else if(pos.x >= 16)
                {
                    getGame()->getParty()->changeMap(_base->getSurrounding(Direction::EAST));
                    getGame()->getParty()->moveTo(pos - Common::Point(16, 0), 4);
                }
        
                if(pos.y < 0)
                {
                    getGame()->getParty()->changeMap(_base->getSurrounding(Direction::SOUTH));
                    getGame()->getParty()->moveTo(pos + Common::Point(0, 16), 4);
                }
                else if(pos.y >= 16)
                {
                    getGame()->getParty()->changeMap(_base->getSurrounding(Direction::NORTH));
                    getGame()->getParty()->moveTo(pos - Common::Point(0, 16), 4);
                }
            }

            // Reset sign message
            setSignMessage(0, Common::Rect(), 0);
        }

        const bool autoExecFlag = (getFlags(pos) & (Segment::AUTOEXECUTE));
        if((autoExec && autoExecFlag) || (!autoExec && !autoExecFlag))
        {
            _events->runEventAt(pos, dir, line);
        }
    }
}

bool XEEN::Maze::Map::canMove(const Common::Point& pos, Direction dir) const
{
    XEEN_VALID();

    const uint32 wallNoPass = _base->getValue(Segment::WALLNOPASS);
    if(_base->getMapFlags() & Segment::MAP_OUTDOORS)
    {
        const Common::Point dest = dir.move(pos, 0, 1);
        return true;
    }
    else
    {
        const uint32 tile = getTile(pos, dir);
        return (tile >> 12) < wallNoPass;
    }
}

bool XEEN::Maze::Map::tryBash(const Common::Point& pos, Direction dir)
{
    // TODO: Return whether or not damage should be take
    // TODO: Calculate strength and chance
    // TODO: Perfect checking on whether a bash is possible.
    // TODO: Handle indoor/outdoor differences.

    if(canMove(pos, dir))
    {
        return true;
    }
    else
    {
//      if(getFlags(pos) & Segment::GRATE)
//      {
            const uint32 tile = _base->getTile(pos, dir);
            if(((tile >> 12) & 0xF) == 9) // TODO: Magic 9=Wall type GRATE
            {
                _base->setWall(pos, dir, 3u); // TODO: 3=Bashed Wall

                dir.turnAround();
                _base->setWall(dir.move(pos, 0, -1), dir, 3u);
                return true;
            }
//      }

        return false;
    }
}

uint16 XEEN::Maze::Map::getTile(const Common::Point& pos, Direction dir) const
{
    XEEN_VALID();
    return _base->getTile(pos, dir);
}

void XEEN::Maze::Map::setTile(const Common::Point& pos, Direction dir, uint16 value)
{
    XEEN_VALID();
    _base->setTile(pos, dir, value);
}

uint8 XEEN::Maze::Map::getFlags(const Common::Point& pos) const
{
    XEEN_VALID();
    return _base->getCellFlags(pos);
}

void XEEN::Maze::Map::setFlags(const Common::Point& pos, uint8 value)
{
    XEEN_VALID();
    _base->setCellFlags(pos, value);
}

uint16 XEEN::Maze::Map::getSurface(const Common::Point& pos) const
{
    XEEN_VALID();

    const uint32 flags = getFlags(pos);

    if(_base->getMapFlags() & Segment::MAP_OUTDOORS)
    {
        const uint32 result = _base->lookupSurface(getTile(pos, 0) & 0xF);
        return ((result == 0) && (flags & Segment::WATER)) ? 8 : result;
    }
    else
    {
        return _base->lookupSurface(flags & 7);
    }
}

bool XEEN::Maze::Map::getObjectAt(const Common::Point& pos, ObjectEntry& data) const
{
    XEEN_VALID();
    return _objects->getObjectAt(pos, data);
}

void XEEN::Maze::Map::moveObject(uint32 id, const Common::Point& pos)
{
    XEEN_VALID();
    _objects->moveObject(id, pos);
}

void XEEN::Maze::Map::moveMonster(uint32 id, const Common::Point& pos, bool spawn)
{
    XEEN_VALID();
    _objects->moveMonster(id, pos, spawn);
}

void XEEN::Maze::Map::fillDrawStruct(const Common::Point& pos, Direction dir)
{
    XEEN_VALID();

    buildOutdoorDrawIndex();
    buildDrawIndex();

    // TODO: Check
    const bool facingFlip = (pos.x + pos.y + (dir.isAlongX() ? 1 : 0)) & 1;

    if(_base->getMapFlags() & Segment::MAP_OUTDOORS)
    {
        // ENVIRONMENT
        const CCFileId skysprite = CCFileId("SKY.SKY");
    
        outdoorDrawIndex[SKY_TOP]->sprite = skysprite;
        outdoorDrawIndex[SKY_BOTTOM]->sprite = skysprite;
    
        outdoorDrawIndex[SKY_TOP]->setFlipped(!dir.isAlongX());
        outdoorDrawIndex[SKY_BOTTOM]->setFlipped(!dir.isAlongX());
    
        outdoorDrawIndex[GROUND]->sprite = CCFileId("OUTDOOR.GND");
        outdoorDrawIndex[GROUND]->setFlipped(facingFlip);
    
        processSurface(pos, dir, facingFlip, outdoorDrawIndex);
        processObjects(pos, dir, outdoorDrawIndex);

        // WALLS
        static const uint16 wallmap[16] =
        {
            0xFFFF,
            CCFileId("MOUNT.WAL"), 
            CCFileId("LTREE.WAL"),
            CCFileId("DTREE.WAL"), 
            0xFFFF,
            CCFileId("SNOTREE.WAL"), //?
            CCFileId("DSNOTREE.WAL"),
            CCFileId("SNOMNT.WAL"),
            0xFFFF,
            0xFFFF,
            CCFileId("LAVAMNT.WAL"),
            CCFileId("PALM.WAL"),
            0xFFFF,
            CCFileId("DEDLTREE.WAL")
        };

        static const int wall0[3] = {FWALL_0_1L, FWALL_0_CEN, FWALL_0_1R};
        static const int wall1[3] = {FWALL_1_1L, FWALL_1_CEN, FWALL_1_1R};
        static const int wall2[3] = {FWALL_2_1L, FWALL_2_CEN, FWALL_2_1R};
        static const int wall3[5] = {FWALL_3_2L, FWALL_3_1L, FWALL_3_CEN, FWALL_3_1R, FWALL_3_2R};
        static const int wall4[9] = {FWALL_4_4L, FWALL_4_3L, FWALL_4_2L, FWALL_4_1L, FWALL_4_CEN,
                                     FWALL_4_1R, FWALL_4_2R, FWALL_4_3R, FWALL_4_4R};    
        static const int wallcount[5] = {3, 3, 3, 5, 9};
        static const int walloffset[5] = {1, 1, 1, 2, 4};
        static const int* const wallbase[] = {wall0, wall1, wall2, wall3, wall4};
        
        for(int i = 0; i != 5; i ++)
        {
            for(int j = 0; j != wallcount[i]; j ++)
            {
                Common::Point cell = dir.move(pos, j - walloffset[i], i);
                outdoorDrawIndex[wallbase[i][j]]->sprite = wallmap[(getTile(cell, 0) >> 4) & 0xF];
            }
        }

    }
    else
    {    
        // ENVIRONMENT
        const CCFileId skysprite = (getFlags(pos) & Segment::INSIDE) ? CCFileId("%s.SKY", _wallType) : CCFileId("SKY.SKY");
    
        indoorDrawIndex[SKY_TOP]->sprite = skysprite;
        indoorDrawIndex[SKY_BOTTOM]->sprite = skysprite;
    
        indoorDrawIndex[SKY_TOP]->setFlipped(!dir.isAlongX());
        indoorDrawIndex[SKY_BOTTOM]->setFlipped(!dir.isAlongX());
    
        indoorDrawIndex[GROUND]->sprite = CCFileId("%s.GND", _wallType);
        indoorDrawIndex[GROUND]->setFlipped(facingFlip);
        
        // SURFACE
        processSurface(pos, dir, facingFlip, indoorDrawIndex);
    
        // DISTANT WALL
        indoorDrawIndex[FWALL_DISTANT]->sprite = CCFileId("F%s1.FWL", _wallType);
    
        // FACING WALLS
        static const unsigned wallmap[16] = {32, 9, 17, 11, 8, 0, 15, 16, 0, 10, 14, 6, 1, 8, 12, 13};
        static const int wall1[3] = {FWALL_1_1L, FWALL_1_CEN, FWALL_1_1R};
        static const int wall2[3] = {FWALL_2_1L, FWALL_2_CEN, FWALL_2_1R};
        static const int wall3[5] = {FWALL_3_2L, FWALL_3_1L, FWALL_3_CEN, FWALL_3_1R, FWALL_3_2R};
        static const int wall4[9] = {FWALL_4_4L, FWALL_4_3L, FWALL_4_2L, FWALL_4_1L, FWALL_4_CEN,
                                     FWALL_4_1R, FWALL_4_2R, FWALL_4_3R, FWALL_4_4R};
    
        static const int baseframe[4] = {0, 0, 17, 0};
        static const int wallcount[4] = {3, 3, 5, 9};
        static const int walloffset[4] = {1, 1, 2, 4};
        static const int* const wallbase[] = {wall1, wall2, wall3, wall4};
        const uint16 fwlids[4] = {0, CCFileId("F%s3.FWL", _wallType), CCFileId("F%s3.FWL", _wallType), CCFileId("F%s4.FWL", _wallType)};
        
        // Nearest walls done separately to support being split between two sprites!
        for(int j = 0; j != wallcount[0]; j ++)
        {
            const int i = 0;
        
            Common::Point cell = dir.move(pos, j - walloffset[i], i);
            uint16 wallData = wallmap[_base->lookupWall(getTile(cell, dir) >> 12)];
            
            indoorDrawIndex[wallbase[i][j]]->sprite = (wallData != 32) ? (wallData < 8) ? CCFileId("F%s1.FWL", _wallType) : CCFileId("F%s2.FWL", _wallType) : CCFileId(0xFFFF);
            indoorDrawIndex[wallbase[i][j]]->frame = (wallData < 8) ? wallData : wallData - 8;
        }
        
        for(int i = 1; i != 4; i ++)
        {
            for(int j = 0; j != wallcount[i]; j ++)
            {
                Common::Point cell = dir.move(pos, j - walloffset[i], i);
                uint16 wallData = wallmap[getTile(cell, dir) >> 12];
                
                indoorDrawIndex[wallbase[i][j]]->sprite = (wallData != 32) ? fwlids[i] : 0xFFFF;
                indoorDrawIndex[wallbase[i][j]]->frame = baseframe[i] + ((wallData > 6) ? wallData - 1 : wallData);
            }
        }
        
        // SIDE WALLS
        static const int swl0_id[2] = {SWALL_0_1L, SWALL_0_1R};
        static const int swl1_id[2] = {SWALL_1_1L, SWALL_1_1R};
        static const int swl2_id[4] = {SWALL_2_2L, SWALL_2_1L, SWALL_2_1R, SWALL_2_2R};
        static const int swl3_id[8] = {SWALL_3_4L, SWALL_3_3L, SWALL_3_2L, SWALL_3_1L, 
                                       SWALL_3_1R, SWALL_3_2R, SWALL_3_3R, SWALL_3_4R};
        static const int swl4_id[8] = {SWALL_4_4L, SWALL_4_3L, SWALL_4_2L, SWALL_4_1L, 
                                       SWALL_4_1R, SWALL_4_2R, SWALL_4_3R, SWALL_4_4R};
    
        processSideWallList(pos, dir, 0, 2, swl0_id);
        processSideWallList(pos, dir, 1, 2, swl1_id);
        processSideWallList(pos, dir, 2, 4, swl2_id);
        processSideWallList(pos, dir, 3, 8, swl3_id);
        processSideWallList(pos, dir, 4, 8, swl4_id);
    
        // OBJECTS
        processObjects(pos, dir, indoorDrawIndex);
    }
}

void XEEN::Maze::Map::draw(Valid<Graphics::Manager> sprites)
{
    XEEN_VALID();

    if(_base->getMapFlags() & Segment::MAP_OUTDOORS)
    {
        for(int i = 0; i != sizeof(outdoorDrawList) / sizeof(outdoorDrawList[0]); i ++)
        {
            if(outdoorDrawList[i].sprite != 0xFFFF)
            {
                sprites->draw(outdoorDrawList[i].sprite, Common::Point(outdoorDrawList[i].x, outdoorDrawList[i].y), outdoorDrawList[i].frame, outdoorDrawList[i].flags & 0x8000, outdoorDrawList[i].scale);
            }
        }
    }
    else
    {
        for(int i = 0; i != sizeof(indoorDrawList) / sizeof(indoorDrawList[0]); i ++)
        {
            if(indoorDrawList[i].sprite != 0xFFFF)
            {
                sprites->draw(indoorDrawList[i].sprite, Common::Point(indoorDrawList[i].x, indoorDrawList[i].y), indoorDrawList[i].frame, indoorDrawList[i].flags & 0x8000, indoorDrawList[i].scale);
            }
        }
    }

    if(_message)
    {
        sprites->drawString(Common::Point(_messageArea.left, _messageArea.top), _message, _messageFlags, _messageArea.width());
    }
}

void XEEN::Maze::Map::drawMini(const Common::Point& pen, const Common::Point& pos, Direction dir, Valid<Graphics::Manager> sprites)
{
    XEEN_VALID();

    if(_base->getMapFlags() & Segment::MAP_OUTDOORS)
    {
        const CCFileId sprite = "OUTDOOR.TIL";

        for(int i = 0; i != 7; i ++)
        {
            for(int j = 0; j != 7; j ++)
            {
                const Common::Point tilepen = pen + Common::Point(j * 10, (6 - i) * 8);
                const uint16 tile = getTile(pos + Common::Point(j - 3, i - 3), 0);

                sprites->draw(sprite, tilepen, _base->lookupSurface(tile & 0xF));

                uint32 wall = _base->lookupWall((tile >> 4) & 0xF);
                if(wall)
                {
                    sprites->draw(sprite, tilepen, 16 + wall);
                }

                if(tile & 0xF00)
                {
                    sprites->draw(sprite, tilepen, 32 + ((tile >> 8) & 0xF));
                }

                // TODO: Overlay?
            }
        }
    }
    else
    {
        const CCFileId sprite("%s.TIL", _wallType);
    
        // Draw surface
        // TODO: The surface tile should be draw a few pixels lower, determine how much
        for(int i = 0; i != 7; i ++)
        {
            for(int j = 0; j != 7; j ++)
            {
                const uint32 sur = getSurface(pos + Common::Point(j - 3, i - 3));
                sprites->draw(sprite, pen + Common::Point(j * 10, (6 - i) * 8), sur ? 36 + sur : 0);
            }
        }
    
        // Draw walls
        for(int i = 0; i != 7; i ++)
        {
            for(int j = 0; j != 7; j ++)
            {
                const uint16 wallData = getTile(pos + Common::Point(j - 3, i - 3), 0);
    
                if(wallData != 0x8888)
                {
                    if(wallData & 0xF000)
                    {
                        sprites->draw(sprite, pen + Common::Point(j * 10, (6 - i) * 8), 2);
                    }
    
                    if(wallData & 0xF)
                    {
                        sprites->draw(sprite, pen + Common::Point(j * 10, (6 - i) * 8), 3);
                    }
                }
                else
                {
                    sprites->draw(sprite, pen + Common::Point(j * 10, (6 - i) * 8), 1);
                }
            }
        }
    }
}

void XEEN::Maze::Map::setSignMessage(const char* message, const Common::Rect& area, uint32 fontFlags)
{
    free(_message);
    _message = message ? strdup(message) : 0;
    _messageArea = area;
    _messageFlags = fontFlags;
}

/////
// List processing
/////
void XEEN::Maze::Map::processSurface(const Common::Point& pos, Direction dir, bool facingFlip, DrawListItem** index)
{
    static const CCFileId surfaceMap[16] = 
    {
        0xFFFF, "DIRT.SRF", "GRASS.SRF", "SNOW.SRF", "SWAMP.SRF", "LAVA.SRF", "DESERT.SRF", "ROAD.SRF",
        "WATER.SRF", "TFLR.SRF", "SKY.SRF", "CROAD.SRF", "SEWER.SRF", "CLOUD.SRF", "SCORTCH.SRF", "SPACE.SRF"
    };

    // TODO: Check
    static const uint8 flipTable[][25] = 
    {
        {18, 19, 20, 24, 23, 22, 21,    11, 12, 13, 17, 16, 15, 14,   6, 7, 10, 9, 8,   3, 5, 4,   0, 2, 1},
        {24, 23, 22, 18, 19, 20, 21,    17, 16, 15, 11, 12, 13, 14,   10, 9, 6, 7, 8,   5, 3, 4,   2, 0, 1}
    };

    static const int16 xoff3[3] = {-1, 1, 0};
    static const int16 xoff5[5] = {-2, -1, 2, 1, 0};
    static const int16 xoff7[7] = {-3, -2, -1, 3, 2, 1, 0};
    static const int16* const xoffsets[5] = {xoff7, xoff7, xoff5, xoff3, xoff3};
    static const uint32 linelength[5] = {7, 7, 5, 3, 3};

    int surfaceTile = SURF00;
    
    for(uint32 i = 0; i != 5; i ++)
    {
        for(uint32 j = 0; j != linelength[i]; j ++, surfaceTile ++)
        {
            Common::Point cell = dir.move(pos, xoffsets[i][j], 4 - i);
            index[surfaceTile]->sprite = surfaceMap[getSurface(cell)];
            index[surfaceTile]->frame = flipTable[facingFlip][surfaceTile - SURF00];
            index[surfaceTile]->setFlipped(facingFlip);
        }
    }
}

void XEEN::Maze::Map::processObjects(const Common::Point& pos, Direction dir, DrawListItem** index)
{
    // OBJECTS
    const ObjectData* const od = _parent->getObjectData();

    static const struct {uint32 id; int32 xOff; int32 yOff; } objOffsets[12] =
    {
        {OBJ_HERE, 0, 0}, {OBJ_1_1L, -1, 1}, {OBJ_1_CEN, 0, 1}, {OBJ_1_1R, 1, 1},
        {OBJ_2_1L, -1, 2}, {OBJ_2_CEN, 0, 2}, {OBJ_2_1R, 1, 2}, {OBJ_3_2L, -2, 3},
        {OBJ_3_1L, -1, 3}, {OBJ_3_CEN, 0, 3}, {OBJ_3_1R, 1, 3}, {OBJ_3_2R, 2, 3}
    };

    for(int i = 0; i != 12; i ++)
    {
        ObjectEntry t;
        if(getObjectAt(dir.move(pos, objOffsets[i].xOff, objOffsets[i].yOff), t))
        {
            index[objOffsets[i].id]->sprite = CCFileId("%03d.%sBJ", t.id, (t.id < 100) ? "O" : "0");
            const uint8* const data = od->getDataForObject(t.id);

            Direction facing = dir - t.dir;

            if(data)
            {
                index[objOffsets[i].id]->frame = data[facing];
                index[objOffsets[i].id]->flags &= ~0x8000;
                index[objOffsets[i].id]->flags |= data[4 + facing] ? 0x8000 : 0;
            }
        }
        else
        {
            index[objOffsets[i].id]->sprite = CCFileId(0xFFFF);
        }
    }
}

void XEEN::Maze::Map::processSideWallList(const Common::Point& pos, Direction dir, uint32 distance, uint32 count, NonNull<const int32> ids)
{
    uint32 midcount = count / 2;

    for(uint32 i = 0; i != count; i ++)
    {
        const int32 offset = (i < midcount) ? (0 - midcount) + i + 1 : i - midcount; // For count = 8: -3, -2, -1, 0, 0, 1, 2, 3

        const Common::Point cell = dir.move(pos, offset, distance);
        const uint16 wallData = (getTile(cell, dir) >> ((i >= midcount) ? 8 : 0)) & 0xF;

        indoorDrawIndex[ids[i]]->sprite = wallData ? CCFileId("S%s.SWL", _wallType) : CCFileId(0xFFFF);
        
        // TODO: Frame should alternate between odd-even tiles
        if(wallData == 2 && indoorDrawIndex[ids[i]]->frame < 24)
        {
            indoorDrawIndex[ids[i]]->frame += 24;
        }
        else if(wallData != 2 && indoorDrawIndex[ids[i]]->frame >= 24)
        {
            indoorDrawIndex[ids[i]]->frame -= 24;
        }
    }
}
