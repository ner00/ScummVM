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
 
#include "common/system.h"
#include "graphics/palette.h"

#include "xeen/game.h"
#include "xeen/imagebuffer.h"
#include "xeen/ccfile.h"
#include "xeen/map.h"
#include "xeen/sprite.h"
#include "xeen/party.h"
#include "xeen/characters.h"
#include "xeen/font.h"


XEEN::Game XEENgame;

XEEN::Game::Game() : _assets(0), _spriteManager(0),_mapManager(0), _characterManager(0), _party(0), _font(0)
{
}

XEEN::Game::~Game()
{
    delete _font;
    delete _party;
    delete _characterManager;
    delete _mapManager;
    delete _spriteManager;
    delete _assets;
}

void XEEN::Game::load()
{
    _assets = new CCFile("XEEN.CC");
    _spriteManager = new SpriteManager();
    _mapManager = new MapManager();
    _characterManager = new CharacterManager();
    _party = new Party();
    _font = new Font();
    
    // Load palette
    {
        byte palette[256 * 3];
        
        CCFileData* pal = _assets->getFile("MM4.PAL");
        for(int i = 0; i != 256 * 3; i ++)
        {
            palette[i] = pal->readByte() << 2;
        }
        delete pal;
        
        g_system->getPaletteManager()->setPalette(palette, 0, 256);
    }

    // Load cursor
    {
        ImageBuffer mouseBuffer;
        byte mouseImage[16*16];

        Sprite* cursor = _spriteManager->getSprite("MOUSE.ICN");        
        cursor->drawCell(mouseBuffer.clear(0), Common::Point(0, 0), 0);
        
        for(int i = 0; i != 16; i ++)
        {
            memcpy(&mouseImage[i * 16], &mouseBuffer.buffer[i * 320], 16);
        }
        
        g_system->setMouseCursor(mouseImage, 16, 16, 0, 0, 0);
        g_system->showMouse(true);
    }
}

void XEEN::Game::draw(ImageBuffer& out)
{   
    if(true) // HACK
    {
        Map* m = _mapManager->getMap(_party->mazeID);
        if(enforce(m))
        {
            m->fillDrawStruct(Common::Point(_party->xPosition, _party->yPosition), _party->facing & 3);
            m->draw(out, *_spriteManager);
        }

        out.resetClipArea();
        _commandsWnd.draw(out);
    }
    else
    {
        _statusWnd.heartbeat();
    
        _commandsWnd.draw(out);
    
        _statusWnd.draw(out);
    }
    
    out.resetClipArea();
    _portraitWnd.draw(out);
}

void XEEN::Game::movePartyTo(uint16 map, int16 x, int16 y, uint32 direction)
{
    _party->mazeID = map;
    _party->xPosition = x;
    _party->yPosition = y;
    _party->facing = direction;
}

