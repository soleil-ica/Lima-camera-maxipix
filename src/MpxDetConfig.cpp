//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2014
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software{ you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation{ either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY{ without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program{ if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#include <iostream>
#include <limits>
#include "lima/Debug.h"
#include "lima/Exceptions.h"
#include "lima/SizeUtils.h"
#include "lima/Constants.h"
#include "MaxipixReconstruction.h"
#include "MpxDetConfig.h"
#include "MpxCommon.h"
#include "INIReader.h"

using namespace lima;
using namespace lima::Maxipix;

MpxDetConfig::MpxDetConfig(std::string path, std::string name) : m_priamPorts(), m_dacs(NULL) {
//	m_path = NULL;
	reset();
	if (!path.empty()) {
		setPath(path);
	}
	if (!name.empty()) {
		loadConfig(name);
	}
}

MpxDetConfig::~MpxDetConfig() {
}

void MpxDetConfig::reset() {
	DEB_MEMBER_FUNCT();
	m_name = "";
	m_cfgFile = "";
//	m_mpxCfg.clear();
	m_priamPorts.clear();
//	m_dacs = NULL;
}

void MpxDetConfig::setPath(const std::string& path) {
	DEB_MEMBER_FUNCT();
	if (checkPath(path)) {
		m_path= path;
	}
}

void MpxDetConfig::loadConfig(const std::string& name) {
	DEB_MEMBER_FUNCT();
	std::string cfgFile;
	getConfigFile(name, cfgFile);
	m_cfgFile= cfgFile;
	loadDetectorConfig(cfgFile);
	m_name= name;
}

void MpxDetConfig::getPath(std::string& path) const {
	path = m_path;
}

void MpxDetConfig::getName(std::string& name) const {
	name = m_name;
}

void MpxDetConfig::getFilename(std::string& cfgFile) const {
	cfgFile = m_cfgFile;
}

//void MpxDetConfig::getMpxCfg(std::map<std::string, int>& config) const {
//	config = m_mpxCfg;
//}

void MpxDetConfig::getPriamPorts(std::vector<int>& ports) const {
	ports =  m_priamPorts;
}

void MpxDetConfig::getDacs(MpxDacs*& dacs) const {
	dacs = m_dacs;
}

void MpxDetConfig::getPositionList(MaxipixReconstruction::PositionList& positions) const {
	positions = m_positions;
}

void MpxDetConfig::getConfigFile(const std::string& name, std::string& cfgFile) {
	DEB_MEMBER_FUNCT();
	cfgFile = name + ".cfg";
	if (!m_path.empty()) {
		cfgFile = m_path + "/" + cfgFile;
	}
}

void MpxDetConfig::loadDetectorConfig(std::string& fname) {
	DEB_MEMBER_FUNCT();

	INIReader reader(fname);
	if (reader.ParseError() < 0) {
		THROW_HW_ERROR(Error) << "I/O error while reading file.";
	}
	std::string type = reader.Get("config", "type", "Unknown");
	std::string version = reader.Get("config", "version", "Unknown");
	if (type != "MAXIPIX" && version != "1.0") {
		THROW_HW_ERROR(Error) << "<" << fname
				<< "> is not a valid MAXIPIX configuration file, 'version' is not 1.0 ";
	}
	parseDetModuleSection(reader);
	parseLayoutSection(reader);
	parseDacsSection(reader);
	parseCalibrationSection(reader);
}

void MpxDetConfig::parseDetModuleSection(INIReader& reader) {
	DEB_MEMBER_FUNCT();
	std::string section = "detmodule";
	//	std::map<std::string, int> m_mpxCfg;

	reader.Get(section, "connection", "Unknown"); // not used
	reader.Get(section, "sensor", "Unknown"); // not used
	reader.GetInteger(section, "bias", -1); // not used
	reader.GetInteger(section, "xpixels", -1); // not used
	reader.GetInteger(section, "ypixels", -1); // not used
	reader.GetInteger(section, "pitch", -1); // not used
	reader.GetInteger(section, "xfirst", -1); // not used
	reader.GetInteger(section, "yfirst", -1); // not used
	reader.Get(section, "name_1", "Unknown"); // not used
	reader.Get(section, "name_2", "Unknown"); // not used
	reader.Get(section, "name_3", "Unknown"); // not used
	reader.Get(section, "name_4", "Unknown"); // not used

	std::string type = reader.Get(section, "asic", "Unknown");
	convert_from_string(type, m_asicType); // Checks for validity
	//	m_mpxCfg["version"] = static_cast<int>(m_asicType);
	DEB_TRACE() << DEB_VAR2(type, m_asicType);

	std::string polarity = reader.Get(section, "polarity", "Unknown");
	convert_from_string(polarity, m_polarity); // Checks for validity
	//	m_mpxCfg["polarity"] = static_cast<int>(m_polarity);

	double value;
	getMandatoryParam(reader, section, "frequency", value);
	m_frequency = (float) value;

	getMandatoryParam(reader, section, "nchips", m_nchips);
	//	m_mpxCfg["nchips"] = m_nchips;
	DEB_TRACE() << DEB_VAR1(m_nchips);

	// the startup energy
	getMandatoryParam(reader, section, "energy", m_energy);
	//	m_mpxCfg["energy"] = m_energy;

	for (int idx = 0; idx < m_nchips; idx++) {
		std::stringstream name;
		name << "chip_" << idx + 1;
		int port;
		getMandatoryParam(reader, section, name.str(), port);
		port -= 1;
		for (std::vector<int>::iterator it = m_priamPorts.begin(); it != m_priamPorts.end(); ++it) {
			if (*it == port) {
				THROW_HW_ERROR(Error) << "In <detmodule> section " << port << " is already assigned";
			}
		}
		m_priamPorts.push_back(port);
	}
}

void MpxDetConfig::parseLayoutSection(INIReader& reader) {
	DEB_MEMBER_FUNCT();
	std::string section = "layout_standard";

	//	m_mpxCfg["positions"] = 0;

	//	MaxipixReconstruction::Layout layout;
	std::string layoutStr = reader.Get(section, "layout", "Unknown");
	convert_from_string(layoutStr, m_layout); // checks for validity
	//	m_mpxCfg["layout"] = static_cast<int>(layout);
	DEB_TRACE() << DEB_VAR1(m_layout);

	// layout paramters for standard monolithic maxipix 2x2 or 5x5 with gap reconstruction
	if (m_layout == MaxipixReconstruction::L_2x2  || m_layout == MaxipixReconstruction::L_5x1) {
		// xchips, ychips and xgap are mandatory
		Range<int>range = Range<int>(1,6);
		getMandatoryParam(reader, section, "xchips", m_xchips, range);
		//		m_mpxCfg["xchips"] = m_xchips;
		range = Range<int>(1,2);
		getMandatoryParam(reader, section, "ychips", m_ychips, range);
		//		m_mpxCfg["ychips"] = m_ychips;
		range = Range<int>(1, 5);
		getMandatoryParam(reader, section, "xgap", m_xgap, range);
		//		m_mpxCfg["xgap"] = m_xgap;
		getMandatoryParam(reader, section, "ygap", m_ygap, range);
		//		m_mpxCfg["ygap"] = m_ygap;

		reader.Get(section, "pos_1", "00"); // not used
		reader.Get(section, "pos_2", "00"); // not used
		reader.Get(section, "pos_3", "00"); // not used
		reader.Get(section, "pos_4", "00"); // not used
		reader.GetInteger(section, "rot_1",-1); // not used
		reader.GetInteger(section, "rot_2",-1); // not used
		reader.GetInteger(section, "rot_3",-1); // not used
		reader.GetInteger(section, "rot_4",-1); // not used
		// layout paramters for general reconstruction, position  are mandatory
		// included L_FREE, a faster reconstruction when there is only rotation on chips
	} else if (m_layout == MaxipixReconstruction::L_GENERAL || m_layout == MaxipixReconstruction::L_FREE) {
			parseLayoutGeneralSection(reader);
	} else if (m_layout == MaxipixReconstruction::L_NONE) {
			return;
	} else {
		THROW_HW_ERROR(Error) << "No <layout_standard> section found";
	}
}

void MpxDetConfig::parseLayoutGeneralSection(INIReader& reader) {
	DEB_MEMBER_FUNCT();
	std::string section = "layout_general";
	for (int idx = 1; idx <= m_nchips; idx++) {
		MaxipixReconstruction::Position position;

		std::stringstream rname;
		rname << "rot_" << idx;
		int rotation;
		getMandatoryParam(reader, section, rname.str(), rotation);
		RotationMode rotationMode = static_cast<RotationMode>(rotation);
		//		m_mpxCfg[rname.str()] = rotation;
		position.rotation = rotationMode;

		std::stringstream xname;
		xname << "xc_" << idx;
		int x;
		Range<int> range = Range<int>(0, 2048);
		getMandatoryParam(reader, section, xname.str(), x, range);
		//		m_mpxCfg[xname.str()] = x;
		((Point) position.origin).x = x;
		std::stringstream yname;
		yname << "yc_" << idx;
		int y;
		getMandatoryParam(reader, section, yname.str(), y, range);
		//		m_mpxCfg[yname.str()] = y;
		((Point) position.origin).y = y;

		m_positions.push_back(position);
	}
}

void MpxDetConfig::parseDacsSection(INIReader& reader) {
	DEB_MEMBER_FUNCT();
	std::string section = "dacs";
	m_dacs =  new MpxDacs(m_asicType, m_nchips);
	std::map<std::string, int> dacMap;
	std::vector<std::string> fsrKeys = MpxFsrDef::getInstance(m_asicType)->listKeys();

	for (std::vector<std::string>::iterator it = fsrKeys.begin(); it != fsrKeys.end(); ++it) {
		std::string name = *it;
		int value;
		getMandatoryParam(reader, section, name, value);
		dacMap[name] = value;
	}
	m_dacs->setDacs(0, dacMap);
}

void MpxDetConfig::parseCalibrationSection(INIReader& reader) {
	DEB_MEMBER_FUNCT();
	std::string section = "calibration";

	std::string mode = reader.Get(section, "mode", "Unknown"); // not used
	std::map<int, int> thlnoise;
	std::map<int, int> thlxray;
	for (int idx = 1; idx <= m_nchips; idx++) {
		std::stringstream noise;
		noise << "thlnoise_" << idx;
		int value;
		getMandatoryParam(reader, section, noise.str(), value);
		thlnoise[idx] = value;
		std::stringstream xray;
		xray << "thlxray_" << idx;
		getMandatoryParam(reader, section, xray.str(), value);
		thlxray[idx] = value;
	}
	double energy;
	getMandatoryParam(reader, section, "energy", energy);
	m_dacs->setThlNoise(thlnoise);
	m_dacs->setThlXray(thlxray);
	m_dacs->setEnergyCalibration(energy);
}

void MpxDetConfig::getMandatoryParam(INIReader& reader, std::string section, std::string name, int& value) {
	DEB_MEMBER_FUNCT();
	if ((value = reader.GetInteger(section, name, -999999)) == -999999) {
		THROW_HW_ERROR(Error) << "Missing mandatory parameter <" + name + "> in section <" << section << ">";
	}
}

void MpxDetConfig::getMandatoryParam(INIReader& reader, std::string section, std::string name, double& value) {
	DEB_MEMBER_FUNCT();
	if ((value = reader.GetReal(section, name, -999.999)) == -999.999) {
		THROW_HW_ERROR(Error) << "Missing mandatory parameter <" + name + "> in section <" << section << ">";
	}
}

void MpxDetConfig::getMandatoryParam(INIReader& reader, std::string section, std::string name, int& value, Range<int>& range) {
	DEB_MEMBER_FUNCT();
	if ((value = reader.GetInteger(section, name, -999999)) == -999999) {
		THROW_HW_ERROR(Error) << "Missing mandatory parameter <" + name + "> in section <" << section << ">";
	}
	if (value < range.min || value > range.max) {
		std::stringstream ss;
		ss << "Section <" << section << ">:<" << name;
		ss << "> has an invalid value. Should be in range " << range.min << " to " << range.max;
		THROW_HW_ERROR(Error) << ss.str();
	}
}

void MpxDetConfig::getMandatoryParam(INIReader& reader, std::string section, std::string name, double& value, Range<double>& range) {
	DEB_MEMBER_FUNCT();
	if ((value = reader.GetReal(section, name, -999.999)) == -999.999) {
		THROW_HW_ERROR(Error) << "Missing mandatory parameter <" + name + "> in section <" << section << ">";
	}
	if (value < range.min || value > range.max) {
		std::stringstream ss;
		ss << "Section <" << section << ">:<" << name;
		ss << "> has an invalid value. Should be in range " << range.min << " to " << range.max;
		THROW_HW_ERROR(Error) << ss.str();
	}
}
