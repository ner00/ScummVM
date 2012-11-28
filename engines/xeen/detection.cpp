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

#include "engines/advancedDetector.h"
#include "common/system.h"

#include "base/plugins.h"

#include "xeen/xeen.h"

static const PlainGameDescriptor testbed_setting[] = {
	{ "xeen", "Might and Magic" },
	{ 0, 0 }
};

static const ADGameDescription testbedDescriptions[] = {
	{
		"xeen",
		"",
		AD_ENTRY1("XEEN.CC", 0),	// Game-data file for detection
		Common::EN_ANY,
		Common::kPlatformPC,
		ADGF_NO_FLAGS,
		GUIO0()
	},
	AD_TABLE_END_MARKER
};

class XeenMetaEngine : public AdvancedMetaEngine {
public:
	XeenMetaEngine() : AdvancedMetaEngine(testbedDescriptions, sizeof(ADGameDescription), testbed_setting) {
		_md5Bytes = 512;
		_singleid = "xeen";
	}

	virtual const char *getName() const {
		return "Might and Magic";
	}

	virtual const char *getOriginalCopyright() const {
		return "Copyright (C) New World Computing";
	}

	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription * /* desc */) const {
		// Instantiate Engine even if the game data is not found.
		*engine = new XEEN::XeenEngine(syst);
		return true;
	}

};

#if PLUGIN_ENABLED_DYNAMIC(XEEN)
	REGISTER_PLUGIN_DYNAMIC(XEEN, PLUGIN_TYPE_ENGINE, XeenMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(XEEN, PLUGIN_TYPE_ENGINE, XeenMetaEngine);
#endif
