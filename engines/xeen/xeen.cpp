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

#include "engines/util.h"

#include "xeen/xeen.h"
#include "xeen/ccfile.h"
#include "xeen/map.h"
#include "xeen/characters.h"
#include "xeen/party.h"
#include "xeen/font.h"
#include "xeen/window.h"
#include "xeen/game.h"

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

    XEENgame.load();

    while(!shouldQuit())
    {
    	Common::Event event;    
    
        while(_eventMan->pollEvent(event))
        {
            switch(event.type)
            {
                case Common::EVENT_LBUTTONDOWN: XEENgame.click(event.mouse); break;
                case Common::EVENT_KEYDOWN: XEENgame.key(event.kbd.keycode); break;
                default: break;
            }
        }

        ImageBuffer buffer;        
        XEENgame.draw(buffer);
                
        _system->copyRectToScreen(buffer.buffer, 320, 0, 0, 320, 200);        
        _system->updateScreen();
    }

    //

    return Common::kNoError;
}
