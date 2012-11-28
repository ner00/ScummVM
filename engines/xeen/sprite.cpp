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

XEEN::SpriteManager::SpriteManager(CCFile& parent) : _cc(parent)
{
    memset(_sprites, 0, sizeof(_sprites));
}

XEEN::SpriteManager::~SpriteManager()
{
    for(int i = 0; i != 65536; i ++)
    {
        delete _sprites[i];
    }
}

XEEN::Sprite* XEEN::SpriteManager::getSprite(CCFileId id)
{
    if(!_sprites[id] && _cc.getEntry(id))
    {
        _sprites[id] = new Sprite(_cc.getFile(id));
    }
    
    return _sprites[id];
}


XEEN::Sprite::Sprite(CCFileData* file) : _file(file), _cellCount(0), _cells(0)
{
    assert(_file && "Sprite file not found");
    
    // Load cells
    _cellCount = _file->readUint16LE();
    _cells = new Cell[_cellCount];
    
    for(uint32 i = 0; i != _cellCount; i ++)
    {
        _cells[i].first = _file->readUint16LE();
        _cells[i].second = _file->readUint16LE();
    }
}

XEEN::Sprite::~Sprite()
{
    delete[] _cells;
    delete _file;
}

void XEEN::Sprite::drawCell(byte* out, uint16 index, uint16 xOffset, uint16 yOffset)
{
    if(index <= _cellCount)
    {
        const Cell& cell = _cells[index];

        // The first frame MUST exist.
        assert(cell.first);
        
        _file->seek(cell.first);
        drawFrame(out, xOffset, yOffset);
        
        // Draw optional second frame
        if(cell.second)
        {
            _file->seek(cell.second);
            drawFrame(out, xOffset, yOffset);        
        }
    }
    else
    {
        debug("Sprite cell out of bounds!");
    }
}

void XEEN::Sprite::drawFrame(byte* out, uint16 xOffset, uint16 yOffset)
{
    // Read the frame header
    const uint16 frameXOffset = _file->readUint16LE();
    /*const uint16 frameWidth =*/ _file->readUint16LE();
    const uint16 frameYOffset = _file->readUint16LE();
    const uint16 frameHeight = _file->readUint16LE();

    // Calculate output pen position
    const uint16 xpos = (xOffset + frameXOffset);
    const uint16 ypos = (yOffset + frameYOffset);

    // Draw the lines
    for(uint32 onLine = 0; onLine != frameHeight; )
    {
        const uint32 linesDrawn = drawLine(&out[320 * (ypos + onLine)], xpos);
        onLine += linesDrawn;

        // Check sanity: At least one line must have been drawn, and No more than 
        // frameHeight lines may have been drawn.
        assert((linesDrawn > 0) && (onLine <= frameHeight));
    }
}

uint32 XEEN::Sprite::drawLine(byte* out, uint16 xOffset)
{   
    uint8 bytes = _file->readByte();
    uint16 x = _file->readByte();
    
    // Skip a number or vertical lines
    if(bytes == 0)
    {
        return x + 1;
    }
    
    // Draw
    x += xOffset;
    int32 end = (_file->pos() - 1) + bytes;
    
    while(_file->pos() != end)
    {
        // Decode operation
        const uint8 controlByte = _file->readByte();
        const uint8 opcode = controlByte >> 5;
        const uint8 length = controlByte & 0x1F;
        
        //TODO: Clipping!!!
        
        switch(opcode)
        {
            case 0:
            {
                _file->read(&out[x], length + 1);
                x += length + 1;
                break;
            }
            
            case 1:
            {
                _file->read(&out[x], length + 33);
                x += length + 33;
                break;            
            }
            
            case 2:
            {
                const uint8 color = _file->readByte();
                
                for(int i = 0; i != length + 3; i ++)
                {
                    out[x++] = color;
                }
                
                break;
            }
            
            case 3:
            {
                const uint16 backTrack = _file->readUint16LE();
                const int32 streamPos = _file->pos();
                
                _file->seek(_file->pos() - backTrack);
                _file->read(&out[x], length + 4);
                _file->seek(streamPos);

                x += length + 4;
                break;
            }
            
            case 4:
            {
                const uint8 color1 = _file->readByte();
                const uint8 color2 = _file->readByte();
                
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
                
                const uint8 color = _file->readByte();
                for(int i = 0; i != (controlByte & 0x7) + 3; i ++)
                {
                    out[x++] = color;
                }
                
                break;
            }
        }
        
        // Sanity: Exactly the specified number of bytes must be used.
        assert(_file->pos() <= end);
    }
    
    return 1;
}
