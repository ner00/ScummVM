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

#include "xeen/graphics/font_.h"
#include "xeen/graphics/imagebuffer_.h"

static const unsigned CHARACTER_COUNT = 256;
static const unsigned CHARACTER_SIZE = 16;
static const unsigned SPACING_OFFSET = 0x1000;


///
/// Font
///
XEEN::Graphics::Font::Font(FilePtr data) : _data(data)
{
    if(!valid(_data))
    {
        _data.reset();
        markInvalid();
    }
}

void XEEN::Graphics::Font::drawString(NonNull<ImageBuffer> out, Common::Point pen, const char* text, uint32 flags, unsigned width) const
{
    XEEN_VALID();

    // TODO: Check centering measurements against real game
    const byte* btext = (const byte*)text;
    const unsigned size = measureString(text, flags);

    if(flags & CENTER)
    {
        pen.x += (width - size) / 2;
    }
    else if(flags & ALIGN_RIGHT)
    {
        //HACK: Account for right side of window
        pen.x += (width - 8) - size;
    }

    for(; *btext; btext ++)
    {
        uint32 character = (flags & SMALL) ? *btext + 128 : *btext;
        uint32 spacing = _data->getByteAt(SPACING_OFFSET + character);
        uint32 offset = character * CHARACTER_SIZE;

        for(int i = 0; i != 8; i ++, offset += 2)
        {
            out->setPen(pen + Common::Point(0, i));
            uint16 line = _data->getU16At(offset);

            for(int j = 0; j != 8 && line; j ++, line >>= 2)
            {
                if(line & 3)
                {
                    out->putPixel(line & 3);
                }
                else
                {
                    out->movePen(Common::Point(1, 0));
                }
            }
        }
        
        pen.x += spacing;
    }
}

unsigned XEEN::Graphics::Font::measureString(const char* text, uint32 flags) const
{
    XEEN_VALID();

    const byte* btext = (const byte*)text;
    uint32 result = 0;

    for(; *btext; btext ++)
    {
        uint32 character = (flags & SMALL) ? *btext + 128 : *btext;
        result += _data->getByteAt(SPACING_OFFSET + character);
    }

    return result;
}
