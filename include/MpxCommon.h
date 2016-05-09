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
#ifndef MPXCOMMON_H
#define MPXCOMMON_H

#include <ostream>
#include "lima/Debug.h"
#include "lima/Exceptions.h"
#include "lima/Constants.h"
#include "lima/MiscUtils.h"
#include "MpxVersion.h"
#include "MaxipixReconstruction.h"
#include "MpxChipConfig.h"

namespace lima {
namespace Maxipix {

bool checkPath(std::string path);

const std::string convert_2_string(const Version& version);
void convert_from_string(const std::string& val,Version& version);
std::ostream& operator<<(std::ostream& os, Version& version);

const std::string convert_2_string(const Polarity& polarity);
void convert_from_string(const std::string& val, Polarity& polarity);
std::ostream& operator<<(std::ostream& os, Polarity& polarity);

const std::string convert_2_string(const MaxipixReconstruction::Layout& layout);
void convert_from_string(const std::string& val, MaxipixReconstruction::Layout& layout);
inline std::ostream& operator<<(std::ostream& os, MaxipixReconstruction::Layout& layout) {
	return os << convert_2_string(layout);
}

const std::string convert_2_string(const MaxipixReconstruction::Type& fillMode);
void convert_from_string(const std::string& val, MaxipixReconstruction::Type& fillMode);
inline std::ostream& operator<<(std::ostream& os, MaxipixReconstruction::Type& fillMode) {
	return os << convert_2_string(fillMode);
}

const std::string convert_2_string(const MpxPixelConfig::TimePixMode& mode);
void convert_from_string(const std::string& val, MpxPixelConfig::TimePixMode& mode);
inline std::ostream& operator<<(std::ostream& os, MpxPixelConfig::TimePixMode& mode) {
	return os << convert_2_string(mode);
}
} // namespace Maxipix
} // namespace lima


#endif // MPXCOMMON_H
