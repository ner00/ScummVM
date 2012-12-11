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

#include "xeen/graphics/imagebuffer.h"
#include "xeen/graphics/spritemanager.h"
#include "xeen/graphics/sprite.h"

#include "xeen/maze/map.h"
#include "xeen/maze/drawlist_.h"
#include "xeen/maze/eventlist_.h"
#include "xeen/maze/text_.h"
#include "xeen/maze/segment_.h"

XEEN::Maze::Map::Map(uint16 mapNumber) : _base(0), _text(0), _events(0), _objects(0)
{
    _base = XEENgame.getMapManager()->getSegment(mapNumber);
 
    if(valid(_base))
    {
        // Load Maze Data
        _text = new Text(mapNumber);
        _events = new EventList(this, mapNumber);
        _objects = new Objects(mapNumber);

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
    XEEN_VALID_RET("");
    return valid(_text) ? _text->getString(id) : "";
}

void XEEN::Maze::Map::runEventAt(uint8 x, uint8 y, uint32 facing)
{
    XEEN_VALID();

    if(valid(_events))
    {
        _events->runEventAt(x, y, facing);
    }
}

uint16 XEEN::Maze::Map::getTile(Common::Point position, uint32 direction)
{
    Segment* seg = _base->resolveSegment(position);
    
    if(position.x < 0 || position.y < 0 || !seg)
    {
        return 0x8888;
    }
    else
    {
        uint16 result = seg->getWall(position.x, position.y);
    
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

uint16 XEEN::Maze::Map::getSurface(Common::Point position)
{
    Segment* seg = _base->resolveSegment(position);

    if(position.x < 0 || position.y < 0 || !seg)
    {
        return 0;
    }
    else
    {
        return _base->lookupSurface(seg->getCellFlags(position.x, position.y) & 7);
    }
}

bool XEEN::Maze::Map::getObjectAt(const Common::Point& position, ObjectEntry& data) const
{
    XEEN_VALID_RET(false);

    if(valid(_objects))
    {
        return _objects->getObjectAt(position.x, position.y, data);
    }

    return false;
}


void XEEN::Maze::Map::fillDrawStruct(Common::Point position, uint16 direction)
{
    buildDrawIndex();

    // ENVIRONMENT
    indoorDrawIndex[SKY_TOP]->sprite = CCFileId("SKY.SKY");
    indoorDrawIndex[SKY_BOTTOM]->sprite = CCFileId("SKY.SKY");
    indoorDrawIndex[GROUND]->sprite = CCFileId("TOWN.GND");
    
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
            Common::Point cell = translatePoint(position, xoffsets[i][j], 4 - i, direction);
            indoorDrawIndex[surfaceTile ++]->sprite = surfaceMap[getSurface(cell)];
        }
    }

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
    
        Common::Point cell = translatePoint(position, j - walloffset[i], i, direction);
        uint16 wallData = wallmap[getTile(cell, direction) >> 12];
        
        indoorDrawIndex[wallbase[i][j]]->sprite = (wallData != 32) ? (wallData < 8) ? CCFileId("FTOWN1.FWL") : CCFileId("FTOWN2.FWL") : CCFileId(0xFFFF);
        indoorDrawIndex[wallbase[i][j]]->frame = (wallData < 8) ? wallData : wallData - 8;
    }
    
    for(int i = 1; i != 4; i ++)
    {
        for(int j = 0; j != wallcount[i]; j ++)
        {
            Common::Point cell = translatePoint(position, j - walloffset[i], i, direction);
            uint16 wallData = wallmap[getTile(cell, direction) >> 12];
            
            indoorDrawIndex[wallbase[i][j]]->sprite = (wallData != 32) ? fwlids[i] : 0xFFFF;
            indoorDrawIndex[wallbase[i][j]]->frame = baseframe[i] + ((wallData > 6) ? wallData - 1 : wallData);
        }
    }
    
    // TODO: Distant wall?


    // SIDE WALLS: 0 Steps forward
    static const int swl0_xoffset[2] = {0, 0};
    static const int swl0_shift[2] = {0, 8};
    static const int swl0_id[2] = {SWALL_0_1L, SWALL_0_1R};
    
    for(int i = 0; i != 2; i ++)
    {
        Common::Point cell = translatePoint(position, swl0_xoffset[i], 0, direction);
        uint16 wallData = (getTile(cell, direction) >> swl0_shift[i]) & 0xF;
        indoorDrawIndex[swl0_id[i]]->sprite = wallData ? CCFileId("STOWN.SWL") : CCFileId(0xFFFF);
    }


    // SIDE WALLS: 1 Steps forward
    static const int swl1_xoffset[2] = {0, 0};
    static const int swl1_shift[2] = {0, 8};
    static const int swl1_id[2] = {SWALL_1_1L, SWALL_1_1R};
    
    for(int i = 0; i != 2; i ++)
    {
        Common::Point cell = translatePoint(position, swl1_xoffset[i], 1, direction);
        uint16 wallData = (getTile(cell, direction) >> swl1_shift[i]) & 0xF;
        indoorDrawIndex[swl1_id[i]]->sprite = wallData ? CCFileId("STOWN.SWL") : CCFileId(0xFFFF);
    }


    // SIDE WALLS: 2 Steps forward
    static const int swl2_xoffset[4] = {-1, 0, 0, 1};
    static const int swl2_shift[4] = {0, 0, 8, 8};
    static const int swl2_id[4] = {SWALL_2_2L, SWALL_2_1L, SWALL_2_1R, SWALL_2_2R};
    
    for(int i = 0; i != 4; i ++)
    {
        Common::Point cell = translatePoint(position, swl2_xoffset[i], 2, direction);
        uint16 wallData = (getTile(cell, direction) >> swl2_shift[i]) & 0xF;
        indoorDrawIndex[swl2_id[i]]->sprite = wallData ? CCFileId("STOWN.SWL") : CCFileId(0xFFFF);
    }

    // SIDE WALLS: 3 Steps forward
    static const int swl3_xoffset[8] = {-3, -2, -1, 0, 0, 1, 2, 3};
    static const int swl3_shift[8] = {0, 0, 0, 0, 8, 8, 8, 8};
    static const int swl3_id[8] = {SWALL_3_4L, SWALL_3_3L, SWALL_3_2L, SWALL_3_1L, 
                                   SWALL_3_1R, SWALL_3_2R, SWALL_3_3R, SWALL_3_4R};
    for(int i = 0; i != 8; i ++)
    {
        Common::Point cell = translatePoint(position, swl3_xoffset[i], 3, direction);
        uint16 wallData = (getTile(cell, direction) >> swl3_shift[i]) & 0xF;
        indoorDrawIndex[swl3_id[i]]->sprite = wallData ? CCFileId("STOWN.SWL") : CCFileId(0xFFFF);    
    }
    
    // OBJECTS
    ObjectEntry t;
    indoorDrawIndex[OBJ_HERE]->sprite = getObjectAt(position, t) ? CCFileId("%03d.OBJ", t.id) : CCFileId(0xFFFF);
    indoorDrawIndex[OBJ_1_1L]->sprite = getObjectAt(translatePoint(position, -1, 1, direction), t) ? CCFileId("%03d.OBJ", t.id) : CCFileId(0xFFFF);
    indoorDrawIndex[OBJ_1_CEN]->sprite = getObjectAt(translatePoint(position, 0, 1, direction), t) ? CCFileId("%03d.OBJ", t.id) : CCFileId(0xFFFF);
    indoorDrawIndex[OBJ_1_1R]->sprite = getObjectAt(translatePoint(position, 1, 1, direction), t) ? CCFileId("%03d.OBJ", t.id) : CCFileId(0xFFFF);        
}

void XEEN::Maze::Map::draw(ImageBuffer& out, SpriteManager& sprites)
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

Common::Point XEEN::Maze::Map::translatePoint(Common::Point position, int16 xOffset, int16 yOffset, uint16 direction)
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
