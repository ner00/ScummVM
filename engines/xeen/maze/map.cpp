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

XEEN::Maze::Map::Map(Valid<Manager> parent, uint16 mapNumber) : GameHolder(parent->getGame()), _parent(parent), _base(0), _text(0), _events(0), _objects(0)
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

void XEEN::Maze::Map::runEventAt(uint8 x, uint8 y, Direction facing)
{
    XEEN_VALID();

    if(valid(_events))
    {
        _events->runEventAt(x, y, facing);
    }
}

bool XEEN::Maze::Map::canMove(const Common::Point& position, Direction dir) const
{
    XEEN_VALID();

    const uint32 wallNoPass = _base->getValue(Segment::WALLNOPASS);
    const uint32 tile = getTile(position, dir);

    return (tile >> 12) < wallNoPass;
}

uint16 XEEN::Maze::Map::getTile(Common::Point position, Direction facing) const
{
    XEEN_VALID();

    Segment* seg = _base->resolveSegment(position);
    
    if(position.x < 0 || position.y < 0 || !seg)
    {
        return 0x8888;
    }
    else
    {
        uint16 result = seg->getWall(position.x, position.y);
    
        switch(facing)
        {
            case 0: return result;
            case 1: return (result >> 12) | (result << 4);
            case 2: return (result >> 8) | (result << 8);
            case 3: return (result >> 4) | (result << 12);
        }

        return result;
    }
}

uint8 XEEN::Maze::Map::getFlags(Common::Point position) const
{
    XEEN_VALID();

    Segment* seg = _base->resolveSegment(position);

    if(position.x < 0 || position.y < 0 || !seg)
    {
        return 0;
    }
    else
    {
        return seg->getCellFlags(position.x, position.y);
    }
}

uint16 XEEN::Maze::Map::getSurface(Common::Point position) const
{
    XEEN_VALID();

    uint32 flags = getFlags(position);

    if(flags & Segment::WATER)
    {
        return 8;
    }

    if(_base->getMapFlags() & Segment::MAP_OUTDOORS)
    {
        return _base->lookupSurface(getTile(position, 0) & 0xF);
    }
    else
    {
        return _base->lookupSurface(getFlags(position) & 7);
    }
}

bool XEEN::Maze::Map::getObjectAt(const Common::Point& position, ObjectEntry& data) const
{
    XEEN_VALID();

    if(valid(_objects))
    {
        return _objects->getObjectAt(position.x, position.y, data);
    }

    return false;
}

void XEEN::Maze::Map::fillDrawStruct(Common::Point position, Direction facing)
{
    XEEN_VALID();
    buildOutdoorDrawIndex();
    buildDrawIndex();

    const bool facingFlip = !(((position.x + position.y) & 1) && facing.isAlongX());

    if(_base->getMapFlags() & Segment::MAP_OUTDOORS)
    {
        // ENVIRONMENT
        const CCFileId skysprite = CCFileId("SKY.SKY");
    
        outdoorDrawIndex[SKY_TOP]->sprite = skysprite;
        outdoorDrawIndex[SKY_BOTTOM]->sprite = skysprite;
    
        outdoorDrawIndex[SKY_TOP]->setFlipped(!facing.isAlongX());
        outdoorDrawIndex[SKY_BOTTOM]->setFlipped(!facing.isAlongX());
    
        outdoorDrawIndex[GROUND]->sprite = CCFileId("TOWN.GND"); // TODO: Proper sprite?
        outdoorDrawIndex[GROUND]->setFlipped(facingFlip);
    
        processSurface(position, facing, outdoorDrawIndex);
        processObjects(position, facing, outdoorDrawIndex);

        // WALLS
        // TODO: Nearest walls
        static const uint16 wallmap[16] = {0xFFFF, CCFileId("MOUNT.WAL"), CCFileId("LTREE.WAL"), CCFileId("LAVAMNT.WAL"),
                                           CCFileId("DTREE.WAL"), CCFileId("DEDLTREE.WAL"), CCFileId("PALM.WAL"),
                                           CCFileId("SNOMNT.WAL"), CCFileId("SNOTREE.WAL")};
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
                Common::Point cell = facing.move(position, j - walloffset[i], i);
                outdoorDrawIndex[wallbase[i][j]]->sprite = wallmap[(getTile(cell, 0) >> 4) & 0xF];
            }
        }

    }
    else
    {    
        // ENVIRONMENT
        const CCFileId skysprite = (getFlags(position) & Segment::INSIDE) ? CCFileId("TOWN.SKY") : CCFileId("SKY.SKY");
    
        indoorDrawIndex[SKY_TOP]->sprite = skysprite;
        indoorDrawIndex[SKY_BOTTOM]->sprite = skysprite;
    
        indoorDrawIndex[SKY_TOP]->setFlipped(!facing.isAlongX());
        indoorDrawIndex[SKY_BOTTOM]->setFlipped(!facing.isAlongX());
    
        indoorDrawIndex[GROUND]->sprite = CCFileId("TOWN.GND");
        indoorDrawIndex[GROUND]->setFlipped(facingFlip);
        
        // SURFACE
        processSurface(position, facing, indoorDrawIndex);
    
        // DISTANT WALL
        indoorDrawIndex[FWALL_DISTANT]->sprite = CCFileId("FTOWN1.FWL");
    
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
        static const uint16 fwlids[4] = {0, CCFileId::fromString("FTOWN3.FWL"), CCFileId::fromString("FTOWN3.FWL"), CCFileId::fromString("FTOWN4.FWL")};
        
        // Nearest walls done separately to support being split between two sprites!
        for(int j = 0; j != wallcount[0]; j ++)
        {
            const int i = 0;
        
            Common::Point cell = facing.move(position, j - walloffset[i], i);
            uint16 wallData = wallmap[getTile(cell, facing) >> 12];
            
            indoorDrawIndex[wallbase[i][j]]->sprite = (wallData != 32) ? (wallData < 8) ? CCFileId("FTOWN1.FWL") : CCFileId("FTOWN2.FWL") : CCFileId(0xFFFF);
            indoorDrawIndex[wallbase[i][j]]->frame = (wallData < 8) ? wallData : wallData - 8;
        }
        
        for(int i = 1; i != 4; i ++)
        {
            for(int j = 0; j != wallcount[i]; j ++)
            {
                Common::Point cell = facing.move(position, j - walloffset[i], i);
                uint16 wallData = wallmap[getTile(cell, facing) >> 12];
                
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
    
        processSideWallList(position, facing, 0, 2, swl0_id);
        processSideWallList(position, facing, 1, 2, swl1_id);
        processSideWallList(position, facing, 2, 4, swl2_id);
        processSideWallList(position, facing, 3, 8, swl3_id);
        processSideWallList(position, facing, 4, 8, swl4_id);
    
        // OBJECTS
        processObjects(position, facing, indoorDrawIndex);
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
}

void XEEN::Maze::Map::drawMini(const Common::Point& pen, const Common::Point& position, Direction facing, Valid<Graphics::Manager> sprites)
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
                const uint16 tile = getTile(position + Common::Point(j - 3, i - 3), 0);

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
        const CCFileId sprite = "TOWN.TIL";
    
        // Draw surface
        // TODO: The surface tile should be draw a few pixels lower, determine how much
        for(int i = 0; i != 7; i ++)
        {
            for(int j = 0; j != 7; j ++)
            {
                const uint32 sur = getSurface(position + Common::Point(j - 3, i - 3));
                sprites->draw(sprite, pen + Common::Point(j * 10, (6 - i) * 8), sur ? 36 + sur : 0);
            }
        }
    
        // Draw walls
        for(int i = 0; i != 7; i ++)
        {
            for(int j = 0; j != 7; j ++)
            {
                const uint16 wallData = getTile(position + Common::Point(j - 3, i - 3), 0);
    
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

/////
// List processing
/////
void XEEN::Maze::Map::processSurface(const Common::Point& position, Direction facing, DrawListItem** index)
{
    // SURFACE
    static const CCFileId surfaceMap[16] = 
    {
        0xFFFF, "DIRT.SRF", "GRASS.SRF", "SNOW.SRF", "SWAMP.SRF", "LAVA.SRF", "DESERT.SRF", "ROAD.SRF",
        "WATER.SRF", "TFLR.SRF", "SKY.SRF", "CROAD.SRF", "SEWER.SRF", "CLOUD.SRF", "SCOORTCH.SRF", "SPACE.SRF"
    };    
    
    static const int16 xoff3[3] = {-1, 1, 0};
    static const int16 xoff5[5] = {-2, -1, 2, 1, 0};
    static const int16 xoff7[7] = {-3, -2, -1, 3, 2, 1, 0};
    static const int16* const xoffsets[5] = {xoff7, xoff7, xoff5, xoff3, xoff3};
    static const uint32 linelength[5] = {7, 7, 5, 3, 3};

    int surfaceTile = SURF00;
    
    for(uint32 i = 0; i != 5; i ++)
    {
        for(uint32 j = 0; j != linelength[i]; j ++)
        {
            Common::Point cell = facing.move(position, xoffsets[i][j], 4 - i);
            index[surfaceTile ++]->sprite = surfaceMap[getSurface(cell)];
        }
    }
}

void XEEN::Maze::Map::processObjects(const Common::Point& position, Direction facing, DrawListItem** index)
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
        if(getObjectAt(facing.move(position, objOffsets[i].xOff, objOffsets[i].yOff), t))
        {
            index[objOffsets[i].id]->sprite = CCFileId("%03d.%sBJ", t.id, (t.id < 100) ? "O" : "0");
            const uint8* const data = od->getDataForObject(t.id);

            uint32 dir = facing.relativeTo(t.facing).turnAround();

            if(enforce(data))
            {
                index[objOffsets[i].id]->frame = data[dir];
                index[objOffsets[i].id]->flags &= ~0x8000;
                index[objOffsets[i].id]->flags |= data[4 + dir] ? 0x8000 : 0;
            }
        }
        else
        {
            index[objOffsets[i].id]->sprite = CCFileId(0xFFFF);
        }
    }
}

void XEEN::Maze::Map::processSideWallList(const Common::Point& position, Direction facing, uint32 distance, uint32 count, NonNull<const int32> ids)
{
    uint32 midcount = count / 2;

    for(uint32 i = 0; i != count; i ++)
    {
        const int32 offset = (i < midcount) ? (0 - midcount) + i + 1 : i - midcount; // For count = 8: -3, -2, -1, 0, 0, 1, 2, 3

        const Common::Point cell = facing.move(position, offset, distance);
        const uint16 wallData = (getTile(cell, facing) >> ((i >= midcount) ? 8 : 0)) & 0xF;

        indoorDrawIndex[ids[i]]->sprite = wallData ? CCFileId("STOWN.SWL") : CCFileId(0xFFFF);
        
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
