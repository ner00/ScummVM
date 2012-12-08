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

#ifndef XEEN_UI_SCROLLWINDOW_H
#define XEEN_UI_SCROLLWINDOW_H

#include "xeen/utility.h"
#include "xeen/ui/window.h"

namespace XEEN
{
    class ScrollWindow : public Window
    {
        public:
            ScrollWindow();
      
        protected:
            virtual unsigned itemCount() const = 0;
            virtual void selectItem(unsigned item) = 0;

            virtual void fillItemText(unsigned item) = 0;
            virtual void fillHeader() { }
            virtual void fillFooter() { }

            virtual void accept() { }
            virtual void cancel() { }

        private:
            unsigned _selected;

        protected:
            const Button* getButtons() const;
            void handleAction(unsigned id);

            const String* getStrings() const;
            void produceString(unsigned id);        
    };

    class SpellSelectWindow : public ScrollWindow
    {
        public:
            SpellSelectWindow();

        protected:
            unsigned itemCount() const;
            void selectItem(unsigned item);

            void fillItemText(unsigned item);
            void fillHeader();
            void fillFooter();

            void accept();
    };
}

#endif // XEEN_UI_SCROLLWINDOW_H
