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
#include "xeen/window.h"

namespace XEEN
{
    class ImageBuffer;
    class CCFile;
    class SpriteManager;
    class MapManager;
    class CharacterManager;
    class Party;
    class Font;

    class Game
    {
        public:
            Game();
            ~Game();
        
            void load();
        
            void draw(ImageBuffer& out);
        
            CCFile& getAssets() { enforce(_assets); return *_assets; }
            SpriteManager& getSpriteManager() { enforce(_spriteManager); return *_spriteManager; }
            MapManager& getMapManager() { enforce(_mapManager); return *_mapManager; }
            CharacterManager& getCharacterManager() { enforce(_characterManager); return *_characterManager; }
            Party& getParty() { enforce(_party); return *_party; }
            Font& getFont() { enforce(_font); return *_font; }

            void movePartyTo(uint16 map, int16 x, int16 y, uint32 direction);
        
        private:
            CCFile* _assets;

            SpriteManager* _spriteManager;
            MapManager* _mapManager;
            CharacterManager* _characterManager;
            Party* _party;
            Font* _font;

            CharacterStatusWindow _statusWnd;
            CharacterPortraitWindow _portraitWnd;
            MainIconWindow _commandsWnd;

    };
}

extern XEEN::Game XEENgame;

#endif // XEEN_GAME_H
