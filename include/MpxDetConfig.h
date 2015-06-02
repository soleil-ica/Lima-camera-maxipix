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
#ifndef MPXDETCONFIG_H
#define MPXDETCONFIG_H

#include "INIReader.h"
#include "lima/Debug.h"
#include "lima/Exceptions.h"
#include "lima/Constants.h"
#include "MpxDacs.h"

namespace lima {
namespace Maxipix {

template<class T>
class Range{
public:
	Range(T amin, T amax) : min(amin), max(amax) {}
	T min;
	T max;
 };

class MpxDetConfig {
	DEB_CLASS_NAMESPC(DebModCamera, "Camera", "MpxDetConfig");
public:
	MpxDetConfig(std::string path="", std::string name="");
	~MpxDetConfig();
    void reset();
	void setPath(std::string path);
	void loadConfig(std::string name);

    void getPath(std::string& path) const;
	void getName(std::string& name) const;
	void getFilename(std::string& filename) const;
	void getMpxCfg(std::map<std::string, int>& config) const;
	void getPriamPorts(std::vector<int>& ports) const;
	void getDacs(MpxDacs* dacs) const;
	void getPositionList(MaxipixReconstruction::PositionList& positions) const;
    void loadDetectorConfig(std::string& fname);
    void getFrequency(float& frequency) { frequency = m_frequency; }
	void getConfigFile(const std::string& name, std::string& cfgFile);

private:
	std::string m_path;
	std::string m_name;
	std::string m_cfgFile;
	float m_frequency;
	std::map<std::string, int> m_mpxCfg;
	std::vector<int> m_priamPorts;
	MpxDacs *m_dacs;
	MaxipixReconstruction::PositionList m_positions;
	std::string m_section;

	void parseDetModuleSection(INIReader& reader);
	void parseLayoutSection(INIReader& reader);
	void parseLayoutGeneralSection(INIReader& reader);
	void parseDacsSection(INIReader& reader);
	void parseCalibrationSection(INIReader& reader);

	void getMandatoryParam(INIReader& reader, std::string section, std::string name, int& value);
	void getMandatoryParam(INIReader& reader, std::string section, std::string name, double& value);
	void getMandatoryParam(INIReader& reader, std::string section, std::string name, int& value, Range<int>& range);
	void getMandatoryParam(INIReader& reader, std::string section, std::string name, double& value, Range<double>& range);
};

} // namespace Maxipix
} // namespace lima

#endif // MPXDETCONFIG_H
