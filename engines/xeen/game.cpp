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

#include "xeen/graphics/manager.h"

#include "xeen/maze/map.h"

XEEN::Game XEENgame;

XEEN::Game::Game() : _windowID(NONE), _currentWindow(0), _activeCharacterSlot(0), _assets(0), _graphicsManager(0), _mapManager(0), _party(0)
{
    markInvalid();
}

XEEN::Game::~Game()
{
    cleanse();
}

void XEEN::Game::cleanse()
{
    XEEN_DELETE(_party);
    XEEN_DELETE(_mapManager);
    XEEN_DELETE(_graphicsManager);
    XEEN_DELETE(_assets);
}

void XEEN::Game::load()
{
    _assets = new Archive("XEEN.CC");
    _graphicsManager = new Graphics::Manager();
    _mapManager = new Maze::Manager();
    _party = new Party();
    
    _windowID = NONE;
    _currentWindow = 0;
    
    if(!(valid(_assets) && valid(_graphicsManager) && valid(_mapManager) && valid(_party)))
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
        byte mouseImage[16*16];

        _graphicsManager->fillRect(Common::Rect(0, 0, 16, 16), 0);
        _graphicsManager->draw("MOUSE.ICN", Common::Point(0, 0), 0);

        const byte* buf = _graphicsManager->getScreenBitmap();
        
        for(int i = 0; i != 16; i ++)
        {
            memcpy(&mouseImage[i * 16], &buf[i * 320], 16);
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

void XEEN::Game::draw()
{   
    XEEN_VALID();

    _portraitWnd.heartbeat();
    _mainWnd.heartbeat();
    
    _graphicsManager->reset();
    _mainWnd.draw();
    _portraitWnd.draw();
    
    if(_currentWindow)
    {
        _currentWindow->heartbeat();

        if(_currentWindow)
        {
            _currentWindow->draw();
        }
    }
    else
    {        
        Maze::Map* m = _party->getMap();
        
        if(valid(m))
        {
            m->fillDrawStruct(_party->getPosition(), _party->getValue(Party::MAZE_FACING));

            _graphicsManager->setClipArea(Common::Rect(8, 8, 224, 140));    
            m->draw(_graphicsManager);

            _graphicsManager->setClipArea(XRect::cr(237, 12, 70, 56));
            m->drawMini(Common::Point(237, 12), _party->getPosition(), _party->getValue(Party::MAZE_FACING), _graphicsManager);
        }
    }
}

XEEN::FilePtr XEEN::Game::getFile(CCFileId id, bool fromSave)
{
    assert(valid(_assets));
    return _assets->getFile(id, fromSave);
}

XEEN::Character* XEEN::Game::getActiveCharacter()
{
    XEEN_VALID();
    
    return valid(_party) ? _party->getMemberInSlot(_activeCharacterSlot) : 0;
}
