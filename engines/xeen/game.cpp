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

XEEN::Game::Game() : _windowDepth(0), _event(0), _activeCharacterSlot(0), _assets(0), _graphicsManager(0), _mapManager(0), _party(0),
                     _statusWnd(0), _portraitWnd(0), _charActionWnd(0), _mainWnd(0), _movementWnd(0), _quickrefWnd(0),
                     _castWnd(0), _spellSelectWnd(0)
{
    memset(_windowID, 0, sizeof(_windowID));
    memset(_windowStack, 0, sizeof(_windowStack));
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
    markValid();

    _assets = new Archive("XEEN.CC");
    _graphicsManager = new Graphics::Manager(this);
    _mapManager = new Maze::Manager(this);
    _party = new Party(this);

    memset(_windowID, 0, sizeof(_windowID));
    memset(_windowStack, 0, sizeof(_windowStack));
    _windowDepth = 0;
    
    _statusWnd = new CharacterStatusWindow(this);
    _portraitWnd = new CharacterWindow(this);
    _charActionWnd = new CharacterActionWindow(this);
    _mainWnd = new GameWindow(this);
    _movementWnd = new MovementWindow(this);
    _quickrefWnd = new QuickReferenceWindow(this);
    _castWnd = new CastWindow(this);
    _spellSelectWnd = new SpellSelectWindow(this);

    if(!(valid(_assets) && valid(_graphicsManager) && valid(_mapManager) && valid(_party)))
    {
        markInvalidAndClean();
        return;
    }

    //
    _mainWnd->show();
    _movementWnd->show();
    _portraitWnd->show();

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
    //HACK
    if(id == NONE)
    {
        closeWindow();
        return;
    }

    if(enforce(_windowDepth < 16) && enforce(id < MAX_WINDOW_ID))
    {
        _windowID[_windowDepth] = id;
    
        switch(id)
        {
            case STATUS: _windowStack[_windowDepth] = _statusWnd; break;
            case QUICKREF: _windowStack[_windowDepth] = _quickrefWnd; break;
            case CASTSPELL: _windowStack[_windowDepth] = _castWnd; break;
            case SELECTSPELL: _windowStack[_windowDepth] = _spellSelectWnd; break;
            case CHARACTION: _windowStack[_windowDepth] = _charActionWnd; break;
            default: enforce(false);
        }
    
        _windowStack[_windowDepth]->show();
        _windowDepth ++;
    }
}

void XEEN::Game::showWindow(Valid<Window> window)
{
    if(enforce(_windowDepth < 16))
    {
        _windowID[_windowDepth] = TEMP;
        _windowStack[_windowDepth] = window;
        _windowDepth ++;
    }
}

void XEEN::Game::closeWindow()
{
    if(_windowDepth)
    {
        _windowDepth --;

        if(_windowID[_windowDepth] == TEMP)
        {
            delete _windowStack[_windowDepth];
        }

        _windowID[_windowDepth] = NONE;
        _windowStack[_windowDepth] = 0;
    }
}


void XEEN::Game::click(const Common::Point& location)
{
    XEEN_VALID();

    if(_windowDepth)
    {
        _windowStack[_windowDepth - 1]->click(location);
        // TODO: Portraits if needed!
    }
    else
    {
        _portraitWnd->click(location);
        _mainWnd->click(location);
        _movementWnd->click(location);
    }
}

void XEEN::Game::key(Common::KeyCode keycode)
{
    XEEN_VALID();

    if(_windowDepth)
    {
        _windowStack[_windowDepth - 1]->key(keycode);
        // TODO: Portraits if needed!
    }
    else
    {
        if(keycode == Common::KEYCODE_SPACE)
        {
            Valid<Maze::Map> map = _party->getMap();
            const Common::Point pos = _party->getPosition();

            map->runEventAt(pos.x, pos.y, _party->getFacing());
        }

        _portraitWnd->key(keycode);
        _mainWnd->key(keycode);
        _movementWnd->key(keycode);
    }
}

void XEEN::Game::draw()
{   
    XEEN_VALID();

    _portraitWnd->heartbeat();
    _mainWnd->heartbeat();
    _movementWnd->heartbeat();
    
    _graphicsManager->reset();
    _mainWnd->draw();
    _movementWnd->draw();
    _portraitWnd->draw();
    
    Maze::Map* m = _party->getMap();
    m->fillDrawStruct(_party->getPosition(), _party->getValue(Party::MAZE_FACING));

    if(_windowDepth)
    {
        for(int i = _windowDepth - 1; i != -1; i --)
        {
            _windowStack[i]->heartbeat();
            _windowStack[i]->draw();
        }
    }
    else
    {                
        if(valid(m))
        {
            _graphicsManager->setClipArea(Common::Rect(8, 8, 224, 140));    
            m->draw(_graphicsManager);

            _graphicsManager->setClipArea(XRect::cr(237, 12, 70, 56));
            m->drawMini(Common::Point(237, 12), _party->getPosition(), _party->getValue(Party::MAZE_FACING), _graphicsManager);

            _graphicsManager->setClipArea(Common::Rect(0, 0, 320, 200));
        }
    }

    if(_event)
    {
        for(Common::List<Valid<Window> >::iterator i = _event->getWindows().begin(); i != _event->getWindows().end(); i ++)
        {
            (*i)->draw();
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

void XEEN::Game::setEvent(Event::Event* event)
{
    _event = event;
}
