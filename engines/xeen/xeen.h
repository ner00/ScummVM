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

#ifndef XEEN_XEEN_H
#define XEEN_XEEN_H

#include "engines/engine.h"
#include "gui/debugger.h"

namespace XEEN {

class Console;

enum {
	kXeenDebugVerbose = 1 << 0
};


class XeenEngine : public Engine {
public:
    XeenEngine(OSystem *syst);
    ~XeenEngine();
    
	Common::Error run();

private:
	Console *_console;
};

class Console : public GUI::Debugger {
public:
    Console(XeenEngine *vm) { }
    virtual ~Console() { }
};

} // End of namespace XEEN

#endif // XEEN_XEEN_H
