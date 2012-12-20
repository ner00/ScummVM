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

#ifndef XEEN_EVENT_H
#define XEEN_EVENT_H

#include "xeen/utility.h"
#include "xeen/ui/window.h"

namespace XEEN
{
    namespace Event
    {
        class Event
        {
            public:
                Event(Valid<Game> parent) : _parent(parent) { }
                virtual ~Event() { }

                virtual bool process() = 0;
    
                virtual const Button* getCommandButtons() const { return 0; }
                virtual void handleAction(unsigned id) { }
    
                Common::List<Valid<Window> >& getWindows() { return _windows; }
                void addWindow(Valid<Window> window) { _windows.push_back(window); }
                void closeWindow() { _windows.pop_back(); }
    
            protected:
                Valid<Game> _parent;
                Common::List<Valid<Window> > _windows;
        };
    }
}

#endif // XEEN_EVENT_H
