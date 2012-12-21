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

XEEN::Game::Game() : _activeCharacterSlot(0), _assets(0), _graphicsManager(0), _mapManager(0), _party(0),
                     _portraitWnd(0), _mainWnd(0), _movementWnd(0)
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

    XEEN_DELETE(_portraitWnd);
    XEEN_DELETE(_mainWnd);
    XEEN_DELETE(_movementWnd);
}

void XEEN::Game::load()
{
    markValid();

    _assets = new Archive("XEEN.CC");
    _graphicsManager = new Graphics::Manager(this);
    _mapManager = new Maze::Manager(this);
    _party = new Party(this);
    
    _portraitWnd = new CharacterWindow(this);
    _mainWnd = new GameWindow(this);
    _movementWnd = new MovementWindow(this);

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

namespace XEEN
{
    struct EvClickProcessor { void operator()(Valid<Window> wnd) { wnd->click(_loc); };  EvClickProcessor(const Common::Point& loc) : _loc(loc) {};  Common::Point _loc;};
    struct EvKeyProcessor   { void operator()(Valid<Window> wnd) { wnd->key(_key);   };  EvKeyProcessor(Common::KeyCode key) : _key(key) {};  Common::KeyCode _key;};
    struct EvDrawProcessor  { void operator()(Valid<Window> wnd) { wnd->draw();      };  EvDrawProcessor(uint32 u) { }; };
    struct EvHeartProcessor { void operator()(Valid<Window> wnd) { wnd->heartbeat(); };  EvHeartProcessor(uint32 u) { }; };
}

void XEEN::Game::click(const Common::Point& location)
{
    XEEN_VALID();

    if(!_events.empty())
    {
        _events.top()->processWindows<EvClickProcessor>(location);
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

    if(!_events.empty())
    {
        _events.top()->processWindows<EvKeyProcessor>(keycode);
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

    if(!_events.empty())
    {
        if(!_events.top()->isFinished())
        {
            _events.top()->process();
            _events.top()->processWindows<EvHeartProcessor>(0);
        }

        if(_events.top()->isFinished())
        {
            Event::Event* ev = _events.pop();
            if(ev->wantsDelete())
            {
                delete ev;
            }
        }
    }

    _portraitWnd->heartbeat();
    _mainWnd->heartbeat();
    _movementWnd->heartbeat();

    // Draw    
    _graphicsManager->reset();

    _mainWnd->draw();
    _movementWnd->draw();
    _portraitWnd->draw();

    Valid<Maze::Map> m = _party->getMap();
    m->fillDrawStruct(_party->getPosition(), _party->getValue(Party::MAZE_FACING));

    _graphicsManager->setClipArea(Common::Rect(8, 8, 224, 140));    
    m->draw(_graphicsManager);

    _graphicsManager->setClipArea(XRect::cr(237, 12, 70, 56));
    m->drawMini(Common::Point(237, 12), _party->getPosition(), _party->getValue(Party::MAZE_FACING), _graphicsManager);

    _graphicsManager->setClipArea(Common::Rect(0, 0, 320, 200));

    if(!_events.empty())
    {
        for(uint32 i = 0; i != _events.size(); i ++)
        {
            _events[i]->processWindows<EvDrawProcessor>(0);
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

void XEEN::Game::setEvent(NonNull<Event::Event> event)
{
    _events.push(event);
}
