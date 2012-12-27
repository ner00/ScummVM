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

#include "xeen/graphics/imagebuffer_.h"
#include "xeen/graphics/sprite_.h"

XEEN::Graphics::Sprite::Sprite(FilePtr file) : _file(file)
{
    if(!(valid(_file) && _file->getSize() >= 2))
    {
        markInvalid("Sprite file not found or invalid"); // TODO: Details
    }
}

void XEEN::Graphics::Sprite::drawCell(NonNull<ImageBuffer> out, const Common::Point& pen, uint16 frame, bool flip, uint32 scale)
{
    XEEN_VALID();

    const uint16 cellCount = _file->getU16At(0);

    if(enforce(frame < cellCount))
    {
        drawFrame(out, _file->getU16At(2 + (4 * frame) + 0), pen.x, pen.y, flip, scale);
        drawFrame(out, _file->getU16At(2 + (4 * frame) + 2), pen.x, pen.y, flip, scale);
    }
}

void XEEN::Graphics::Sprite::drawFrame(NonNull<ImageBuffer> out, uint32 offset, int32 x, int32 y, bool flip, uint32 scale)
{
    XEEN_VALID();

    if(offset && offset < _file->getSize())
    {
        _file->seek(offset);

        // Read the frame header
        const int16 penX = _file->readSint16LE();
        const uint16 frameWidth = _file->readUint16LE();
        const int16 penY = _file->readSint16LE();
        const uint16 frameHeight = _file->readUint16LE();
    
        if(scale == 0x8000) // Double size
        {
            y += penY * 2;
    
            for(uint32 onLine = 0; onLine != frameHeight; )
            {
                byte line[320];
                const uint32 linesDrawn = drawLine(line);
    
                out->drawLine<0>(x, y + onLine * 2, penX + frameWidth, line, 0x8000, flip);
                out->drawLine<0>(x, y + onLine * 2 + 1, penX + frameWidth, line, 0x8000, flip);
    
                onLine += linesDrawn;
                assert((linesDrawn > 0) && (onLine <= frameHeight)); // < Sanity
            }
        }
        else
        {
            const uint32 scaledWidth = out->scaleSize(penX + frameWidth, scale);
        
            uint32 yoff = 0;
            for(int16 i = 0; i != penY; i ++)
            {
                yoff += (out->checkScale(i, scale)) ? 1 : 0;
            }
        
            // Draw the lines
            for(uint32 onLine = 0; onLine != frameHeight; )
            {
                uint8 line[320];
                const uint32 linesDrawn = drawLine(line);
        
                if(out->checkScale(penY + onLine, scale))
                {
                    out->drawLine<0>(x + (penX + frameWidth - scaledWidth) / 2, y + yoff, penX + frameWidth, line, scale, flip);
                }
        
                for(uint32 i = 0; i != linesDrawn; i ++)
                {
                    yoff += (out->checkScale(penY + onLine + i, scale)) ? 1 : 0;
                }
        
                onLine += linesDrawn;
        
                // Check sanity: At least one line must have been drawn, and No more than 
                // frameHeight lines may have been drawn.
                assert((linesDrawn > 0) && (onLine <= frameHeight));
            }
        }
    }
}

uint32 XEEN::Graphics::Sprite::drawLine(NonNull<uint8> out)
{
    XEEN_VALID();

    uint8* pen = out;

    uint8 bytes = _file->readByte();
    int16 x = _file->readByte();
    
    // Skip a number or vertical lines
    if(bytes == 0)
    {
        return x + 1;
    }
    
    // Draw
    memset(pen, 0, 320);
    pen += x;

    int32 end = (_file->pos() - 1) + bytes;
    
    while(_file->pos() != end)
    {
        // Decode operation
        const uint8 controlByte = _file->readByte();
        const uint8 opcode = controlByte >> 5;
        const uint8 length = controlByte & 0x1F;
        
        switch(opcode)
        {
            case 0: case 1:
            {
                _file->read(pen, (controlByte & 0x3F) + 1);
                pen += (controlByte & 0x3F) + 1;
                break;
            }
            
            case 2:
            {
                const uint8 color = _file->readByte();
                memset(pen, color, length + 3);
                pen += length + 3;
                break;
            }
            
            case 3:
            {
                const uint16 backTrack = _file->readUint16LE();
                const int32 streamPos = _file->pos();
                
                _file->seek(_file->pos() - backTrack);
                _file->read(pen, length + 4);
                _file->seek(streamPos);

                pen += length + 4;
                break;
            }
            
            case 4:
            {
                const uint8 color1 = _file->readByte();
                const uint8 color2 = _file->readByte();
                
                for(int i = 0; i != length + 2; i ++)
                {
                    *pen++ = color1;
                    *pen++ = color2;
                }
                
                break;
            }
            
            case 5:
            {
                memset(pen, 0, length + 1);
                pen += length + 1;
                break;
            }
            
            case 6: case 7:
            {
                static const int8 pattern[] = {0, 1, 1, 1, 2, 2, 3, 3, 0, -1, -1, -1, -2, -2, -3, -3};
                const int8* const change = &pattern[(controlByte >> 2) & 0x0E];
                const uint32 count = (controlByte & 0x7) + 3;

                uint8 color = _file->readByte();
                
                for(uint32 i = 0; i != count; color += change[i & 1], i ++)
                {
                    *pen++ = color;
                }
                
                break;
            }
        }
        
        // Sanity: Exactly the specified number of bytes must be used.
        assert(_file->pos() <= end);
    }
    
    return 1;
}
