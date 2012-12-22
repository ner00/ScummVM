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
#define XEEN_MAZE_SOURCE

#include "xeen/game.h"
#include "xeen/ui/basicwindows.h"
#include "xeen/event/basicevents.h"
#include "xeen/utility.h"
#include "xeen/characters.h"

#include "xeen/maze/eventlist_.h"
#include "xeen/maze/events_.h"
#include "xeen/maze/map.h"

XEEN::Maze::EventList::EventList(Valid<Map> parent, FilePtr data) : _parent(parent), _data(data)
{
    if(_data)
    {
        while(!_data->eos())
        {
            uint8 length = _data->readByte();

            if(length < 5)
            {
                break;
            }

            const uint8 x = _data->readByte();
            const uint8 y = _data->readByte();
            const uint8 f = _data->readByte();
            const uint8 l = _data->readByte();
            const uint32 key = (x << 16) | (y << 8);

            (void)f;

            Event& ev = _events[key];

            if(enforce(l == ev.lines.size()))
            {
                ev.lines.push_back(_data->pos() - 5);
            }
           
            _data->seek(_data->pos() + length - 4);
        }
    }
    else
    {
        _events.clear();
        markInvalid();
    }
}

void XEEN::Maze::EventList::runEventAt(const Common::Point& pos, Direction facing, uint32 line)
{
    XEEN_VALID();

    const uint32 key = (pos.x << 16) | (pos.y << 8);

    if(_events.contains(key))
    {
        Event& ev = _events[key];
        EventState state(this, pos, facing, line, 0);

        for(; state.line < ev.lines.size(); state.line ++)
        {
            state.offset = ev.lines[state.line];
            if(!runEventLine(state, ev.lines[state.line]))
            {
                break;
            }
        }
    }
}

bool XEEN::Maze::EventList::runEventLine(const EventState& state, int32 offset)
{
    XEEN_VALID();

    const uint8 opcode = _data->getByteAt(offset + 5);

    switch(opcode)
    {
        case 0x00: { return evNOP(state, offset); }
        case 0x01: { return evMESSAGE(state, offset); }
        case 0x02: { return evMAPTEXT(state, offset); }
        case 0x03: { return evMAPTEXT(state, offset); }
        case 0x04: { return evMAPTEXT(state, offset); }
        case 0x05: { return evNPC(state, offset); }
        case 0x07: { return evTELEPORT(state, offset); }
        case 0x08: { return evIF(state, offset); }
        case 0x09: { return evIF(state, offset); }
        case 0x0A: { return evIF(state, offset); }
        case 0x1F: { return evTELEPORT(state, offset); }
        case 0x27: { return evMAPTEXT(state, offset); }
        case 0x29: { return evMESSAGE(state, offset); }
        case 0x35: { return evMESSAGE(state, offset); }
        default: debug("EV: %02X", opcode); return true;
    }
}

bool XEEN::Maze::EventList::evNOP(const EventState& state, int32 offset)
{
    return true;
}

bool XEEN::Maze::EventList::evMAPTEXT(const EventState& state, int32 offset)
{
    // TODO
    debug("MAPTEXT");
    debug("%s", _parent->getString(_data->getByteAt(offset + 6)));
    return true;
}

bool XEEN::Maze::EventList::evMESSAGE(const EventState& state, int32 offset)
{
    const char* msg = _parent->getString(_data->getByteAt(offset + 6));
    _parent->getGame()->setEvent(new Message(_parent->getGame(), state, msg));
    return false; // TODO: ?
}

bool XEEN::Maze::EventList::evNPC(const EventState& state, int32 offset)
{
    // TODO
    debug("NPC");

    const char* name = _parent->getString(_data->getByteAt(offset + 6));
    const char* msg = _parent->getString(_data->getByteAt(offset + 7));

    _parent->getGame()->setEvent(new NPC(_parent->getGame(), state, name, msg));

    return false;
}

bool XEEN::Maze::EventList::evTELEPORT(const EventState& state, int32 offset)
{
    // TODO: Handle map == 0 for mirrors!
    Valid<Party> p = _parent->getGame()->getParty();
    p->changeMap(_data->getByteAt(offset + 6));
    p->moveTo(Common::Point(_data->getByteAt(offset + 7), _data->getByteAt(offset + 8)), 4);
    
    return _data->getByteAt(offset + 5) == 0x07;
}

bool XEEN::Maze::EventList::evIF(const EventState& state, int32 offset)
{
    // TODO
    debug("IF");
    uint32 val = produceValue(_data->getByteAt(offset + 6));

    return true;
}

uint32 XEEN::Maze::EventList::produceValue(uint32 id)
{
    Valid<Game> g = _parent->getGame();
    Character* c = g->getActiveCharacter();

    switch(id)
    {
        case 0x03: return c->getSex();
        case 0x04: return c->getRace();
        case 0x05: return c->getClass();
        case 0x08: return c->getValue(Character::HP) & 0xFF; //TODO: ?
        case 0x09: return c->getValue(Character::SP) & 0xFF; //TODO: ?
        case 0x0A: debug("AC"); return 10;
        case 0x0B: return c->getStat(LEVEL).getTemp();
        case 0x0C: debug("AGE"); return 21;
        case 0x0D: debug("SKILL"); return 0;
        case 0x0F: debug("AWARD"); return 0;
        case 0x10: return c->getValue(Character::EXPERIENCE);
        case 0x11: debug("POISPROT"); return 0;
        case 0x12: debug("CONDITION"); return 0;
        case 0x13: debug("CANCAST"); return 0;
        case 0x14: debug("GAMEFLAG"); return 0;
        case 0x15: debug("ITEM"); return 0;
        case 0x19: debug("MINS"); return 0;
        case 0x22: debug("GOLD"); return 0;
        case 0x23: debug("GEMS"); return 0;
        case 0x25: return c->getStat(MIGHT).getTemp();
        case 0x26: return c->getStat(INTELLECT).getTemp();
        case 0x27: return c->getStat(PERSONALITY).getTemp();
        case 0x28: return c->getStat(ENDURANCE).getTemp();
        case 0x29: return c->getStat(SPEED).getTemp();
        case 0x2A: return c->getStat(ACCURACY).getTemp();
        case 0x2B: return c->getStat(LUCK).getTemp();
        case 0x2C: debug("YESNO"); return 0;
        case 0x2D: return c->getStat(MIGHT).getReal();
        case 0x2E: return c->getStat(INTELLECT).getReal();
        case 0x2F: return c->getStat(PERSONALITY).getReal();
        case 0x30: return c->getStat(ENDURANCE).getReal();
        case 0x31: return c->getStat(SPEED).getReal();
        case 0x32: return c->getStat(ACCURACY).getReal();
        case 0x33: return c->getStat(LUCK).getReal();
        case 0x34: return c->getStat(FIRE).getReal();
        case 0x35: return c->getStat(ELEC).getReal();
        case 0x36: return c->getStat(COLD).getReal();
        case 0x37: return c->getStat(POISON).getReal();
        case 0x38: return c->getStat(ENERGY).getReal();
        case 0x39: return c->getStat(MAGIC).getReal();
        case 0x3A: return c->getStat(FIRE).getTemp();
        case 0x3B: return c->getStat(ELEC).getTemp();
        case 0x3C: return c->getStat(COLD).getTemp();
        case 0x3D: return c->getStat(POISON).getTemp();
        case 0x3E: return c->getStat(ENERGY).getTemp();
        case 0x3F: return c->getStat(MAGIC).getTemp();
        case 0x40: return c->getStat(LEVEL).getReal();
        case 0x41: debug("FOOD"); return 600;
        case 0x45: debug("LEVITATE"); return 0;
        case 0x46: debug("LIGHT"); return 0;
        case 0x47: debug("FIREPROT"); return 0;
        case 0x48: debug("ELECPROT"); return 0;
        case 0x49: debug("COLDPROT"); return 0;
        case 0x4C: debug("DAY"); return 0;
        case 0x4D: debug("ACTEMP"); return 0;
        case 0x4E: debug("HPFULL"); return 0;
        case 0x4F: debug("WIZEYE"); return 0;
        case 0x51: debug("SPFULL"); return 0;
        case 0x55: debug("YEAR"); return 100;
        case 0x56: return c->getStat(MIGHT).getValue();
        case 0x57: return c->getStat(INTELLECT).getValue();
        case 0x58: return c->getStat(PERSONALITY).getValue();
        case 0x59: return c->getStat(ENDURANCE).getValue();
        case 0x5A: return c->getStat(SPEED).getValue();
        case 0x5B: return c->getStat(ACCURACY).getValue();
        case 0x5C: return c->getStat(LUCK).getValue();
        case 0x5D: debug("WEEK"); return 0;
        case 0x5E: debug("WALKONWATER"); return 0;
        case 0x63: debug("SKILL"); return 0;
        case 0x66: debug("THIEV"); return 0;
        case 0x67: debug("WORLD FLAGS"); return 0;
        case 0x68: debug("QUEST FLAG"); return 0;
        case 0x69: debug("CREDITS"); return 0;
        default: debug("Unknown Value ID: %02X", id); return 0;
    }
}
