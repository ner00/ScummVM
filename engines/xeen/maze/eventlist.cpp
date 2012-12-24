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
#include "xeen/maze/objects_.h"
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

            if(l != ev.lines.size())
            {
                markInvalid("Event line out of order: Found %d expected %d", l, ev.lines.size());
                return;
            }
           
            ev.lines.push_back(_data->pos() - 5);
            _data->seek(_data->pos() + length - 4);
        }
    }
    else
    {
        _events.clear();
        markInvalid("Event file not found"); // TODO: Print name
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
        case 0x00: { return true; }
        case 0x01: { return evMESSAGE(state, offset); }
        case 0x02: { return evMAPTEXT(state, offset); }
        case 0x03: { return evMAPTEXT(state, offset); }
        case 0x04: { return evMAPTEXT(state, offset); }
        case 0x05: { return evNPC(state, offset); }
        case 0x06: { debug("PlayFX"); return true; }
        case 0x07: { return evTELEPORT(state, offset); }
        case 0x08: { return evIF(state, offset); }
        case 0x09: { return evIF(state, offset); }
        case 0x0A: { return evIF(state, offset); }
        case 0x0B: { return evMOVEOBJ(state); }
        case 0x0C: { return evGIVETAKE(state); return true; }
        case 0x0D: { return true; }
        case 0x0E: { return evREMOVE(state); }
        case 0x0F: { debug("SetChar"); return true; }
        case 0x10: { return evSPAWN(state); }
        case 0x11: { debug("DoTownEvent"); return true; }
        case 0x12: { return false; }
        case 0x13: { debug("AltarMap"); return true; }
        case 0x14: { debug("GiveTreasureChest"); return true; }
        case 0x15: { debug("ConfirmWord"); return true; }
        case 0x16: { debug("Damage"); return true; }
        case 0x17: { debug("JmpRnd"); return true; }
        case 0x18: { return evALTEREVENT(state); }
        case 0x19: { debug("CallEvent"); return true; }
        case 0x1A: { debug("Return"); return true; }
        case 0x1B: { debug("SetVar"); return true; }
        case 0x1C: { debug("TakeOrGive"); return true; }
        case 0x1D: { debug("TakeOrGive"); return true; }
        case 0x1E: { debug("CutsceneEndClouds"); return true; }
        case 0x1F: { return evTELEPORT(state, offset); }
        case 0x20: { return evWHOWILL(state); }
        case 0x21: { debug("RndDamage"); return true; }
        case 0x22: { debug("MoveWallObj"); return true; }
        case 0x23: { return evSETCELLFLAGS(state); }
        case 0x24: { debug("AlterHed"); return true; }
        case 0x25: { debug("DisplayStat"); return true; }
        case 0x26: { debug("TakeOrGive"); return true; }
        case 0x27: { return evMAPTEXT(state, offset); }
        case 0x28: { debug("PlayEventVoc"); return true; }
        case 0x29: { return evMESSAGE(state, offset); }
        case 0x2A: { return evIFMAPFLAG(state); }
        case 0x2B: { debug("SelRndChar"); return true; }
        case 0x2C: { debug("GiveEnchanted"); return true; }
        case 0x2D: { debug("ItemType"); return true; }
        case 0x2E: { evREMOVE(state); }
        case 0x2F: { return true; }
        case 0x30: { debug("ChooseNumeric"); return true; }
        case 0x31: { debug("DisplayBottomTwoLines"); return true; }
        case 0x32: { debug("DisplayLarge"); return true; }
        case 0x33: { debug("ExchObj"); return true; }
        case 0x34: { debug("FallToMap"); return true; }
        case 0x35: { return evMESSAGE(state, offset); }
        case 0x36: { debug("GOTO"); return true; }
        case 0x37: { debug("ConfirmWord2"); return true; }
        case 0x38: { debug("GotoRandom"); return true; }
        case 0x39: { debug("CutsceneEndDarkside"); return true; }
        case 0x3A: { debug("CutsceneEndWorld"); return true; }
        case 0x3B: { debug("FlipWorld"); return true; }
        case 0x3C: { debug("PlayCD"); return true; }
        default: debug("EV: %02X", opcode); return true;
    }
}

bool XEEN::Maze::EventList::evMAPTEXT(const EventState& state, int32 offset)
{
    // TODO: Add values for each opcode
    _parent->setSignMessage(_parent->getString(_data->getByteAt(offset + 6)), Common::Rect(8, 8, 200, 100), 0);
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

    const uint8 valueID = _data->getByteAt(offset + 6);

    if(valueID == 0x2C) // YES/NO
    {
        _parent->getGame()->setEvent(new IfYesNo(_parent->getGame(), state));
        return false;
    }
    else
    {
        Valid<Game> g = _parent->getGame();
        Character* c = g->getActiveCharacter();
        Party* p = g->getParty();
    
        bool result = false;

        switch(valueID)
        {
            case 0x09: result = true; break; // TODO: Compare byte against SP!
            case 0x14: result = p->getGameFlag(state.getByteAt(7)); break;


/*            case 0x03: val = c->getSex(); break;
            case 0x04: val = c->getRace(); break;
            case 0x05: val = c->getClass(); break;
            case 0x08: val = c->getValue(Character::HP) & 0xFF; break; //TODO: ?
            case 0x0A: debug("AC"); break;
            case 0x0B: val = c->getStat(LEVEL).getTemp(); break;
            case 0x0C: debug("AGE"); break;
            case 0x0D: debug("SKILL"); break;
            case 0x0F: debug("AWARD"); break;
            case 0x10: val = c->getValue(Character::EXPERIENCE); break;
            case 0x11: debug("POISPROT"); break;
            case 0x12: debug("CONDITION"); break;
            case 0x13: debug("CANCAST"); break;*/

/*            case 0x15: debug("ITEM"); break;
            case 0x19: debug("MINS"); break;
            case 0x22: debug("GOLD"); break;
            case 0x23: debug("GEMS"); break;
            case 0x25: val = c->getStat(MIGHT).getTemp(); break;
            case 0x26: val = c->getStat(INTELLECT).getTemp(); break;
            case 0x27: val = c->getStat(PERSONALITY).getTemp(); break;
            case 0x28: val = c->getStat(ENDURANCE).getTemp(); break;
            case 0x29: val = c->getStat(SPEED).getTemp(); break;
            case 0x2A: val = c->getStat(ACCURACY).getTemp(); break;
            case 0x2B: val = c->getStat(LUCK).getTemp(); break;
            case 0x2C: debug("YESNO"); break;
            case 0x2D: val = c->getStat(MIGHT).getReal(); break;
            case 0x2E: val = c->getStat(INTELLECT).getReal(); break;
            case 0x2F: val = c->getStat(PERSONALITY).getReal(); break;
            case 0x30: val = c->getStat(ENDURANCE).getReal(); break;
            case 0x31: val = c->getStat(SPEED).getReal(); break;
            case 0x32: val = c->getStat(ACCURACY).getReal(); break;
            case 0x33: val = c->getStat(LUCK).getReal(); break;
            case 0x34: val = c->getStat(FIRE).getReal(); break;
            case 0x35: val = c->getStat(ELEC).getReal(); break;
            case 0x36: val = c->getStat(COLD).getReal(); break;
            case 0x37: val = c->getStat(POISON).getReal(); break;
            case 0x38: val = c->getStat(ENERGY).getReal(); break;
            case 0x39: val = c->getStat(MAGIC).getReal(); break;
            case 0x3A: val = c->getStat(FIRE).getTemp(); break;
            case 0x3B: val = c->getStat(ELEC).getTemp(); break;
            case 0x3C: val = c->getStat(COLD).getTemp(); break;
            case 0x3D: val = c->getStat(POISON).getTemp(); break;
            case 0x3E: val = c->getStat(ENERGY).getTemp(); break;
            case 0x3F: val = c->getStat(MAGIC).getTemp(); break;
            case 0x40: val = c->getStat(LEVEL).getReal(); break;
            case 0x41: debug("FOOD"); break;
            case 0x45: debug("LEVITATE"); break;
            case 0x46: debug("LIGHT"); break;
            case 0x47: debug("FIREPROT"); break;
            case 0x48: debug("ELECPROT"); break;
            case 0x49: debug("COLDPROT"); break;
            case 0x4C: debug("DAY"); break;
            case 0x4D: debug("ACTEMP"); break;
            case 0x4E: debug("HPFULL"); break;
            case 0x4F: debug("WIZEYE"); break;
            case 0x51: debug("SPFULL"); break;
            case 0x55: debug("YEAR"); break;
            case 0x56: val = c->getStat(MIGHT).getValue(); break;
            case 0x57: val = c->getStat(INTELLECT).getValue(); break;
            case 0x58: val = c->getStat(PERSONALITY).getValue(); break;
            case 0x59: val = c->getStat(ENDURANCE).getValue(); break;
            case 0x5A: val = c->getStat(SPEED).getValue(); break;
            case 0x5B: val = c->getStat(ACCURACY).getValue(); break;
            case 0x5C: val = c->getStat(LUCK).getValue(); break;
            case 0x5D: debug("WEEK"); break;
            case 0x5E: debug("WALKONWATER"); break;
            case 0x63: debug("SKILL"); break;
            case 0x66: debug("THIEV"); break;
            case 0x67: debug("WORLD FLAGS"); break;
            case 0x68: debug("QUEST FLAG"); break;
            case 0x69: debug("CREDITS"); break;*/
            default: debug("Unknown Value ID: %02X", valueID); break;
        }

        if(result)
        {
            EventState oldState = state;
            oldState.runFrom(state.getByteAt(state.getByteAt(0)));
            return false;            
        }

        return true;
    }
}

bool XEEN::Maze::EventList::evMOVEOBJ(const EventState& state)
{
    const Common::Point dest(state.getByteAt(7), state.getByteAt(8));
    _parent->moveObject(state.getByteAt(6), dest);
    return true;
}

bool XEEN::Maze::EventList::evSPAWN(const EventState& state)
{
    const Common::Point dest(state.getByteAt(7), state.getByteAt(8));
    _parent->moveMonster(state.getByteAt(6), dest, true);
    return true;
}

bool XEEN::Maze::EventList::evSETCELLFLAGS(const EventState& state)
{
    // TODO: I don't think this is right.......
    const Common::Point dest(state.getByteAt(6), state.getByteAt(7));
    _parent->setFlags(dest, state.getByteAt(8));
    return true;
}

bool XEEN::Maze::EventList::evIFMAPFLAG(const EventState& state)
{
    const uint8 id = state.getByteAt(6);

    // TODO: Check All
    if(id == 0xFF)
    {
        return true;
    }

    Valid<Objects> objs = _parent->getObjects();
    const ObjectEntry oe = objs->getMonsterData(id);

    // TODO: Check proper lower range
    if(oe.pos.x >= 0x20 && oe.pos.y >= 0x20)
    {
        EventState oldState = state;
        oldState.runFrom(state.getByteAt(7));
        return false;
    }

    return true;
}

bool XEEN::Maze::EventList::evGIVETAKE(const EventState& state)
{
    static const uint8 valSize[0x70] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        4, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1,
        1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    const uint32 take = state.getByteAt(6);
    const uint32 giveOffset = 7 + ((take < 0x70) ? valSize[take] : 1);
    const uint32 give = state.getByteAt(giveOffset);

    Valid<Game> g = _parent->getGame();
    Character* const c = g->getActiveCharacter();
    Party* const p = g->getParty();

    switch(take)
    {
        case 0x00: break;
        case 0x14: p->setGameFlag(state.getByteAt(giveOffset + 1), false); break;
        default: debug("Unknown Take ID: %02X", take); break;
    }

    switch(give)
    {
        case 0x00: break;
        case 0x14: p->setGameFlag(state.getByteAt(giveOffset + 1), true); break;
        default: debug("Unknown Give ID: %02X", give); break;
    }

    return true;
}

bool XEEN::Maze::EventList::evREMOVE(const EventState& state)
{
    // TODO: Untested
    // TODO: Differentiate between 0x0E(Remove) and 0x2E(MakeNothingHere)
    // TODO: How does the real game achieve this. Here I just set every opcode to NOP.
    const uint32 key = (state.pos.x << 16) | (state.pos.y << 8);

    if(enforce(_events.contains(key)))
    {
        Event& ev = _events[key];
        for(uint32 i = 0; i != ev.lines.size(); i ++)
        {
            _data->setByteAt(ev.lines[i] + 5, 0);
        }
    }

    return true;
}

bool XEEN::Maze::EventList::evALTEREVENT(const EventState& state)
{
    const uint32 key = (state.pos.x << 16) | (state.pos.y << 8);
    const uint32 line = state.getByteAt(6);
    const uint32 newop = state.getByteAt(7);

    if(enforce(_events.contains(key)))
    {
        Event& ev = _events[key];
        if(enforce(line < ev.lines.size()))
        {
            const uint32 oldop = _data->getByteAt(ev.lines[line] + 5);
            _data->setByteAt(ev.lines[line] + 5, newop);

            if(oldop == 0x3C && ev.lines.size() >= line) // PLAY_CD
            {
                _data->setByteAt(ev.lines[line + 1], newop);
            }
        }
    }

    return true;
}

bool XEEN::Maze::EventList::evWHOWILL(const EventState& state)
{
    const char* msg = _parent->getString(state.getByteAt(7));
    _parent->getGame()->setEvent(new WhoWill(_parent->getGame(), state, msg));

    return false;
}
