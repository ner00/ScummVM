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
    if(valid(_data))
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
            if(!runEventLine(state))
            {
                break;
            }
        }
    }
}

bool XEEN::Maze::EventList::runEventLine(const EventState& state)
{
    XEEN_VALID();

    const uint8 opcode = state.getByteAt(5);

    const char* const names[0x3D] = 
    {
        "NOP", "Display0x01", "DoorTextSml", "DoorTextLrg", 
        "SignTextLrg", "NPC", "PlayFX", "Teleport",
        "If", "If", "If", "MoveObj",
        "TakeOrGive", "NoAction", "Remove", "SetChar",
        "Spawn", "DoTownEvent", "Exit", "AlterMap",
        "GiveTreasureChest", "ConfirmWord", "Damage", "JumpRnd",
        "AlterEvent", "CallEvent", "Return", "SetVar", 
        "TakeOrGive", "TakeOrGive", "EndClouds", "Teleport",
        "WhoWill", "RndDamage", "MoveWallObj", "AlterCellFlag",
        "AlterHed", "DisplayStat", "TakeOrGive", "SignTextSml",
        "PlayEventVoc", "DisplayBottom", "IfMapFlag", "SelRndChar",
        "GiveEnchanted", "ItemType", "MakeNothingHere", "NoAction",
        "ChooseNumeric", "DisplayBottomTwoLines", "DisplayLarge", "ExchObj",
        "FallToMap", "DisplayMain", "Goto", "ConfirmWord2",
        "GotoRandom", "EndDarkside", "EndWorld", "FlipWorld",
        "PlayCD"
    };

    debugN("%24s: ", (opcode < 0x3D) ? names[opcode] : "UNK");
    debugN("%02X  %02X %02X %02X  %02X  ", state.getByteAt(0), state.getByteAt(1), state.getByteAt(2), state.getByteAt(3), state.getByteAt(4));
    for(int i = 0; i != state.getByteAt(0) - 4; i ++)
    {
        debugN("%02X ", state.getByteAt(5 + i));
    }
    debugN("\n");

    switch(opcode)
    {
        case 0x00: { return true; }
        case 0x01: { return evMESSAGE(state); }
        case 0x02: { return evMAPTEXT(state); }
        case 0x03: { return evMAPTEXT(state); }
        case 0x04: { return evMAPTEXT(state); }
        case 0x05: { return evNPC(state); }
        case 0x06: { return true; }
        case 0x07: { return evTELEPORT(state); }
        case 0x08: { return evIF(state); }
        case 0x09: { return evIF(state); }
        case 0x0A: { return evIF(state); }
        case 0x0B: { return evMOVEOBJ(state); }
        case 0x0C: { return evGIVETAKE(state); }
        case 0x0D: { return true; }
        case 0x0E: { return evREMOVE(state); }
        case 0x0F: { return true; }
        case 0x10: { return evSPAWN(state); }
        case 0x11: { return true; }
        case 0x12: { return false; }
        case 0x13: { return evALTERMAP(state); }
        case 0x14: { return true; }
        case 0x15: { return true; }
        case 0x16: { return evDAMAGE(state); }
        case 0x17: { return true; }
        case 0x18: { return evALTEREVENT(state); }
        case 0x19: { return evCALL(state); }
        case 0x1A: { return false; }
        case 0x1B: { return true; }
        case 0x1C: { debug("UNSUPPORTED OP"); return true; }
        case 0x1D: { debug("UNSUPPORTED OP"); return true; }
        case 0x1E: { return true; }
        case 0x1F: { return evTELEPORT(state); }
        case 0x20: { return evWHOWILL(state); }
        case 0x21: { return true; }
        case 0x22: { return true; }
        case 0x23: { return evSETCELLFLAGS(state); }
        case 0x24: { return true; }
        case 0x25: { return true; }
        case 0x26: { debug("UNSUPPORTED OP"); return true; }
        case 0x27: { return evMAPTEXT(state); }
        case 0x28: { return true; }
        case 0x29: { return evMESSAGE(state); }
        case 0x2A: { return evIFMAPFLAG(state); }
        case 0x2B: { return true; }
        case 0x2C: { return true; }
        case 0x2D: { return true; }
        case 0x2E: { return evREMOVE(state); }
        case 0x2F: { return true; }
        case 0x30: { return true; }
        case 0x31: { return true; }
        case 0x32: { return true; }
        case 0x33: { return evMOVEOBJ(state); }
        case 0x34: { return true; }
        case 0x35: { return evMESSAGE(state); }
        case 0x36: { return true; }
        case 0x37: { return true; }
        case 0x38: { return true; }
        case 0x39: { return true; }
        case 0x3A: { return true; }
        case 0x3B: { return true; }
        case 0x3C: { return true; }
        default: return true;
    }
}

bool XEEN::Maze::EventList::evMAPTEXT(const EventState& state)
{
    // TODO: Add values for each opcode
    _parent->setSignMessage(_parent->getString(state.getByteAt(6)), Common::Rect(8, 8, 200, 100), 0);
    return true;
}

bool XEEN::Maze::EventList::evMESSAGE(const EventState& state)
{
    const char* msg = _parent->getString(state.getByteAt(6));
    _parent->getGame()->setEvent(new Message(_parent->getGame(), state, msg));
    return false; // TODO: ?
}

bool XEEN::Maze::EventList::evNPC(const EventState& state)
{
    // TODO
    const char* name = _parent->getString(state.getByteAt(6));
    const char* msg = _parent->getString(state.getByteAt(7));

    _parent->getGame()->setEvent(new NPC(_parent->getGame(), state, name, msg));

    return false;
}

bool XEEN::Maze::EventList::evTELEPORT(const EventState& state)
{
    // TODO: Handle map == 0 for mirrors!
    Valid<Party> p = _parent->getGame()->getParty();
    p->changeMap(state.getByteAt(6));
    p->moveTo(Common::Point(state.getByteAt(7), state.getByteAt(8)), 4);
    
    return state.getByteAt(5) == 0x07;
}

bool XEEN::Maze::EventList::evIF(const EventState& state)
{
    struct Comparinator
    {
        uint32 _type;

        Comparinator(uint32 type) : _type(type) { }
        bool operator()(int32 actual, int32 script) const
        {
            // TODO: Check logic
            switch(_type)
            {
                case 0x08: return actual > script;
                case 0x09: return actual == script;
                case 0x0A: return actual <= script;
            }

            assert(false);
        }
    };

    const Comparinator comparer(state.getByteAt(5));

    // TODO
    const uint8 valueID = state.getByteAt(6);

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
            case 0x03: result = comparer(c->getValue<uint8>(Character::SEX), state.getByteAt(7)); break;
            case 0x04: result = comparer(c->getValue<uint8>(Character::RACE), state.getByteAt(7)); break;
            case 0x05: result = comparer(c->getValue<uint8>(Character::CLASS), state.getByteAt(7)); break;
            case 0x08: result = comparer(c->getValue<int16>(Character::HP), state.getByteAt(7)); break;
            case 0x09: result = comparer(c->getValue<int16>(Character::SP), state.getByteAt(7)); break;
            case 0x0B: result = comparer(c->getStat(LEVEL).getTemp(), state.getByteAt(7)); break;
            case 0x0D: result = c->hasSkill(state.getByteAt(7)); break;
            case 0x0F: result = c->hasAward(state.getByteAt(7)); break;
            case 0x10: result = comparer(c->getValue<uint32>(Character::EXPERIENCE), state.getU32At(7)); break;
            case 0x14: result = p->getGameFlag(state.getByteAt(7)); break;
            case 0x22: result = comparer(p->getValue<uint32>(Party::GOLD), state.getByteAt(7)); break; //TODO: Size?
            case 0x23: result = comparer(p->getValue<uint32>(Party::GEMS), state.getByteAt(7)); break; //TODO: Size?

            case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2A: case 0x2B:
            {
                Statistic stat = c->getStat((Stat)(valueID - 0x25));
                result = comparer(stat.getTemp(), state.getByteAt(7));
                break;
            }
    
            case 0x2D: case 0x2E: case 0x2F: case 0x30: case 0x31: case 0x32: case 0x33:
            {
                Statistic stat = c->getStat((Stat)(valueID - 0x2D));
                result = comparer(stat.getReal(), state.getByteAt(7));
                break;
            }

            case 0x34: case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
            {
                Statistic stat = c->getStat((Stat)(FIRE + (valueID - 0x34)));
                result = comparer(stat.getReal(), state.getByteAt(7));
                break;
            }

            case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3E: case 0x3F:
            {
                Statistic stat = c->getStat((Stat)(FIRE + (valueID - 0x3A)));
                result = comparer(stat.getTemp(), state.getByteAt(7));
                break;
            }

            case 0x40: result = comparer(c->getStat(LEVEL).getReal(), state.getByteAt(7)); break;
            case 0x41: result = comparer(p->getValue<uint16>(Party::FOOD), state.getByteAt(7)); break; //TODO: Size?

            case 0x56: case 0x57: case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C:
            {
                Statistic stat = c->getStat((Stat)(valueID - 0x56));
                result = comparer(stat.getValue(), state.getByteAt(7));
                break;
            }

            case 0x63: result = p->hasSkill(state.getByteAt(7)); break;
/*          
            case 0x0A: debug("AC"); break;
            case 0x0C: debug("AGE"); break;
            case 0x11: debug("POISPROT"); break;
            case 0x12: debug("CONDITION"); break;
            case 0x13: debug("CANCAST"); break;*/

/*            case 0x15: debug("ITEM"); break;
            case 0x19: debug("MINS"); break;
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
    if(state.getByteAt(5) == 0x0B)
    {
        const Common::Point dest(state.getByteAt(7), state.getByteAt(8));
        _parent->moveObject(state.getByteAt(6), dest);
    }
    else
    {
        Valid<Objects> objs = _parent->getObjects();

        const uint32 firstID = state.getByteAt(6);
        const uint32 secondID = state.getByteAt(7);

        const ObjectEntry first = objs->getObjectData(firstID);
        const ObjectEntry second = objs->getObjectData(secondID);

        objs->moveObject(firstID, second.pos);
        objs->moveObject(secondID, first.pos);
    }
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

struct Add      { uint32 operator()(uint32 orig, uint32 mod) { return orig + mod; } };
struct Subtract { uint32 operator()(uint32 orig, uint32 mod) { return orig - mod; } };

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

    Valid<Game> g = _parent->getGame();
    Character* const c = g->getActiveCharacter();
    Party* const p = g->getParty();

    switch(take)
    {
        case 0x00: break;
        case 0x08: c->modifyValue<uint16, Subtract>(Character::HP, state.getByteAt(7)); break; // TODO: Size
        case 0x09: c->modifyValue<uint16, Subtract>(Character::SP, state.getByteAt(7)); break; // TODO: Size
        case 0x0D: c->setSkill(state.getByteAt(7), false); break;
        case 0x0F: c->setAward(state.getByteAt(7), false); break;
        case 0x10: c->modifyValue<uint32, Subtract>(Character::EXPERIENCE, state.getU32At(7)); break;
        case 0x14: p->setGameFlag(state.getByteAt(7), false); break;

        case 0x22: case 0x23:
        {
            const uint32 moddedVal = ((take == 0x22) ? +Party::GOLD : +Party::GEMS);
            const uint32 current = p->getValue<uint32>(moddedVal);
            const uint32 modifier = (take == 0x22) ? state.getU32At(7) : state.getU16At(7);

            if(modifier <= current)
            {
                p->setValue(moddedVal, current - modifier);
            }
            else
            {
                // TODO: Check messages, don't hardcode them here
                static const char* const messages[2] = {
                    "Not enough Gold in the party!", "Not enough Gems in the party!"
                };
                _parent->getGame()->setEvent(new Message(_parent->getGame(), state, messages[(take == 0x22) ? 0 : 1], true));
                return false;
            }
        }

        case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2A: case 0x2B:
        {
            Statistic stat = c->getStat((Stat)(take - 0x25));
            stat.modifyTemp(0 - state.getByteAt(7));
            break;
        }

        case 0x2D: case 0x2E: case 0x2F: case 0x30: case 0x31: case 0x32: case 0x33:
        {
            Statistic stat = c->getStat((Stat)(take - 0x2D));
            stat.modifyReal(0 - state.getByteAt(7));
            break;
        }

        case 0x34: case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
        {
            Statistic stat = c->getStat((Stat)(FIRE + (take - 0x34)));
            stat.modifyReal(0 - state.getByteAt(7));
            break;
        }

        case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3E: case 0x3F:
        {
            Statistic stat = c->getStat((Stat)(FIRE + (take - 0x3A)));
            stat.modifyTemp(state.getByteAt(7));
            break;
        }

        default: debug("Unknown Take ID: %02X", take); break;
    }

    const uint32 giveOffset = 7 + ((take < 0x70) ? valSize[take] : 1);
    const uint32 give = state.getByteAt(giveOffset);

    switch(give)
    {
        case 0x00: break;
        case 0x08: c->modifyValue<uint16, Add>(Character::HP, state.getByteAt(giveOffset + 1)); break; // TODO: Size
        case 0x09: c->modifyValue<uint16, Add>(Character::SP, state.getByteAt(giveOffset + 1)); break; // TODO: Size
        case 0x0D: c->setSkill(state.getByteAt(giveOffset + 1), true); break;
        case 0x0F: c->setAward(state.getByteAt(giveOffset + 1), true); break;
        case 0x10: c->modifyValue<uint32, Add>(Character::EXPERIENCE, state.getU32At(giveOffset + 1)); break;
        case 0x14: p->setGameFlag(state.getByteAt(giveOffset + 1), true); break;
        case 0x22: p->modifyValue<uint32, Add>(Party::GOLD, state.getU32At(giveOffset + 1)); break;
        case 0x23: p->modifyValue<uint32, Add>(Party::GEMS, state.getU16At(giveOffset + 1)); break;

        case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2A: case 0x2B:
        {
            Statistic stat = c->getStat((Stat)(give - 0x25));
            stat.modifyTemp(state.getByteAt(giveOffset + 1));
            break;
        }

        case 0x2D: case 0x2E: case 0x2F: case 0x30: case 0x31: case 0x32: case 0x33:
        {
            Statistic stat = c->getStat((Stat)(give - 0x2D));
            stat.modifyReal(state.getByteAt(giveOffset + 1));
            break;
        }

        case 0x34: case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
        {
            Statistic stat = c->getStat((Stat)(FIRE + (give - 0x34)));
            stat.modifyReal(state.getByteAt(giveOffset + 1));
            break;
        }

        case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3E: case 0x3F:
        {
            Statistic stat = c->getStat((Stat)(FIRE + (give - 0x3A)));
            stat.modifyTemp(state.getByteAt(giveOffset + 1));
            break;
        }

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
            _data->set<uint8>(ev.lines[i] + 5, 0);
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
            const uint32 oldop = _data->get<uint8>(ev.lines[line] + 5);
            _data->set<uint8>(ev.lines[line] + 5, newop);

            if(oldop == 0x3C && ev.lines.size() >= line) // PLAY_CD
            {
                _data->set<uint8>(ev.lines[line + 1], newop);
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

bool XEEN::Maze::EventList::evCALL(const EventState& state)
{
    struct CallEvent : public MazeEvent
    {
        bool done;

        CallEvent(Valid<Game> parent, const EventState& istate) : MazeEvent(parent, istate), done(false) { }
        void process()
        {
            if(!done)
            {
                done = true;

                const Common::Point pos(_state.getByteAt(6), _state.getByteAt(7));
                const Direction dir = _state.getByteAt(8);
                _state.parent->runEventAt(pos, dir);
            }
            else
            {
                setFinished(true, true);
                _state.runFrom(_state.line + 1);
            }
        }
    };

    _parent->getGame()->setEvent(new CallEvent(_parent->getGame(), state));
    return false;
}

bool XEEN::Maze::EventList::evALTERMAP(const EventState& state)
{
    // TODO: Test
    const Common::Point pos(state.getByteAt(6), state.getByteAt(7));
    const Direction dir = state.getByteAt(8);
    const uint32 value = state.getByteAt(9);

    if(enforce(value < 0x10))
    {
        _parent->setTile(pos, dir, value);
    }

    return true;
}

bool XEEN::Maze::EventList::evDAMAGE(const EventState& state)
{
    const uint16 amount = state.getU16At(6);
    const uint8 type = state.getByteAt(8);

    _parent->getGame()->getActiveCharacter()->giveDamage(amount, type);
        

    return true;
}
