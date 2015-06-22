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
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include "lima/Debug.h"
#include "lima/Exceptions.h"
#include "MpxChipConfig.h"
#include "MpxCommon.h"

using namespace lima;
using namespace lima::Maxipix;

const int DEFLEN = 4;

struct ArrayDefs {
	std::string labels[DEFLEN];
	uint8_t mask[DEFLEN];
	uint8_t depth[DEFLEN];
	uint8_t bpcShift[DEFLEN];
};
struct ArrayDefs m_arrayDefs[] = {
	{{"Mask Array", "Test Array", "Low Threshold Array", "High Threshold Array"},
	{0x1, 0x1, 0x7, 0x7},
	{1, 1, 3, 3},
	{7, 6, 0, 3}},
	{{"Mask Array", "Test Array", "Low Threshold Array", "High Threshold Array"},
	{0x1, 0x1, 0x7, 0x7},
	{1, 1, 3, 3},
	{7, 6, 0, 3}},
	{{"Mask Array", "Test Array", "Low Threshold Array", "High Threshold Array"},
	{0x1, 0x1, 0x7, 0x7},
	{1, 1, 3, 3},
	{7, 6, 0, 3}},
	{{"Mask Array", "Test Array", "Threshold Array", "Pixel Mode Array"},
	{0x1, 0x1, 0xf, 0x3},
	{1, 1, 4, 2},
	{7, 6, 0, 4}}
};

MpxPixelConfig::MpxPixelConfig(Version version, int nchip) {
	DEB_CONSTRUCTOR();
	m_version = version;
	m_nchip = nchip;
	reset();
}

MpxPixelConfig::~MpxPixelConfig() {
	DEB_DESTRUCTOR();
	for (int idx = 0; idx < m_nchip; idx++) {
		delete[] m_pixelArray[idx];
	}
}

void MpxPixelConfig::reset() {
	DEB_MEMBER_FUNCT();
	if (!m_pixelArray.empty()) {
	  for (int idx = 0; idx < m_nchip; idx++) {
		delete [] m_pixelArray[idx];
	  }
	  m_pixelArray.clear();
	}


	std::string blankName = "";

	// should use unique_ptr here!!!!!!!!!!!!!!!!
	for (int idx = 0; idx < m_nchip; idx++) {
		MpxPixelArray* mpa = new MpxPixelArray(m_version, blankName);
		m_pixelArray.push_back(mpa);
	}
	m_name = "";
}

void MpxPixelConfig::setPath(const std::string& path) {
	DEB_MEMBER_FUNCT();
	if (checkPath(path)) {
	  m_path = path;
	}
}

void MpxPixelConfig::loadConfig(const std::string& name) {
	DEB_MEMBER_FUNCT();
	for (int idx = 0; idx < m_nchip; idx++) {
		m_pixelArray[idx]->load(getConfigFile(name, idx + 1));
	}
}

void MpxPixelConfig::getMpxString(int chipid, std::string& mpxString) {
	DEB_MEMBER_FUNCT();
	if (chipid < 1 || chipid > m_nchip) {
		THROW_HW_ERROR(Error) << "Invalid chipid <" << chipid << ">. Range is [1," << m_nchip << "]";
	}
	m_pixelArray[chipid - 1]->getMpxString(mpxString);
}

void MpxPixelConfig::getChipArray(int chipid, MpxPixelArray& pixelArray) {
	DEB_MEMBER_FUNCT();
	if (chipid < 1 || chipid > m_nchip) {
		THROW_HW_ERROR(Error) << "Invalid chipid <" << chipid << ">. Range is [1," << m_nchip << "]";
	}
	pixelArray = *m_pixelArray[chipid - 1];
}

void MpxPixelConfig::setTimePixMode(TimePixMode mode) {
	DEB_MEMBER_FUNCT();
	for (int idx = 0; idx < m_nchip; idx++) {
		m_pixelArray[idx]->setTimePixMode(mode);
	}
}

void MpxPixelConfig::getTimePixMode(TimePixMode& mode) {
	DEB_MEMBER_FUNCT();
	std::vector<TimePixMode> modes;
	for (int idx = 0; idx < m_nchip; idx++) {
		TimePixMode tpmode;
		m_pixelArray[idx]->getTimePixMode(tpmode);
		modes.push_back(tpmode);
	}
	mode = modes[0];
	if (modes.size() > 1) {
		for (unsigned idx = 1; idx < modes.size(); idx++) {
			if (modes[idx] != mode) {
				THROW_HW_ERROR(Error) << "ChipID <" << idx + 1
						<< "> does not have same mode (" << modes[idx] << "<>"
						<< mode << ")";
			}
		}
	}
}

void MpxPixelConfig::setLow2Max(int chipid) {
	DEB_MEMBER_FUNCT();
	for (int idx = 0; idx < m_nchip; idx++) {
		m_pixelArray[idx]->setLow2Max();
	}
}

void MpxPixelConfig::setLow2Min(int chipid) {
	DEB_MEMBER_FUNCT();
	for (int idx = 0; idx < m_nchip; idx++) {
		m_pixelArray[idx]->setLow2Min();
	}
}

void MpxPixelConfig::setHigh2Max(int chipid) {
	DEB_MEMBER_FUNCT();
	for (int idx = 0; idx < m_nchip; idx++) {
		m_pixelArray[idx]->setHigh2Max();
	}
}

void MpxPixelConfig::setHigh2Min(int chipid) {
	DEB_MEMBER_FUNCT();
	for (int idx = 0; idx < m_nchip; idx++) {
		m_pixelArray[idx]->setHigh2Min();
	}
}

std::string MpxPixelConfig::getConfigFile(const std::string& name, int chip) {
	DEB_MEMBER_FUNCT();
	std::string ext[] = { "edf", "bpc" };
	std::stringstream ss;
	if (!m_path.empty()) {
		ss << m_path << "/";
	}
	ss << name << "_chip_" << chip << ".";
	std::string root = ss.str();
	std::string fname;
	for (int i = 0; i < 2; i++) {
		fname = root + ext[i];
		if (access(fname.c_str(), F_OK) != -1)
			return std::string(fname);
	}
	THROW_HW_ERROR(Error) << "No config file found for <" << root << "*>";
}

/**
 * Initialisation parameters:
 * version : chip version as defined in MedipixVersion list
 * filename : if a filename is given, tries to load it in either EDF or BPC
 *          format depending on file extension
 */
MpxPixelArray::MpxPixelArray(Version& version, std::string& filename) {
	DEB_CONSTRUCTOR();
	m_version = version;
	int idx = static_cast<int>(version);
	for (int i=0; i<DEFLEN; i++) {
	  m_arrayLabels[i] = m_arrayDefs[idx].labels[i];
	  m_arrayMask[i] = m_arrayDefs[idx].mask[i];
	  m_arrayDepth[i] = m_arrayDefs[idx].depth[i];
	}
	for (int i = 0; i < DEFLEN; i++) {
		m_arrays[i] = new uint8_t[ChipSize.getWidth() * ChipSize.getHeight()];
		::memset(m_arrays[i], 0, ChipSize.getWidth() * ChipSize.getHeight());
	}
	if (!filename.empty()) {
		load(filename);
	}
}
MpxPixelArray::~MpxPixelArray() {
	DEB_DESTRUCTOR();
	for (int idx = 0; idx < DEFLEN; idx++) {
		delete[] m_arrays[idx];
	}
}

/**
 * Reset all arrays: mask, test, low and high arrays
 */
void MpxPixelArray::reset() {
	DEB_MEMBER_FUNCT();
	for (int idx = 0; idx < DEFLEN; idx++) {
		resetArray(idx);
	}
}

/**
 * Conversion into string needed for Priam transfer
 */
void MpxPixelArray::getMpxString(std::string& mpxString) {
	DEB_MEMBER_FUNCT();
	PixelConfigArray array = PixelConfigArray(m_version);
	array.maskArray = m_arrays[MASK];
	array.testArray = m_arrays[TEST];
	array.lowArray = m_arrays[LOW];
	array.highArray = m_arrays[HIGH];
	array.convert(mpxString);
}

/**
 * Saves current arrays to file.
 * Format is EDF or BPC depending on filename extension (.edf, .bpc)
 */
void MpxPixelArray::save(const std::string& filename) {
	DEB_MEMBER_FUNCT();
	int len = filename.length();
	std::string ext = filename.substr(len - 4);
	if (ext == ".edf") {
		saveEdf(filename);
	} else if (ext == ".bpc") {
		saveBpc(filename);
	} else {
		THROW_HW_ERROR(Error) << "Unknown file extension (not .edf or .bpc)";
	}
	m_filename = filename;
}

/**
 * Load arrays from file in either EDF or BPC (depending on extension .edf or .bpc)
 */
void MpxPixelArray::load(const std::string& filename) {
	DEB_MEMBER_FUNCT();
	if (access(filename.c_str(), F_OK) == -1)
		THROW_HW_ERROR(Error) << "Cannot find file <" << filename << ">";

	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	if (rc != 0 || stat_buf.st_size == 0) {
		THROW_HW_ERROR(Error) << "<" << filename << "> is empty";
	}
	int len = filename.length();
	std::string ext = filename.substr(len - 4);
	if (ext == ".edf") {
		loadEdf(filename);
	} else if (ext == ".bpc") {
		loadBpc(filename);
	} else {
		THROW_HW_ERROR(Error) << "Unknown file extension (not .edf or .bpc)";
	}
}

/**
 * Load a config file in BPC format (pixelman)
 */
void MpxPixelArray::loadBpc(const std::string& filename) {
	DEB_MEMBER_FUNCT();
	int fd;
	try {
		fd = open(filename.c_str(), O_RDONLY);
	} catch (Exception& e) {
		THROW_HW_ERROR(Error) << "Cannot open <" << filename << "> for reading";
	}
	int size = ChipSize.getWidth() * ChipSize.getHeight();
	uint8_t* data = new (std::nothrow) uint8_t[size];
	if (!data) {
		THROW_HW_ERROR(Error) << "Insufficient memory for loading file " << filename;
	}
	int nbytes = read(fd, data, size);
	close(fd);

	if (nbytes != size) {
		THROW_HW_ERROR(Error) << "<" << filename << "> has not the correct size";
	}
	uint8_t* shift = m_arrayDefs[m_version].bpcShift;
	reset();
	uint8_t* dataout = new (std::nothrow) uint8_t[size];
	if (!dataout) {
		THROW_HW_ERROR(Error) << "Insufficient memory for loading file " << filename;
	}
	for (int i = 0; i < size; i++) {
		dataout[i] = (data[i] >> shift[MASK]) & m_arrayMask[MASK];
	}
	setMaskArray(dataout);
	for (int i = 0; i < size; i++) {
		dataout[i] = (data[i] >> shift[TEST]) & m_arrayMask[TEST];
	}
	setTestArray(dataout);
	for (int i = 0; i < size; i++) {
		dataout[i] = (data[i] >> shift[HIGH]) & m_arrayMask[HIGH];
	}
	setHighArray(dataout);
	for (int i = 0; i < size; i++) {
		dataout[i] = data[i] & m_arrayMask[LOW];
	}
	setLowArray(dataout);
	delete[] data;
	delete[] dataout;
}

/**
 *
 */
void MpxPixelArray::saveBpc(const std::string& filename) {
	DEB_MEMBER_FUNCT();
	int size = ChipSize.getWidth() * ChipSize.getHeight();
	uint8_t* shift = m_arrayDefs[m_version].bpcShift;
	uint8_t* data = new (std::nothrow) uint8_t[size]();
	if (!data) {
		THROW_HW_ERROR(Error) << "Insufficient memory for saving file " << filename;
	}
	for (int i = 0; i < size; i++) {
		data[i] |= ((m_arrays[MASK][i] & m_arrayMask[MASK]) << shift[MASK]);
		data[i] |= ((m_arrays[TEST][i] & m_arrayMask[TEST]) << shift[TEST]);
		data[i] |= ((m_arrays[HIGH][i] & m_arrayMask[HIGH]) << shift[HIGH]);
		data[i] |= (m_arrays[LOW][i] & m_arrayMask[LOW]);
	}
	std::ofstream fout;
	fout.clear();
	fout.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	fout.open(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
	fout.write((char*) data, size);
	fout.close();
}

/**
 *
 */
void MpxPixelArray::loadEdf(const std::string& filename) {
	DEB_MEMBER_FUNCT();
	//Assumptions: As we save the header in 1024 bytes we will only read a header of that size
	// but at least test to check for the '}'. The data type will be uint8_t and size determined
	// by the chipsize.
	int headerSize = 1024;
	std::ifstream file(filename.c_str(), std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
	if (file.is_open()) {
		long fileSize = file.tellg();
		file.seekg(0, std::ios::beg);
		if (fileSize != 4 * headerSize * ChipSize.getWidth() * ChipSize.getHeight()) {
			THROW_HW_ERROR(Error) << "<" << filename << "> does not contain 4 images";
		}
		reset();
		char *hdr = new char[headerSize];
		long size = ChipSize.getWidth() * ChipSize.getHeight();
		uint8_t *data = new uint8_t[size];
		for (int idx = 0; idx < 4; idx++) {
			file.read(hdr, headerSize);
			if (hdr[1022] == '}') {
				THROW_HW_ERROR(Error) << "<" << filename << "> is not in EDF format";
			}
			file.read((char*) data, size);
			setArray(idx, data);
		}
		file.close();
		delete[] hdr;
		delete[] data;
	} else {
		THROW_HW_ERROR(Error) << "Unable to open file <" << filename << ">";
	}
}

/**
 * Saves current config file in EDF format
 */
void MpxPixelArray::saveEdf(const std::string& filename) {
	DEB_MEMBER_FUNCT();
	std::ofstream fout;
	fout.clear();
	fout.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	fout.open(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
	for (int idx = 0; idx < 4; idx++) {
		std::stringstream header;
		header << "{\n";
		header << "HeaderID = EH:00000" << idx << ":000000:000000 ;\n"; // this line only applicable upto idx=9
		header << "Image = " << idx << " ;\n";
		header << "ByteOrder = LowByteFirst ;\n";
		header << "DataType = UnsignedByte ;\n";
		header << "Dim_1 = " << ChipSize.getWidth() << " ;\n";
		header << "Dim_2 = " << ChipSize.getHeight() << " ;\n";
		header << "Size = " << ChipSize.getWidth() * ChipSize.getHeight()
				<< " ;\n";
		header << "MedipixConfig = " << m_arrayLabels[idx] << " ;\n";
		int hsize = header.str().length();
		int ssize = 1024 - hsize % 1024 - 2;
		if (ssize < 0)
			ssize += 1024;
		if (ssize)
			for (int i = 0; i < ssize; i++)
				header << " ";
		header << "}\n";
		fout.write((char*) m_arrays[idx],
				ChipSize.getWidth() * ChipSize.getHeight());
	}
	fout.close();
}

/**
 * Load an EDF mask file and sets maskArray (edf with array of values 0/1)
 */
void MpxPixelArray::loadMask(const std::string& filename) {
	DEB_MEMBER_FUNCT();
	//Assumptions: As we save the mask header in 1024 bytes we will only read a header of that size
	// but at least test to check for the '}'. The data type will be uint8_t and size determined
	// by the chipsize.
	int headerSize = 1024;
	std::ifstream file(filename.c_str(), std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
	if (file.is_open()) {
		long fileSize = file.tellg();
		file.seekg(0, std::ios::beg);
		if (fileSize != headerSize * ChipSize.getWidth() * ChipSize.getHeight())
			THROW_HW_ERROR(Error) << "<" << filename << "> does not contain 4 images";
		resetMaskArray();
		char *hdr = new char[headerSize];
		long size = ChipSize.getWidth() * ChipSize.getHeight();
		uint8_t *data = new uint8_t[size];
		file.read(hdr, headerSize);
		if (hdr[1022] == '}') {
			THROW_HW_ERROR(Error) << "<" << filename << "> is not in EDF format";
		}
		file.read((char*) data, size);
		setMaskArray(data);
		file.close();
		delete[] hdr;
		delete[] data;
	} else {
		THROW_HW_ERROR(Error) << "Unable to open file <" << filename << ">";
	}
}

/**
 * Saves current mask array in a separate EDF file
 */
void MpxPixelArray::saveMask(const std::string& filename) {
	DEB_MEMBER_FUNCT();
	std::ofstream fout;
	fout.clear();
	fout.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	fout.open(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
	std::stringstream header;
	header << "{\n";
	header << "HeaderID = EH:000001:000000:000000 ;\n"; // this line only applicable upto idx=9
	header << "Image = 1 ;\n";
	header << "ByteOrder = LowByteFirst ;\n";
	header << "DataType = UnsignedByte ;\n";
	header << "Dim_1 = " << ChipSize.getWidth() << " ;\n";
	header << "Dim_2 = " << ChipSize.getHeight() << " ;\n";
	header << "Size = " << ChipSize.getWidth() * ChipSize.getHeight() << " ;\n";
	header << "MedipixConfig = " << m_arrayLabels[MASK] << " ;\n";
	int hsize = header.str().length();
	int ssize = 1024 - hsize % 1024 - 2;
	if (ssize < 0) {
		ssize += 1024;
	}
	if (ssize) {
		for (int i = 0; i < ssize; i++) {
			header << " ";
		}
	}
	header << "}\n";
	fout.write((char*) getMaskArray(), ChipSize.getWidth() * ChipSize.getHeight());
	fout.close();
}

/**
 * Set timepix mode for all pixels.
 */
void MpxPixelArray::setTimePixMode(std::string modeStr) {
	MpxPixelConfig::TimePixMode mode;
	convert_from_string(modeStr, mode);
	setArrayValue(HIGH, mode);
}

void MpxPixelArray::setTimePixMode(MpxPixelConfig::TimePixMode mode) {
	DEB_MEMBER_FUNCT();

	if (convert_2_string(mode) == "Unknown") {
		THROW_HW_ERROR(Error) << "Invalid TimePixMode. Should be in [0..3]";
	}
	setArrayValue(HIGH, mode);
}

/**
 * Get current timepix mode and check mode consistency
 */
void MpxPixelArray::getTimePixMode(MpxPixelConfig::TimePixMode& mode) {
	DEB_MEMBER_FUNCT();
	uint8_t* harr = getHighArray();
	int imode = harr[0];
	long size = ChipSize.getWidth() * ChipSize.getHeight();
	int sum = 0;
	for (int idx = 0; idx < size; idx++) {
		if (harr[idx] != imode) {
			sum += harr[idx];
		}
	}
	if (sum > 0) {
		THROW_HW_ERROR(Error) << "TimePixMode array has different values";
	}
	mode = static_cast<MpxPixelConfig::TimePixMode>(imode);
}

void MpxPixelArray::setLow2Max() {
	DEB_MEMBER_FUNCT();
	// Set low threshold array (lowArray) to its max value
	setArrayValue(LOW, m_arrayDefs[m_version].mask[LOW]);
}

void MpxPixelArray::setLow2Min() {
	DEB_MEMBER_FUNCT();
	// Set low threshold array (lowArray) to its min value
	setArrayValue(LOW, 0);
}

void MpxPixelArray::setHigh2Max() {
	DEB_MEMBER_FUNCT();
	// Set high threshold array (lowArray) to its max value
	setArrayValue(HIGH, m_arrayDefs[m_version].mask[HIGH]);
}

void MpxPixelArray::setHigh2Min() {
	DEB_MEMBER_FUNCT();
	// Set high threshold array (lowArray) to its min value
	setArrayValue(HIGH, 0);
}

void MpxPixelArray::setArrayValue(int index, uint8_t value) {
	DEB_MEMBER_FUNCT();
	::memset(m_arrays[index], value, ChipSize.getWidth() * ChipSize.getHeight());
}

uint8_t* MpxPixelArray::getMaskArray() {
	DEB_MEMBER_FUNCT();
	return getArray(MASK);
}

uint8_t* MpxPixelArray::getTestArray() {
	DEB_MEMBER_FUNCT();
	return getArray(TEST);
}

uint8_t* MpxPixelArray::getLowArray() {
	DEB_MEMBER_FUNCT();
	return getArray(LOW);
}

uint8_t* MpxPixelArray::getHighArray() {
	DEB_MEMBER_FUNCT();
	return getArray(HIGH);
}

uint8_t* MpxPixelArray::getArray(int index) {
	DEB_MEMBER_FUNCT();
	return m_arrays[index];
}

void MpxPixelArray::setMaskArray(uint8_t* data) {
	DEB_MEMBER_FUNCT();
	setArray(MASK, data);
}

void MpxPixelArray::setTestArray(uint8_t* data) {
	DEB_MEMBER_FUNCT();
	setArray(TEST, data);
}

void MpxPixelArray::setLowArray(uint8_t* data) {
	DEB_MEMBER_FUNCT();
	setArray(LOW, data);
}

void MpxPixelArray::setHighArray(uint8_t* data) {
	DEB_MEMBER_FUNCT();
	setArray(HIGH, data);
}

void MpxPixelArray::setArray(int index, uint8_t* data) {
	DEB_MEMBER_FUNCT();
	long size = ChipSize.getWidth() * ChipSize.getHeight();
//	uint8_t* bptr = m_arrays[index];
//	uint8_t* bptr2 = m_arrayMask[index];
	for (int idx = 0; idx < size; idx++) {
		m_arrays[index][idx] |= (data[idx]); // & bptr2[idx]);
	}
}

void MpxPixelArray::resetMaskArray() {
	DEB_MEMBER_FUNCT();
	resetArray(MASK);
}

void MpxPixelArray::resetTestArray() {
	DEB_MEMBER_FUNCT();
	resetArray(TEST);
}

void MpxPixelArray::resetLowArray() {
	DEB_MEMBER_FUNCT();
	resetArray(LOW);
}

void MpxPixelArray::resetHighArray() {
	DEB_MEMBER_FUNCT();
	resetArray(HIGH);
}

void MpxPixelArray::resetArray(int index) {
	DEB_MEMBER_FUNCT();
	::memset(m_arrays[index], 0, ChipSize.getWidth() * ChipSize.getHeight());
}

