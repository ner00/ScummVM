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

#include "xeen/event/basicevents.h"
#include "xeen/ui/scrollwindow.h"
#include "xeen/game.h"
#include "xeen/party.h"
#include "xeen/characters.h"

namespace XEEN
{
    // TODO: This whole class is a stub
    class SpellSelectWindow : public ScrollWindow
    {
        public:
            SpellSelectWindow(Valid<Game> parent) : ScrollWindow(parent) { }

        protected:
            unsigned itemCount() const
            {
                XEEN_VALID();
                return 3;
            }

            void selectItem(unsigned item)
            {
                XEEN_VALID();
            }

            void fillItemText(unsigned item)
            {
                XEEN_VALID();
            
                static const char* const spellhack[] = {"Awaken", "First Aid", "Light"};
                if(item < 3)
                {
                    fillStringBuffer("%s", spellhack[item]);
                }
            }

            void fillHeader()
            {
                XEEN_VALID();
            
                Character* chara = getGame()->getActiveCharacter();
            
                if(valid(chara))
                {
                    fillStringBuffer("Spells for %s", chara->getName()); //TODO: Fill proper name
                }
            }

            void fillFooter()
            {
                XEEN_VALID();
            
                // TODO
                fillStringBuffer("Test");

            }

            void accept()
            {
                XEEN_VALID();
                _finished = true;
            }

            void cancel()
            {
                XEEN_VALID();
                _finished = true;
            }
    };
}


XEEN::Event::SelectSpell::SelectSpell(Valid<Game> parent) : Event(parent)
{
    addWindow(new SpellSelectWindow(parent));
}

void XEEN::Event::SelectSpell::process()
{
    setFinished(getWindows().back()->isFinished(), true);
}
