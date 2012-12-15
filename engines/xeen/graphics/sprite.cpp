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

XEEN::Graphics::Sprite::Sprite(FilePtr file) : _file(file), _cellCount(0), _cells(0)
{
    if(_file)
    {
        _cellCount = _file->readUint16LE();
        
        if(_cellCount)
        {
            _cells = new Cell[_cellCount];
            
            if(enforce(_cells))
            {  
                for(unsigned i = 0; i != _cellCount; i ++)
                {
                    _cells[i].offset[0] = _file->readUint16LE();
                    _cells[i].offset[1] = _file->readUint16LE();
                }
                
                return;
            }
        }
    }

    markInvalidAndClean();
}

XEEN::Graphics::Sprite::~Sprite()
{
    cleanse();
}

void XEEN::Graphics::Sprite::cleanse()
{
    XEEN_DELETE_ARRAY(_cells);
    _cellCount = 0;
}

void XEEN::Graphics::Sprite::drawCell(NonNull<ImageBuffer> out, const Common::Point& pen, uint16 frame, bool flip, uint32 scale)
{
    XEEN_VALID();

    if(enforce(frame < _cellCount))
    {
        const Cell& cell = _cells[frame];

        for(int i = 0; i != 2; i ++)
        {
            if(cell.offset[i])
            {
                _file->seek(cell.offset[i]);
                drawFrame(out, pen, flip, scale);
            }
        }
    }
}

void XEEN::Graphics::Sprite::drawFrame(NonNull<ImageBuffer> out, const Common::Point& pen, bool flip, uint32 scale)
{
    XEEN_VALID();

    // Read the frame header
    const int16 penX = _file->readSint16LE();
    const uint16 frameWidth = _file->readUint16LE();
    const int16 penY = _file->readSint16LE();
    const uint16 frameHeight = _file->readUint16LE();

    uint16 yscale = 16 - scale;
    uint32 yaccum = yscale * penY;
    uint32 yoff = 0;

    const uint32 scaledWidth = (uint32)(((float)(penX + frameWidth)) / (16.0 / (float)(16 - scale)));

    // Draw the lines
    for(uint32 onLine = 0; onLine != frameHeight; )
    {
        for(; yaccum >= 16; yaccum -= 16)
        {
            yoff ++;
        }

        out->setPen(pen + Common::Point(((penX + frameWidth) - scaledWidth) / 2, yoff));
        out->setPenOffset(Common::Point(1, 0));
        out->setScale(scale);

        if(flip)
        {
            out->advancePen(penX + frameWidth - 1);
            out->setPenOffset(Common::Point(-1, 0));
        }

        out->advancePen(penX);

        const uint32 linesDrawn = drawLine(out);
        onLine += linesDrawn;

        yaccum += yscale * linesDrawn;

        // Check sanity: At least one line must have been drawn, and No more than 
        // frameHeight lines may have been drawn.
        assert((linesDrawn > 0) && (onLine <= frameHeight));
    }
}

uint32 XEEN::Graphics::Sprite::drawLine(NonNull<ImageBuffer> out)
{
    XEEN_VALID();

    uint8 bytes = _file->readByte();
    int16 x = _file->readByte();
    
    // Skip a number or vertical lines
    if(bytes == 0)
    {
        return x + 1;
    }
    
    // Draw
    out->advancePen(x);
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
                out->readPixels(*_file, (controlByte & 0x3F) + 1);
                break;
            }
            
            case 2:
            {
                const uint8 color = _file->readByte();
                
                for(int i = 0; i != length + 3; i ++)
                {
                    out->putPixel(color);
                }
                
                break;
            }
            
            case 3:
            {
                const uint16 backTrack = _file->readUint16LE();
                const int32 streamPos = _file->pos();
                
                _file->seek(_file->pos() - backTrack);
                out->readPixels(*_file, length + 4);
                _file->seek(streamPos);
                break;
            }
            
            case 4:
            {
                const uint8 color1 = _file->readByte();
                const uint8 color2 = _file->readByte();
                
                for(int i = 0; i != length + 2; i ++)
                {
                    out->putPixel(color1);
                    out->putPixel(color2);
                }
                
                break;
            }
            
            case 5:
            {
                out->advancePen(length + 1);
                break;
            }
            
            case 6: case 7:
            {
                // TODO: Implement properly                
                const uint8 color = _file->readByte();
                for(int i = 0; i != (controlByte & 0x7) + 3; i ++)
                {
                    out->putPixel(color);
                }
                
                break;
            }
        }
        
        // Sanity: Exactly the specified number of bytes must be used.
        assert(_file->pos() <= end);
    }
    
    return 1;
}
