//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2014
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#ifndef MPXCHIPCONFIG_H
#define MPXCHIPCONFIG_H

#include <vector>
#include <stdint.h>
#include "lima/HwInterface.h"
#include "PixelArray.h"
#include "MpxVersion.h"

namespace lima {
namespace Maxipix {

const int MASK = 0;
const int TEST = 1;
const int LOW = 2;
const int HIGH = 3;

class MpxPixelArray;

class MpxPixelConfig {
	DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Maxipix");
public:

	enum TimePixMode {MEDIPIX, TOT, TIMEPIX_LESS_1HIT, TIMEPIX};

	MpxPixelConfig(Version version, int nchip);
	~MpxPixelConfig();
	void reset();
	void setPath(const std::string& path);
	void loadConfig(const std::string& name);
	void getMpxString(int chipid, std::string& mpxString);
	void getChipArray(int chipid, MpxPixelArray& pixelArray);
	void setTimePixMode(TimePixMode mode);
	void getTimePixMode(TimePixMode& mode);
	void setLow2Max(int chipid);
	void setLow2Min(int chipid);
	void setHigh2Max(int chipid);
	void setHigh2Min(int chipid);
private:
	Version m_version;
	int m_nchip;
	std::string m_path;
	std::string m_name;
	std::vector<MpxPixelArray> m_array;

	std::string getConfigFile(const std::string& name, int chip);
};

class MpxPixelArray {
	DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Maxipix");
public:

	MpxPixelArray(Version& version, std::string& filename);
	~MpxPixelArray();
	void reset();
	void getMpxString(std::string& mpxString);
	void save(const std::string& filename);
	void load(const std::string& filename);
	void loadBpc(const std::string& filename);
	void saveBpc(const std::string& filename);
	void loadEdf(const std::string& filename);
	void saveEdf(const std::string& filename);
	void loadMask(const std::string& filename);
	void saveMask(const std::string& filename);
	void setTimePixMode(std::string mode);
	void setTimePixMode(MpxPixelConfig::TimePixMode mode);
	void getTimePixMode(MpxPixelConfig::TimePixMode& mode);
	void setLow2Max();
	void setLow2Min();
	void setHigh2Max();
	void setHigh2Min();

private:
	Version m_version;
	uint8_t* m_arrays[4];
	std::string m_filename;
	std::string m_arrayLabels[4];
	uint8_t m_arrayMask[4];
	uint8_t m_arrayDepth[4];

	void setArrayValue(int index, uint8_t value);
	uint8_t* getMaskArray();
	uint8_t* getTestArray();
	uint8_t* getLowArray();
	uint8_t* getHighArray();
	uint8_t* getArray(int index);
	void setMaskArray(uint8_t* data);
	void setTestArray(uint8_t* data);
	void setLowArray(uint8_t* data);
	void setHighArray(uint8_t* data);
	void setArray(int index, uint8_t* data);
	void resetMaskArray();
	void resetTestArray();
	void resetLowArray();
	void resetHighArray();
	void resetArray(int index);
};

} // namespace Maxipix
} // namespace lima

#endif // MPXCHIPCONFIG_H
