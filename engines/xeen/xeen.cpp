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

#include "common/error.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/memstream.h"
#include "common/events.h"
#include "common/system.h"
#include "graphics/palette.h"

#include "engines/util.h"

#include "xeen/xeen.h"
#include "xeen/ccfile.h"
#include "xeen/sprite.h"
#include "xeen/map.h"
#include "xeen/characters.h"
#include "xeen/party.h"
#include "xeen/font.h"
#include "xeen/window.h"

#include "xeen/imagebuffer.h"

XEEN::XeenEngine::XeenEngine(OSystem* syst) : Engine(syst), _console(0)
{
	DebugMan.addDebugChannel(kXeenDebugVerbose, "verbose", "verbose");
}

XEEN::XeenEngine::~XeenEngine()
{
    delete _console;
	DebugMan.clearAllDebugChannels();
}

Common::Error XEEN::XeenEngine::run()
{
	initGraphics(320, 200, false);

	_console = new Console(this);

    //

    XEEN::CCFile ccf("XEEN.CC");    

    // Load palette
    {
        byte palette[256 * 3];
        
        CCFileData* pal = ccf.getFile("MM4.PAL");
        for(int i = 0; i != 256 * 3; i ++)
        {
            palette[i] = pal->readByte() << 2;
        }
        delete pal;
        
        _system->getPaletteManager()->setPalette(palette, 0, 256);
    }

    // Load cursor
    {
        ImageBuffer mouseBuffer;
        byte mouseImage[16*16];

        Sprite* cursor = ccf.getSpriteManager().getSprite("MOUSE.ICN");        
        cursor->drawCell(mouseBuffer.clear(0), Common::Point(0, 0), 0);
        
        for(int i = 0; i != 16; i ++)
        {
            memcpy(&mouseImage[i * 16], &mouseBuffer.buffer[i * 320], 16);
        }
        
        _system->setMouseCursor(mouseImage, 16, 16, 0, 0, 0);
        _system->showMouse(true);
    }

    Party& party = ccf.getParty();
    Map* testMap = ccf.getMapManager().getMap(party.mazeID);
            
    Common::Point loc(party.xPosition, party.yPosition);
    int16 dir = party.facing;
            
    CCFileData* file = ccf.getFile("BACK.RAW");
         
    bool showMenu = false;
    
    CharacterStatusWindow window;
    CharacterPortraitWindow ports;
    MainIconWindow commands;
                
    while(!shouldQuit())
    {
    	Common::Event event;    
    
        while(_eventMan->pollEvent(event))
        {
            switch(event.type)
            {
                case Common::EVENT_LBUTTONDOWN:
                {
                    if(showMenu)
                    {
                        window.click(event.mouse);
                    }
                    
                    break;
                }
            
                case Common::EVENT_KEYDOWN:
                {
                    if(event.kbd.keycode == Common::KEYCODE_TAB) showMenu = !showMenu;                
                
                    if(!showMenu)
                    {
                        if(event.kbd.keycode == Common::KEYCODE_UP) loc = Map::translatePoint(loc, 0, 1, dir&3);
                        if(event.kbd.keycode == Common::KEYCODE_DOWN) loc = Map::translatePoint(loc, 0, -1, dir&3);
                        if(event.kbd.keycode == Common::KEYCODE_LEFT) dir --;
                        if(event.kbd.keycode == Common::KEYCODE_RIGHT) dir ++;                    
    
                        if(dir < 0) dir = 3;
                        if(dir > 3) dir = 0;
                    }
                }
            
                default:
                {
                    break;
                }
            }
        }

        ImageBuffer buffer;
        testMap->fillDrawStruct(loc, dir&3);
        
        memcpy(buffer.buffer, file->getData(), 320 * 200);
        
        
        if(!showMenu)
        {
            buffer.setClipArea(Common::Rect(8, 8, 224, 140));
            testMap->draw(buffer, ccf.getSpriteManager());
            buffer.resetClipArea();

            commands.draw(buffer, ccf);
        }
        else
        {
            window.heartbeat();
        
            commands.draw(buffer, ccf);
        
            window.draw(buffer, ccf);
        }
        
        buffer.resetClipArea();
        ports.draw(buffer, ccf);
                
        _system->copyRectToScreen(buffer.buffer, 320, 0, 0, 320, 200);        
        _system->updateScreen();
    }

    //

    return Common::kNoError;
}
