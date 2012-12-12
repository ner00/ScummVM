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

#include "xeen/game.h"

#include "xeen/graphics/imagebuffer.h"
#include "xeen/graphics/sprite.h"

XEEN::Sprite::Sprite(FilePtr file) : _file(file), _cellCount(0), _cells(0)
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
                    _file->seek(2 + i * 4);
                    uint16 off1 = _file->readUint16LE();
                    uint16 off2 = _file->readUint16LE();
                    cacheCell(_cells[i], off1, off2);
                }
                
                return;
            }
        }
    }

    markInvalidAndClean();
}

XEEN::Sprite::~Sprite()
{
    cleanse();
}

void XEEN::Sprite::cleanse()
{
    DELETE_ARRAY(_cells);
    _cellCount = 0;
}

void XEEN::Sprite::drawCell(ImageBuffer& out, const Common::Point& pen, uint16 frame, bool flip, uint32 scale)
{
    if(enforce(frame < _cellCount))
    {
        Cell& cell = _cells[frame];

        if(scale == 0)
        {
            for(int i = 0; i != cell.height; i ++)
            {
                const byte* line = &cell.pixels[i * cell.width];
    
                if(flip)
                {
                    out.setPen(pen + Common::Point(cell.width, i));
                    out.setPenOffset(Common::Point(-1, 0));
                }
                else
                {
                    out.setPen(pen + Common::Point(0, i));
                    out.setPenOffset(Common::Point(1, 0));
                }
    
                for(int j = 0; j != cell.width; j ++, line ++)
                {
                    if(*line)
                    {
                        out.putPixel(*line);
                    }
                    else
                    {
                        out.movePen(Common::Point(1, 0));
                    }
                }
            }
        }
        else
        {
            uint32 yAccum = 0;
            uint32 online = 0;

            for(int i = 0; i != cell.height; i ++)
            {
                yAccum += 4;

                while(yAccum > scale)
                {
                    yAccum -= scale;
                    const byte* line = &cell.pixels[i * cell.width];
        
                    if(flip)
                    {
                        out.setPen(pen + Common::Point(cell.width, online++));
                        out.setPenOffset(Common::Point(-1, 0));
                    }
                    else
                    {
                        out.setPen(pen + Common::Point(0, online++));
                        out.setPenOffset(Common::Point(1, 0));
                    }

                    uint32 accum = 0;
                    for(int j = 0; j != cell.width; j ++, line ++)
                    {
                        accum += 4;
    
                        while(accum > scale)
                        {
                            accum -= scale;
                            if(*line)
                            {
                                out.putPixel(*line);
                            }
                            else
                            {
                                out.movePen(Common::Point(1, 0));
                            }                        
                        }
                    }
                }
            }
        }
    }

    out.setPenOffset(Common::Point(1, 0));
}


void XEEN::Sprite::cacheCell(Cell& cell, uint16 offset1, uint16 offset2)
{
    XEEN_VALID();

    if(offset1)
    {
        cacheFrame(cell, offset1);
    }

    if(offset2)
    {
        cacheFrame(cell, offset2);
    }
}

void XEEN::Sprite::cacheFrame(Cell& out, uint16 offset)
{
    XEEN_VALID();

    _file->seek(offset);

    // Read the frame header
    const int16 offX = _file->readSint16LE();
    const uint16 width = _file->readUint16LE();
    const int16 offY = _file->readSint16LE();
    const uint16 height = _file->readUint16LE();

    if(out.pixels == 0)
    {
        out.width = offX + width;
        out.height = offY + height;
        out.pixels = new byte[out.width * out.height];
        memset(out.pixels, 0, out.width * out.height);
    }

    // Draw the lines
    for(uint32 onLine = 0; onLine < height; )
    {
        const uint32 linesDrawn = cacheLine(out, offY + onLine, offX);
        onLine += linesDrawn;

        // Check sanity: At least one line must have been drawn, and No more than 
        // frameHeight lines may have been drawn.
        assert((linesDrawn > 0) && (onLine <= height));
    }
}

uint32 XEEN::Sprite::cacheLine(Cell& out, uint16 line, uint16 offX)
{
    uint8 bytes = _file->readByte();
    int16 x = _file->readByte();
    
    // Skip a number or vertical lines
    if(bytes == 0)
    {
        return x + 1;
    }
    
    // Draw
    byte* const pixels = &out.pixels[line * out.width];
    offX += x;
    int32 end = (_file->pos() - 1) + bytes;
    
    while(_file->pos() != end)
    {
        // Sanity: Exactly the specified number of bytes must be used.
        assert(_file->pos() <= end);

        // Decode operation
        const uint8 controlByte = _file->readByte();
        const uint8 opcode = controlByte >> 5;
        const uint8 length = controlByte & 0x1F;
        
        switch(opcode)
        {
            case 0: case 1:
            {
                for(int i = 0; i != (controlByte & 0x3F) + 1; i ++)
                {
                    if((offX < out.width))
                    {
                        pixels[offX ++] = _file->readByte();
                    }
                    else _file->readByte();
                }
                break;
            }
            
            case 2:
            {
                const uint8 color = _file->readByte();
                
                for(int i = 0; i != length + 3; i ++)
                {
                    if((offX < out.width))
                    {
                        pixels[offX ++] = color;
                    }
                }
                
                break;
            }
            
            case 3:
            {
                const uint16 backTrack = _file->readUint16LE();
                const int32 streamPos = _file->pos();
                
                _file->seek(_file->pos() - backTrack);

                for(int i = 0; i != length + 4; i ++)
                {
                    if((offX < out.width))
                    {
                        pixels[offX ++] = _file->readByte();
                    }
                    else _file->readByte();
                }

                _file->seek(streamPos);
                break;
            }
            
            case 4:
            {
                const uint8 color1 = _file->readByte();
                const uint8 color2 = _file->readByte();
                
                for(int i = 0; i != length + 2; i ++)
                {
                    if((offX < out.width))
                    {
                        pixels[offX ++] = color1;
                    }

                    if((offX < out.width))
                    {
                        pixels[offX ++] = color2;
                    }
                }
                
                break;
            }
            
            case 5:
            {
                offX += length + 1;
                break;
            }
            
            case 6: case 7:
            {
                // TODO: Implement properly                
                const uint8 color = _file->readByte();
                for(int i = 0; i != (controlByte & 0x7) + 3; i ++)
                {
                    if((offX < out.width))
                    {
                        pixels[offX ++] = color;
                    }
                }
                
                break;
            }
        }
    }
    
    return 1;
}
