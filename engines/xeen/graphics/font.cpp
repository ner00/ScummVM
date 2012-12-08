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

#include "common/ptr.h"

#include "xeen/game.h"
#include "xeen/ccfile.h"
#include "xeen/utility.h"

#include "xeen/graphics/font.h"
#include "xeen/graphics/imagebuffer.h"

///
/// Font
///
XEEN::Font::Font()
{
    memset(_glyphs, 0, sizeof(_glyphs));
    
    //
    Common::ScopedPtr<CCFileData> reader(XEENgame.getAssets().getFile("FNT"));

    if(reader)
    {
        for(unsigned i = 0; i != CHARACTER_COUNT; i ++)
        {
            for(int y = 0; y != 8; y ++)
            {
                const uint16 line = reader->readUint16LE();
                
                for(int x = 0; x != 8; x ++)
                {
                    _glyphs[i].pixels[y * 8 + x] = (line >> (2 * x)) & 3;
                }
            }
        }
            
        for(unsigned i = 0; i != CHARACTER_COUNT; i ++)
        {
            _glyphs[i].spacing = reader->readByte();
        }
    }
    else
    {
        markInvalid();
    }
}

void XEEN::Font::drawString(ImageBuffer& out, Common::Point pen, const char* text, uint32 flags) const
{
    XEEN_VALID();

    const byte* btext = (const byte*)text;

    for(; *btext; btext ++)
    {
        const Glyph& ch = _glyphs[(*btext) + ((flags & SMALL) ? 128 : 0)];
        
        for(int i = 0; i != 8; i ++)
        {
            out.setPen(pen + Common::Point(0, i)).drawPixels_K_U_NF<0>(&ch.pixels[i * 8], 8);
        }
        
        pen.x += ch.spacing;
    }
}
