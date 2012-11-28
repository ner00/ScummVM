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
        
        Common::MemoryReadStream palFile = ccf.getFile("MM4.PAL");
        for(int i = 0; i != 256 * 3; i ++)
        {
            palette[i] = palFile.readByte() << 2;
        }
        
        _system->getPaletteManager()->setPalette(palette, 0, 256);
    }
    
    int32 monId = 0;
    int frame = 0;
    
    while(!shouldQuit())
    {
    	Common::Event event;    
    
        while(_eventMan->pollEvent(event))
        {
            switch(event.type)
            {
                case Common::EVENT_KEYDOWN:
                {   
                    if(event.kbd.keycode == Common::KEYCODE_LEFT)
                    {
                        monId --;
                    }
                    else if(event.kbd.keycode == Common::KEYCODE_RIGHT)
                    {
                        monId ++;
                    }
                    
                    if(monId < 0)
                    {
                        monId = 0;
                    }
                    
                    if(monId > 255)
                    {
                        monId = 255;
                    }
                }
            }
        }
        
        char buf[1024];
        sprintf(buf, "%03d.MON", monId);
        
        if(ccf.getEntry(buf))
        {
            XEEN::Sprite spriteTest(ccf, buf);   
            byte buffer[320 * 200];    
            memset(buffer, 0, 320 * 200);
            spriteTest.drawCell(buffer, frame, 10, 10);
            _system->copyRectToScreen(buffer, 320, 0, 0, 320, 200);
            _system->updateScreen();
        }

        _system->delayMillis(50);
        frame += 1;
        frame %= 8;
    }

    //

    return Common::kNoError;
}
