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
        markInvalid("Font file not found."); // TODO: Print name
    }
}

void XEEN::Graphics::Font::drawString(NonNull<ImageBuffer> out, Common::Point pen, NonNull<const char> text, uint32 flags, unsigned width) const
{
    XEEN_VALID();

    static const uint8 commandSize[16] = {0, 0, 0, 1, 3, 0, 0, 3, 1, 3, 0, 3, 2, 0, 0};

    // TODO: Check centering measurements against real game
    const byte* btext = (const byte*)(const char*)text;

    byte line[128];
    byte word[128];
    uint32 wordLength = 0;

    memset(line, 0, sizeof(line));
    memset(word, 0, sizeof(word));

    for(uint32 i = 0; btext[i]; i ++)
    {
        if(btext[i] <= 0x20)
        {
            if((measureString(line, flags) + measureString(word, flags)) > width)
            {
                drawLine(out, pen.x, pen.y, line, flags);

                pen.y += 10;
                memset(line, 0, sizeof(line));
            }

            word[wordLength] = 0x20;
            strcat((char*)line, (char*)word);
            memset(word, 0, sizeof(word));
            wordLength = 0;

            i += (btext[i] < 0x10) ? commandSize[btext[i]] : 0;
        }
        else
        {
            word[wordLength ++] = btext[i];
        }
    }

    if((measureString(line, flags) + measureString(word, flags)) > width)
    {
        drawLine(out, pen.x, pen.y, line, flags);
        drawLine(out, pen.x, pen.y + 10, word, flags);
    }
    else
    {
        strcat((char*)line, (char*)word);
        drawLine(out, pen.x, pen.y, line, flags);
    }
}

void XEEN::Graphics::Font::drawLine(NonNull<ImageBuffer> out, int32 x, int32 y, NonNull<const byte> text, uint32 flags) const
{
    const byte* btext = text;

    for(; *btext; btext ++)
    {
        const uint32 character = (flags & SMALL) ? *btext + 128 : *btext;
        const uint32 ascii = character & 0x7F;
        const uint32 spacing = _data->getByteAt(SPACING_OFFSET + character);
        uint32 offset = character * CHARACTER_SIZE;

        if(ascii > 0x20)
        {
            const uint32 descent = (ascii == 'g' || ascii == 'j' || ascii == 'p' || ascii == 'q' || ascii == 'y') ? 1 : 0;

            for(int i = 0; i != 8; i ++, offset += 2)
            {
                uint8 pixels[8];
                uint16 line = _data->getU16At(offset);
    
                for(int j = 0; j != 8; j ++, line >>= 2)
                {
                    pixels[j] = line & 3;
                }
                out->drawLine<0>(x, y + descent + i, 8, pixels, 0, false);
            }
        }
        
        x += spacing;
    }
}

unsigned XEEN::Graphics::Font::measureString(NonNull<const byte> text, uint32 flags) const
{
    XEEN_VALID();

    const byte* btext = text;
    uint32 result = 0;

    for(; *btext; btext ++)
    {
        uint32 character = (flags & SMALL) ? *btext + 128 : *btext;
        result += _data->getByteAt(SPACING_OFFSET + character);
    }

    return result;
}
