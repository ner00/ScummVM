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

#ifndef XEEN_GAME_H
#define XEEN_GAME_H

#include "xeen/utility.h"

#include "xeen/party.h"
#include "xeen/event/event.h"

#include "xeen/graphics/manager.h"

#include "xeen/archive/file.h"

#include "xeen/maze/manager.h"

#include "xeen/ui/window.h"
#include "xeen/ui/basicwindows.h"
#include "xeen/ui/scrollwindow.h"

namespace XEEN
{
    class Archive;
    class Party;
    class Character;
    class Font;

    class Game : public Validateable_Cleanable, public Common::NonCopyable
    {
        public:
            enum Type { CLOUDS, DARKSIDE, WORLD };
    
        public:
            Game();
            ~Game();

        protected:
            void cleanse();
            
        public:
            void load();
            Type getGameType() const { return CLOUDS; }
        
            void click(const Common::Point& location);
            void key(Common::KeyCode key);
            void draw();

            Valid<Graphics::Manager> getGraphicsManager() const { return _graphicsManager; }
            Valid<Maze::Manager> getMapManager() const { return _mapManager; }
            Valid<Party> getParty() const { return _party; }

            FilePtr getFile(CCFileId id, bool fromSave = false);

            Character* getActiveCharacter();
            unsigned getActiveCharacterSlot() const { return _activeCharacterSlot; }
            void selectCharacter(unsigned slot) { _activeCharacterSlot = slot; }

            void setEvent(NonNull<Event::Event> event);

        private:
            Common::Stack<NonNull<Event::Event> > _events;

            unsigned _activeCharacterSlot;
                
        private:
            Archive* _assets;

            Graphics::Manager* _graphicsManager;
            Maze::Manager* _mapManager;
            Party* _party;
            Font* _font;

            CharacterWindow* _portraitWnd;
            GameWindow* _mainWnd;
            MovementWindow* _movementWnd;
    };
}

#endif // XEEN_GAME_H
