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
#define XEEN_GRAPHICS_SOURCE

#include "xeen/game.h"

#include "xeen/graphics/sprite_.h"
#include "xeen/graphics/imagebuffer_.h"
#include "xeen/graphics/font_.h"
#include "xeen/graphics/manager.h"

XEEN::Graphics::Manager::Manager(Valid<Game> parent) : GameHolder(parent), _screen(new ImageBuffer()), _font(new Font(parent->getFile("FNT")))
{
    memset(_sprites, 0, sizeof(_sprites));
}

XEEN::Graphics::Manager::~Manager()
{
    for(unsigned i = 0; i != MAX_SPRITES; i ++)
    {
        delete _sprites[i];
    }

    delete _screen;
    delete _font;
}

void XEEN::Graphics::Manager::reset()
{
    XEEN_VALID();
    _screen->reset();
    _screen->clear(0x00);
}

void XEEN::Graphics::Manager::setClipArea(const Common::Rect& area)
{
    XEEN_VALID();
    _screen->setClipArea(area);
}


void XEEN::Graphics::Manager::fillRect(Common::Rect area, uint8 color)
{
    XEEN_VALID();

    _screen->fillRect(area, color);
}

void XEEN::Graphics::Manager::draw(const CCSpriteId& id, const Common::Point& pen)
{
    const uint64 time = g_system->getMillis() / 250;
    const uint32 count = id._count ? id._count : 1;

    if(!_sprites[id])
    {
        _sprites[id] = new Sprite(getGame()->getFile(id));
    }

    if(valid(_sprites[id]))
    {
        _sprites[id]->drawCell(_screen, pen, id._frame + (time % count), id._flip, id._scale);
    }
}

void XEEN::Graphics::Manager::drawString(Common::Point pen, const char* text, uint32 flags, uint32 width)
{
    XEEN_VALID();

    _font->drawString(_screen, pen, text, flags, width);
}

const byte* XEEN::Graphics::Manager::getScreenBitmap() const
{
    XEEN_VALID();

    return _screen->buffer;
}
