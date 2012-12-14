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
#include "xeen/party.h"
#include "xeen/characters.h"

#include "xeen/archive/archive.h"

#include "xeen/graphics/font.h"
#include "xeen/graphics/imagebuffer.h"
#include "xeen/graphics/spritemanager.h"

#include "xeen/maze/map.h"

XEEN::Game XEENgame;

XEEN::Game::Game() : _windowID(NONE), _currentWindow(0), _activeCharacterSlot(0), _assets(0), _spriteManager(0),_mapManager(0), _party(0), _font(0)
{
    markInvalid();
}

XEEN::Game::~Game()
{
    cleanse();
}

void XEEN::Game::cleanse()
{
    XEEN_DELETE(_font);
    XEEN_DELETE(_party);
    XEEN_DELETE(_mapManager);
    XEEN_DELETE(_spriteManager);
    XEEN_DELETE(_assets);
}

void XEEN::Game::load()
{
    _assets = new Archive("XEEN.CC");
    _spriteManager = new SpriteManager();
    _mapManager = new Maze::Manager();
    _party = new Party();
    _font = new Font();
    
    _windowID = NONE;
    _currentWindow = 0;
    
    if(!(valid(_assets) && valid(_spriteManager) && valid(_mapManager) && valid(_party) && valid(_font)))
    {
        markInvalidAndClean();
        return;
    }
    
    markValid();

    //
    _mainWnd.show();
    _portraitWnd.show();

    // HACK: TODO Error check
    // Load palette
    {
        byte palette[256 * 3];
        
        FilePtr pal = _assets->getFile("MM4.PAL", false);
        for(int i = 0; i != 256 * 3; i ++)
        {
            palette[i] = pal->readByte() << 2;
        }
        
        g_system->getPaletteManager()->setPalette(palette, 0, 256);
    }

    // Load cursor
    {
        ImageBuffer mouseBuffer;
        byte mouseImage[16*16];

        _spriteManager->draw("MOUSE.ICN", mouseBuffer.clear(0), Common::Point(0, 0), 0);
        
        for(int i = 0; i != 16; i ++)
        {
            memcpy(&mouseImage[i * 16], &mouseBuffer.buffer[i * 320], 16);
        }
        
        g_system->setMouseCursor(mouseImage, 16, 16, 0, 0, 0);
        g_system->showMouse(true);
    }
}

void XEEN::Game::showWindow(WindowID id)
{
    _windowID = id;
    _currentWindow = 0;

    switch(id)
    {
        case STATUS: _currentWindow = &_statusWnd; break;
        case QUICKREF: _currentWindow = &_quickrefWnd; break;
        case CASTSPELL: _currentWindow = &_castWnd; break;
        case SELECTSPELL: _currentWindow = &_spellSelectWnd; break;
        case GAMEINFO: _currentWindow = &_gameInfoWnd; break;
        case CHARACTION: _currentWindow = &_charActionWnd; break;
    }

    if(_currentWindow)
    {
        _currentWindow->show();
    }
}

void XEEN::Game::click(const Common::Point& location)
{
    XEEN_VALID();

    if(_currentWindow)
    {
        _currentWindow->click(location);
        // TODO: Portraits if needed!
    }
    else
    {
        _portraitWnd.click(location);
        _mainWnd.click(location);
    }
}

void XEEN::Game::key(Common::KeyCode keycode)
{
    XEEN_VALID();

    if(_currentWindow)
    {
        _currentWindow->key(keycode);
        // TODO: Portraits if needed!
    }
    else
    {
        _portraitWnd.key(keycode);
        _mainWnd.key(keycode);
    }
}

void XEEN::Game::draw(ImageBuffer& out)
{   
    XEEN_VALID();

    _portraitWnd.heartbeat();
    _mainWnd.heartbeat();
    
    out.resetClipArea();
    _mainWnd.draw(out);
    _portraitWnd.draw(out);
    
    if(_currentWindow)
    {
        _currentWindow->heartbeat();

        if(_currentWindow)
        {
            _currentWindow->draw(out);
        }
    }
    else
    {
        out.setClipArea(Common::Rect(8, 8, 224, 140));    
        
        Maze::Map* m = _party->getMap();
        
        if(valid(m))
        {
           m->fillDrawStruct(_party->getPosition(), _party->getValue(Party::MAZE_FACING));
           m->draw(out, *_spriteManager); 
        }

        out.resetClipArea();
    }
}

XEEN::FilePtr XEEN::Game::getFile(CCFileId id, bool fromSave)
{
    assert(valid(_assets));
    return _assets->getFile(id, fromSave);
}

void XEEN::Game::drawString(ImageBuffer& out, Common::Point pen, const char* text, uint32 flags, unsigned width) const
{
    return _font->drawString(out, pen, text, flags, width);
}


XEEN::Character* XEEN::Game::getActiveCharacter()
{
    XEEN_VALID();
    
    return valid(_party) ? _party->getMemberInSlot(_activeCharacterSlot) : 0;
}
