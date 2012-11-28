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

#include "xeen/ccfile.h"
#include "xeen/sprite.h"

#include "common/file.h"
#include "common/debug.h"

XEEN::Sprite::Sprite(CCFile& cc, CCFileId id) : _cc(cc), _id(id), _file(cc.getFileRaw(id)), _cellCount(0), _cells(0)
{
    assert(_file && _file->size);
    
    Common::MemoryReadStream reader(_file->data, _file->size);

    // Load cells
    _cellCount = reader.readUint16LE();
    _cells = new Cell[_cellCount];
    
    for(uint32 i = 0; i != _cellCount; i ++)
    {
        _cells[i].first = reader.readUint16LE();
        _cells[i].second = reader.readUint16LE();
    }
}

XEEN::Sprite::~Sprite()
{
    delete[] _cells;

    // TODO: Close file
}

void XEEN::Sprite::drawCell(byte* out, uint16 index, uint16 xOffset, uint16 yOffset)
{
    if(index <= _cellCount)
    {
        const Cell& cell = _cells[index];
        Common::MemoryReadStream reader(_file->data, _file->size);        

        // The first frame MUST exist.
        assert(cell.first);
        
        reader.seek(cell.first);
        drawFrame(out, reader, xOffset, yOffset);
        
        // Draw optional second frame
        if(cell.second)
        {
            reader.seek(cell.second);
            drawFrame(out, reader, xOffset, yOffset);        
        }
    }
    else
    {
        debug("Sprite cell out of bounds: %d, %d", (short)_id, index);
    }
}

void XEEN::Sprite::drawFrame(byte* out, Common::MemoryReadStream& reader, uint16 xOffset, uint16 yOffset)
{
    // Read the frame header
    const uint16 frameXOffset = reader.readUint16LE();
    /*const uint16 frameWidth =*/ reader.readUint16LE();
    const uint16 frameYOffset = reader.readUint16LE();
    const uint16 frameHeight = reader.readUint16LE();

    // Calculate output pen position
    const uint16 xpos = (xOffset + frameXOffset);
    const uint16 ypos = (yOffset + frameYOffset);

    // Draw the lines
    for(uint32 onLine = 0; onLine != frameHeight; )
    {
        const uint32 linesDrawn = drawLine(&out[320 * (ypos + onLine)], reader, xpos);
        onLine += linesDrawn;

        // Check sanity: At least one line must have been drawn, and No more than 
        // frameHeight lines may have been drawn.
        assert((linesDrawn > 0) && (onLine <= frameHeight));
    }
}

uint32 XEEN::Sprite::drawLine(byte* out, Common::MemoryReadStream& reader, uint16 xOffset)
{   
    uint8 bytes = reader.readByte();
    uint16 x = reader.readByte();
    
    // Skip a number or vertical lines
    if(bytes == 0)
    {
        return x + 1;
    }
    
    // Draw
    x += xOffset;
    int32 end = (reader.pos() - 1) + bytes;
    
    while(reader.pos() != end)
    {
        // Decode operation
        const uint8 controlByte = reader.readByte();
        const uint8 opcode = controlByte >> 5;
        const uint8 length = controlByte & 0x1F;
        
        //TODO: Clipping!!!
        
        switch(opcode)
        {
            case 0:
            {
                reader.read(&out[x], length + 1);
                x += length + 1;
                break;
            }
            
            case 1:
            {
                reader.read(&out[x], length + 33);
                x += length + 33;
                break;            
            }
            
            case 2:
            {
                const uint8 color = reader.readByte();
                
                for(int i = 0; i != length + 3; i ++)
                {
                    out[x++] = color;
                }
                
                break;
            }
            
            case 3:
            {
                const uint16 backTrack = reader.readUint16LE();
                const int32 streamPos = reader.pos();
                
                reader.seek(reader.pos() - backTrack);
                reader.read(&out[x], length + 4);
                reader.seek(streamPos);

                x += length + 4;
                break;
            }
            
            case 4:
            {
                const uint8 color1 = reader.readByte();
                const uint8 color2 = reader.readByte();
                
                for(int i = 0; i != length + 2; i ++)
                {
                    out[x++] = color1;
                    out[x++] = color2;
                }
                
                break;
            }
            
            case 5:
            {
                x += length + 1;
                break;
            }
            
            case 6:
            case 7:
            {
//                debug("Sprite Pattern Command: Not implemented");
                
                const uint8 color = reader.readByte();
                for(int i = 0; i != (controlByte & 0x7) + 3; i ++)
                {
                    out[x++] = color;
                }
                
                break;
            }
        }
        
        // Sanity: Exactly the specified number of bytes must be used.
        assert(reader.pos() <= end);
    }
    
    return 1;
}
