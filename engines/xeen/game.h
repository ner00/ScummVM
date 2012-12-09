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

#include "common/scummsys.h"
#include "common/keyboard.h"

#include "xeen/utility.h"

#include "xeen/graphics/font.h"
#include "xeen/graphics/spritemanager.h"
#include "xeen/maze/map.h"
#include "xeen/party.h"

#include "xeen/archive/file.h"
#include "xeen/archive/archive.h"

#include "xeen/ui/window.h"
#include "xeen/ui/characteraction.h"
#include "xeen/ui/characterwindow.h"
#include "xeen/ui/gamewindow.h"
#include "xeen/ui/quickreference.h"
#include "xeen/ui/castwindow.h"
#include "xeen/ui/gameinfowindow.h"
#include "xeen/ui/scrollwindow.h"

namespace XEEN
{
    class ImageBuffer;
    class SpriteManager;
    class MapManager;
    class Party;
    class Character;
    class Font;

    class Game : public Validateable_Cleanable
    {
        public:
            enum WindowID { NONE, STATUS, QUICKREF, CASTSPELL, SELECTSPELL, GAMEINFO, CHARACTION, MAX_WINDOW_ID };
    
        public:
            Game();
            ~Game();

        protected:
            void cleanse();
            
        public:
            void load();
        
            void showWindow(WindowID id);
        
            void click(const Common::Point& location);
            void key(Common::KeyCode key);
            void draw(ImageBuffer& out);
        
            FilePtr getFile(CCFileId id, bool fromSave = false);

            Valid<SpriteManager> getSpriteManager() { return _spriteManager; }
            Valid<MapManager> getMapManager() { return _mapManager; }
            Valid<Party> getParty() { return _party; }
            Valid<Font> getFont() { return _font; }

            Character* getActiveCharacter();
            unsigned getActiveCharacterSlot() const { return _activeCharacterSlot; }
            void selectCharacter(unsigned slot) { _activeCharacterSlot = slot; }

            void movePartyTo(uint16 map, int16 x, int16 y, uint32 direction);
                
        private:
            WindowID _windowID;
            Window* _currentWindow;
            unsigned _activeCharacterSlot;
                
        private:
            Archive* _assets;

            SpriteManager* _spriteManager;
            MapManager* _mapManager;
            Party* _party;
            Font* _font;

            CharacterStatusWindow _statusWnd;
            CharacterWindow _portraitWnd;
            CharacterActionWindow _charActionWnd;
            GameWindow _mainWnd;
            QuickReferenceWindow _quickrefWnd;
            CastWindow _castWnd;
            GameInfoWindow _gameInfoWnd;
            SpellSelectWindow _spellSelectWnd;
    };
}

extern XEEN::Game XEENgame;

#endif // XEEN_GAME_H
