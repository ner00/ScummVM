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

#ifndef XEEN_IMAGEBUFFER_H
#define XEEN_IMAGEBUFFER_H

#ifndef XEEN_GRAPHICS_SOURCE
# error "Private header included"
#endif

#include "xeen/utility.h"

namespace XEEN
{
    namespace Graphics
    {
        static const uint16 scaleTable[] = {0xFFFF, 0xFFEF, 0xEFEF, 0xEFEE, 0xEEEE, 0xEEAE,
                                            0xAEAE, 0xAEAA, 0xAAAA, 0xAA8A, 0x8A8A, 0x8A88,
                                            0x8888, 0x8880, 0x8080, 0x8000};

        struct ImageBuffer
        {
            public:
                static const uint32 WIDTH = 320;
                static const uint32 HEIGHT = 200;

            public:
                ImageBuffer() : _pen(0, 0), _penOffset(1, 0), _clip(0, 0, WIDTH, HEIGHT)
                {
                
                }

                bool checkScale(uint32 offset, uint32 scale)
                {
                    const uint16 val = scaleTable[scale & 0xF];
                    return (val << (offset & 0xF)) & 0x8000;
                }

                uint32 scaleSize(uint32 size, uint32 scale)
                {
                    uint32 result = 0;
                    for(uint32 i = 0; i != size; i ++)
                    {
                        result += checkScale(i, scale) ? 1 : 0;
                    }
                    return result;
                }

                void reset()
                {
                    _pen = Common::Point(0, 0);
                    _penOffset = Common::Point(1, 0);
                    _clip = Common::Rect(0, 0, WIDTH, HEIGHT);
                }
                
                ImageBuffer& clear(uint8 color)
                {
                    memset(buffer, color, sizeof(buffer));
                    return *this;
                }
                
                ImageBuffer& setClipArea(const Common::Rect& clip)
                {
                    _clip = clip;
                    _clip.clip(Common::Rect(0, 0, WIDTH, HEIGHT));

                    return *this;
                }
        
                // TODO: CLIP
                ImageBuffer& fillRect(const Common::Rect& rect, byte color)
                {
                    enforce(rect.isValidRect());
                
                    for(int i = rect.top; i != rect.bottom && i < 200; i ++)
                    {
                        for(int j = rect.left; j != rect.right && j < 320; j ++)
                        {
                            if(j >= 0 && i >= 0)
                            {
                                buffer[i * 320 + j] = color;
                            }
                        }
                    }
                    
                    return *this;
                }

                template <uint8 KEY>
                void drawLine(int32 x, int32 y, uint32 width, const uint8* pixels, uint32 scale, bool flip)
                {
                    if(_clip.contains(_clip.left, y))
                    {
                        if(scale & 0x8000) // Double Size
                        {
                            if(!flip)
                            {
                                for(uint32 i = 0; i != width && x < _clip.right; i ++)
                                {
                                    const uint8 pixel = pixels[i];
                                    if(pixel != KEY && x >= _clip.left)
                                    {
                                        buffer[y * WIDTH + x] = pixel;
                                    }

                                    x ++;

                                    if(pixel != KEY && x >= _clip.left && x < _clip.right)
                                    {
                                        buffer[y * WIDTH + x] = pixel;
                                    }

                                    x ++;
                                }
                            }
                            else
                            {
                                x += width * 2;

                                for(uint32 i = 0; i != width && x >= _clip.left; i ++)
                                {
                                    const uint8 pixel = pixels[i];
                                    if(pixel != KEY && x < _clip.right)
                                    {
                                        buffer[y * WIDTH + x] = pixel;
                                    }

                                    x --;

                                    if(pixel != KEY && x < _clip.right && x >= _clip.left)
                                    {
                                        buffer[y * WIDTH + x] = pixel;
                                    }

                                    x --;
                                }
                            }
                        }
                        else
                        {
                            if(!flip)
                            {
                                for(uint32 i = 0; i != width && x < _clip.right; i ++)
                                {
                                    if(checkScale(i, scale))
                                    {
                                        const uint8 pixel = pixels[i];
                                        if(pixel != KEY && x >= _clip.left)
                                        {
                                            buffer[y * WIDTH + x] = pixel;
                                        }
        
                                        x ++;
                                    }
                                }
                            }
                            else
                            {
                                x += scaleSize(width, scale);
    
                                for(uint32 i = 0; i != width && x >= _clip.left; i ++)
                                {
                                    if(checkScale(i, scale))
                                    {
                                        const uint8 pixel = pixels[i];
                                        if(pixel != KEY && x < _clip.right)
                                        {
                                            buffer[y * WIDTH + x] = pixel;
                                        }
        
                                        x --;
                                    }
                                }
                            }
                        }
                    }
                }
        
            public:    
                byte buffer[WIDTH * HEIGHT];
                
            private:
                Common::Point _pen;
                Common::Point _penOffset;
                
                Common::Rect _clip;
        };
    }
}

#endif // XEEN_IMAGEBUFFER_H
