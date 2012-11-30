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
    for(unsigned i = 0; i != MAX_SPRITES; i ++)
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
    if(!initialize())
    {
        assert(false);
        cleanse();
    }
}

XEEN::Sprite::~Sprite()
{
    cleanse();
}

bool XEEN::Sprite::initialize()
{
    bool success = false;

    if(enforce(_file))
    {
        _cellCount = _file->readUint16LE();
        
        if(enforce(_cellCount))
        {
            _cells = new Cell[_cellCount];
            
            if(enforce(_cells))
            {
                success = true;
                
                for(unsigned i = 0; success && i != _cellCount; i ++)
                {
                    _cells[i].offset[0] = _file->readUint16LE();
                    _cells[i].offset[1] = _file->readUint16LE();
                }
            }
        }
    }
    
    return success;
}

void XEEN::Sprite::cleanse()
{
    DELETE_ARRAY(_cells);
    DELETE(_file);
    _cellCount = 0;
}

void XEEN::Sprite::drawCell(ImageBuffer& out, const Common::Point& pen, uint16 frame, bool flip)
{
    if(enforce(frame < _cellCount))
    {
        const Cell& cell = _cells[frame];

        for(int i = 0; i != 2; i ++)
        {
            if(cell.offset[i])
            {
                _file->seek(cell.offset[i]);
                drawFrame(out, pen, flip);
            }
        }
    }
}

void XEEN::Sprite::drawFrame(ImageBuffer& out, const Common::Point& pen, bool flip)
{
    // Read the frame header
    const int16 penX = _file->readSint16LE();
    const uint16 frameWidth = _file->readUint16LE();
    const int16 penY = _file->readSint16LE();
    const uint16 frameHeight = _file->readUint16LE();

    out.setOrigin(pen + Common::Point(penX + (flip ? frameWidth - 1 : 0), penY));
    out.setPenOffset(Common::Point(flip ? -1 : 1, 0));

    // Draw the lines
    for(uint32 onLine = 0; onLine != frameHeight; )
    {
        const uint32 linesDrawn = drawLine(out.setPen(Common::Point(0, onLine)));
        onLine += linesDrawn;

        // Check sanity: At least one line must have been drawn, and No more than 
        // frameHeight lines may have been drawn.
        assert((linesDrawn > 0) && (onLine <= frameHeight));
    }
}

uint32 XEEN::Sprite::drawLine(ImageBuffer& out)
{   
    uint8 bytes = _file->readByte();
    int16 x = _file->readByte();
    
    // Skip a number or vertical lines
    if(bytes == 0)
    {
        return x + 1;
    }
    
    // Draw
    out.movePen(Common::Point(x, 0));
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
                out.readPixels(*_file, (controlByte & 0x3F) + 1);
                break;
            }
            
            case 2:
            {
                const uint8 color = _file->readByte();
                
                for(int i = 0; i != length + 3; i ++)
                {
                    out.putPixel(color);
                }
                
                break;
            }
            
            case 3:
            {
                const uint16 backTrack = _file->readUint16LE();
                const int32 streamPos = _file->pos();
                
                _file->seek(_file->pos() - backTrack);
                out.readPixels(*_file, length + 4);
                _file->seek(streamPos);
                break;
            }
            
            case 4:
            {
                const uint8 color1 = _file->readByte();
                const uint8 color2 = _file->readByte();
                
                for(int i = 0; i != length + 2; i ++)
                {
                    out.putPixel(color1);
                    out.putPixel(color2);
                }
                
                break;
            }
            
            case 5:
            {
                out.movePen(Common::Point(length + 1, 0));
                break;
            }
            
            case 6: case 7:
            {
                // TODO: Implement properly                
                const uint8 color = _file->readByte();
                for(int i = 0; i != (controlByte & 0x7) + 3; i ++)
                {
                    out.putPixel(color);
                }
                
                break;
            }
        }
        
        // Sanity: Exactly the specified number of bytes must be used.
        assert(_file->pos() <= end);
    }
    
    return 1;
}
