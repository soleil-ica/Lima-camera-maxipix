//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2015
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

#include <sstream>
#include <cstdio>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lima/Debug.h"
#include "lima/Exceptions.h"
#include "MaxipixCamera.h"
#include "MpxCommon.h"

using namespace std;
using namespace lima;
using namespace lima::Maxipix;

namespace lima {
namespace Maxipix {

typedef pair<int, RotationMode> RotationPair;
static const RotationPair RotationCList[] = {
		RotationPair(0, Rotation_0),
		RotationPair(1, Rotation_90),
		RotationPair(2, Rotation_180),
		RotationPair(3, Rotation_270),
		RotationPair(-1, Rotation_270),
		RotationPair(-2, Rotation_180),
		RotationPair(-3, Rotation_90),
};
std::map<int, RotationMode> MpxRotation(C_LIST_ITERS(RotationCList));

bool checkPath(std::string path) {
	struct stat statbuf;

	if (::stat(path.c_str(), &statbuf) != -1) {
		if (S_ISDIR(statbuf.st_mode)) {
			return true;
		}
	} else {
		std::stringstream ss;
		if (errno == ENOENT) {
			ss << "<" << path << "> does not exist";
			throw LIMA_EXC(lima::CameraPlugin, Error, ss.str());
		} else if (errno == ENOTDIR) {
			ss << "<" << path << "> is not a directory";
			throw LIMA_EXC(lima::CameraPlugin, Error, ss.str());
		} else if (errno == EACCES) {
			ss << "no read permission on <" << path << ">";
			throw LIMA_EXC(lima::CameraPlugin, Error, ss.str());
		}
		return false;
	}
	return false;
}

const string convert_2_string(const Version& version) {
	string name;
	switch (version) {
	case MPX2: name = "MPX2"; break;
	case MXR2: name = "MXR2"; break;
	case TPX1: name = "TPX1"; break;
	case DUMMY: name = "DUMMY"; break;
	default: name = "Unknown"; break;
	}
	return name;
}
inline std::ostream& operator<<(std::ostream& os, Version& version) {
	return os << lima::Maxipix::convert_2_string(version);
}

void convert_from_string(const std::string& val, Version& version) {
	std::string buffer = val;
	std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::toupper);
	if (buffer == "MPX2") {
		version = MPX2;
	} else if (buffer == "MXR2") {
		version = MXR2;
	} else if (buffer == "TPX1") {
		version = TPX1;
	} else if (buffer == "DUMMY") {
		version = DUMMY;
	} else {
		std::stringstream msg;
		msg << "MaxipixCamera::Version can't be:" << DEB_VAR1(val);
		throw LIMA_EXC(lima::CameraPlugin, InvalidValue, msg.str());
	}
}

const string convert_2_string(const Polarity& polarity) {
	string name;
	switch (polarity) {
	case NEGATIVE: name = "NEGATIVE"; break;
	case POSITIVE: name = "POSITIVE"; break;
	default: name = "Unknown"; break;
	}
	return name;
}
inline std::ostream& operator<<(std::ostream& os, Polarity& polarity) {
	return os << lima::Maxipix::convert_2_string(polarity);
}

void convert_from_string(const std::string& val, Polarity& polarity) {
	std::string buffer = val;
	std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::toupper);
	if (buffer == "NEGATIVE") {
		polarity = NEGATIVE;
	} else if (buffer == "POSITIVE") {
		polarity = POSITIVE;
	} else {
		std::stringstream msg;
		msg << "MaxipixCamera::Polarity can't be:" << DEB_VAR1(val);
		throw LIMA_EXC(lima::CameraPlugin, InvalidValue, msg.str());
	}
}

const string convert_2_string(const MaxipixReconstruction::Type& fillMode) {
	string name;
	switch (fillMode) {
	case MaxipixReconstruction::RAW: name = "RAW"; break;
	case MaxipixReconstruction::ZERO: name = "ZERO"; break;
	case MaxipixReconstruction::DISPATCH: name = "DISPATCH"; break;
	case MaxipixReconstruction::MEAN: name = "MEAN"; break;
	default: name = "Unknown"; break;
	}
	return name;
}

void convert_from_string(const std::string& val, MaxipixReconstruction::Type& fillMode) {
	std::string buffer = val;
	std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::toupper);
	if (buffer == "RAW") {
		fillMode = MaxipixReconstruction::RAW;
	} else if (buffer == "ZERO") {
		fillMode = MaxipixReconstruction::ZERO;
	} else if (buffer == "DISPATCH") {
		fillMode = MaxipixReconstruction::DISPATCH;
	} else if (buffer == "MEAN") {
		fillMode = MaxipixReconstruction::MEAN;
	} else {
		std::stringstream msg;
		msg << "MaxipixReconstruction::Type can't be:" << DEB_VAR1(val);
		throw LIMA_EXC(lima::CameraPlugin, InvalidValue, msg.str());
	}
}

const string convert_2_string(const MaxipixReconstruction::Layout& layout) {
	string name;
	switch (layout) {
	case MaxipixReconstruction::L_NONE: name = "L_NONE"; break;
	case MaxipixReconstruction::L_2x2: name = "L_2X2"; break;
	case MaxipixReconstruction::L_5x1: name = "L_5X1"; break;
	case MaxipixReconstruction::L_FREE: name = "L_FREE"; break;
	case MaxipixReconstruction::L_GENERAL: name = "L_GENERAL"; break;
	default: name = "Unknown"; break;
	}
	return name;
}

void convert_from_string(const std::string& val, MaxipixReconstruction::Layout& layout) {
	std::string buffer = val;
	std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::toupper);
	if (buffer == "L_NONE") {
		layout = MaxipixReconstruction::L_NONE;
	} else if (buffer == "L_2X2") {
		layout = MaxipixReconstruction::L_2x2;
	} else if (buffer == "L_5X1") {
		layout = MaxipixReconstruction::L_5x1;
	} else if (buffer == "L_FREE") {
		layout = MaxipixReconstruction::L_FREE;
	} else if (buffer == "L_GENERAL") {
		layout = MaxipixReconstruction::L_GENERAL;
	} else {
		std::stringstream msg;
		msg << "MaxipixReconstruction::Layout can't be:" << DEB_VAR1(val);
		throw LIMA_EXC(lima::CameraPlugin, InvalidValue, msg.str());
	}
}
const string convert_2_string(const MpxPixelConfig::TimePixMode& mode) {
	string name;
	switch (mode) {
	case MpxPixelConfig::MEDIPIX: name = "medipix"; break;
	case MpxPixelConfig::TOT: name = "tot"; break;
	case MpxPixelConfig::TIMEPIX_LESS_1HIT: name = "timepix-1hit"; break;
	case MpxPixelConfig::TIMEPIX: name = "timepix"; break;
	default: name = "Unknown"; break;
	}
	return name;
}

void convert_from_string(const std::string& val, MpxPixelConfig::TimePixMode& mode) {
	std::string buffer = val;
	std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::tolower);
	if (buffer == "medipix") {
		mode = MpxPixelConfig::MEDIPIX;
	} else if (buffer == "tot") {
		mode = MpxPixelConfig::TOT;
	} else if (buffer == "timepix-1hit") {
		mode = MpxPixelConfig::TIMEPIX_LESS_1HIT;
	} else if (buffer == "timepix") {
		mode = MpxPixelConfig::TIMEPIX;
	} else {
		std::stringstream msg;
		msg << "MpxPixelConfig::TimePixMode can't be:" << DEB_VAR1(val);
		throw LIMA_EXC(lima::CameraPlugin, InvalidValue, msg.str());
	}
}

}; //namespace maxipix
}; //namespace lima
