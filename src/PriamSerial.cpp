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

#include "PriamSerial.h"
#include <sstream>

using namespace std;
using namespace lima;
using namespace lima::Maxipix;

const PriamSerial::PriamCodeType PriamSerial::PriamRegCode[] = {
    {(short)PR_MCR1, "mcr1", 0x00, 1, 0x80, 1},
    {(short)PR_MCR2, "mcr2", 0x01, 1, 0x81, 1},
    {(short)PR_BID, "bid", 0xff, 0, 0x82, 1},
    {(short)PR_MSR, "msr", 0x03, 1, 0x83, 1},
    {(short)PR_ET2, "et1", 0x04, 1, 0x84, 1},
    {(short)PR_ET1, "et2", 0x05, 1, 0x85, 1},
    {(short)PR_ROM, "rom", 0x06, 1, 0x86, 1},
    {(short)PR_AOP, "aop", 0x07, 1, 0x87, 1},
    {(short)PR_LUTAD1, "lutad1", 0x08, 1, 0x88, 1},
    {(short)PR_LUTAD2, "lutad2", 0x09, 1, 0x89, 1},
    {(short)PR_TP, "tp", 0x20, 1, 0xa0, 1},
    {(short)PR_INB1, "inb1", 0x22, 1, 0xa2, 1},
    {(short)PR_INB2, "inb2", 0x23, 1, 0xa3, 1},
    {(short)PR_IT1, "it1", 0x24, 1, 0xa4, 1},
    {(short)PR_IT2, "it2", 0x25, 1, 0xa5, 1},
    {(short)PR_OSC, "osc", 0x26, 2, 0xff, 0},
    {(short)PR_TAS, "tas", 0x27, 1, 0xa7, 1},
    {(short)PR_TAP, "tap", 0x28, 1, 0xa8, 1},
    {(short)PR_ST1, "st1", 0x2a, 1, 0xaa, 1},
    {(short)PR_ST2, "st2", 0x2b, 1, 0xab, 1},
    {(short)PR_TIP, "tip", 0x2c, 1, 0xac, 1},
    {(short)PR_CFG, "cfg", 0x2d, 1, 0xad, 1},
    {(short)PR_MPINIT, "mpinit", 0x2e, 1, 0xff, 0},
    {(short)PR_MPSEL, "mpsel", 0x2f, 1, 0xff, 0},
    {(short)PR_OWT1, "owt1", 0x14, 1, 0x94, 1},
    {(short)PR_OWT2, "owt2", 0x15, 1, 0x95, 1},
    {(short)PR_OWTR, "owtr", 0x16, -1, 0x96, -1},
    {(short)PR_OWCR, "owcr", 0x17, 1, 0x97, 1},
    {(short)PR_SPISIZ, "spisiz", 0x18, 1, 0x98, 1},
    {(short)PR_SPICTZ, "spictz", 0x19, 1, 0x99, 1},
    {(short)PR_SPITZ, "spitz", 0x1a, -1, 0x9a, -1},
};

const PriamSerial::PriamCodeType PriamSerial::PriamLutCode[] = {
    {(short)PLUT_CC, "cc", 0x0a, -1, 0x8a, -1},
    {(short)PLUT_FF1, "ff1", 0x0b, -1, 0x8b, -1},
    {(short)PLUT_FF2, "ff2", 0x0c, -1, 0x8c, -1},
    {(short)PLUT_FF3, "ff3", 0x0d, -1, 0x8d, -1},
    {(short)PLUT_FF4, "ff4", 0x0e, -1, 0x8e, -1},
    {(short)PLUT_FF5, "ff5", 0x0f, -1, 0x8f, -1}
};

const PriamSerial::PriamCodeType PriamSerial::PriamSerTxCode[] = { {
		(short) PSER_MATRIX, "matrix", 0x10, 114688, 0x90, 114688 }, {
		(short) PSER_FSR, "FSR", 0x91, 32, 0xff, 3 } };

const double PriamSerial::ResetLinkWaitTime = 5;

PriamSerial::PriamSerial(Espia::SerialLine &espia_serial) :
		m_espia_serial(espia_serial), m_mutex(MutexAttr::Normal) {
	DEB_CONSTRUCTOR();
	ostringstream os;
	os << "Dev#" << espia_serial.getDev().getDevNb();
	DEB_SET_OBJ_NAME(os.str());

	int chan_up_led;
	espia_serial.getDev().getChanUpLed(chan_up_led);
	// Sometimes the link can be down for any reason, so try first to restart it
	if (!chan_up_led) {
		DEB_WARNING() << "Aurora link down. Forcing a link reset!";
		espia_serial.getDev().resetLink();
		DEB_TRACE() << "Sleeping additional "
			    << DEB_VAR1(Maxipix::PriamSerial::ResetLinkWaitTime);
		Sleep(ResetLinkWaitTime);
	}


	DEB_TRACE() << "Flushing serial line ...";
	// In case of remaining chars in TX (after detector disconnected and tries of connection),
	// Espia sends those chars to the priam which can answer on RX.
	// Making no more synchro in between cmds and answers. So flush is always mandatory for fresh start!!
	m_espia_serial.flush();
}

PriamSerial::~PriamSerial() {
	DEB_DESTRUCTOR();
}

void PriamSerial::writeRegister(PriamRegister reg, const string& buffer) {
	DEB_MEMBER_FUNCT();

	PriamCodeType wreg;
	string rbuf;

	wreg = PriamRegCode[reg];

	if (wreg.writeCode == 0xff)
		THROW_HW_ERROR(InvalidValue) << "Priam register " << reg << " ("
				<< wreg.name << ") is not writable";

	if ((wreg.writeSize > 0)
			&& (buffer.size() != (unsigned long) wreg.writeSize))
		THROW_HW_ERROR(InvalidValue) << "Wrong buffer size for Priam register";

	DEB_TRACE() << "write" << DEB_VAR2(wreg.name, buffer);

	// Lock here the serial write/read acess to the priam to avoid deadlock due to concurrent access
	AutoMutex lock(m_mutex);
	_writeCommand(wreg.writeCode, buffer);
	_readAnswer(wreg.writeCode, 0, rbuf);
}

void PriamSerial::readRegister(PriamRegister reg, string& buffer,
		long size) const {
	DEB_MEMBER_FUNCT();

	long rsize;
	PriamCodeType rreg;

	rreg = PriamRegCode[reg];

	if (rreg.readCode == 0xff)
		THROW_HW_ERROR(InvalidValue) << "Priam register " << reg << " ("
				<< rreg.name << ") is not readable";

	DEB_TRACE() << "read" << DEB_VAR2(rreg.name, size);

	// Lock here the serial write/read acess to the priam to avoid deadlock due to concurrent access
	AutoMutex lock(m_mutex);

	_writeCommand(rreg.readCode, buffer);

	if (rreg.readSize == -1) {
		rsize = size;
	} else {
		rsize = rreg.readSize;
	}

	_readAnswer(rreg.readCode, rsize, buffer);

	if (buffer.size() != (unsigned long) rsize)
		THROW_HW_ERROR(Error) << "Priam return " << DEB_VAR1(buffer.size())
				<< "is not correct; should be " << DEB_VAR1(rsize);
}

void PriamSerial::_writeCommand(short code, const string &inbuf) const {
	DEB_MEMBER_FUNCT();
	string wbuf;

	wbuf.assign(1, (char) code);

	if (inbuf.size() > 0) {
		m_espia_serial.write(wbuf, false);
		m_espia_serial.write(inbuf, true);
	} else {
		m_espia_serial.write(wbuf, true);
	}
}

void PriamSerial::_readAnswer(short code, long size, string &rbuf) const {
	DEB_MEMBER_FUNCT();
	double tout;
	short iret;
	string sret("");

	m_espia_serial.read(sret, 1, 0.2);
	if (sret.size() == 0) {
		THROW_HW_ERROR(Error) << "No answer from priam";
	}
	iret = sret.at(0) & 0xff;
	if (iret == SERIAL_ERR) {
		m_espia_serial.flush();
		THROW_HW_ERROR(Error) << "Priam serial error";
	}
	if (iret == SERIAL_BAD) {
		m_espia_serial.flush();
		THROW_HW_ERROR(Error) << "Priam command not authorized";
	}
	if (iret != code) {
		m_espia_serial.flush();
		THROW_HW_ERROR(Error) << "Priam code not replyed";
	}

	if (size > 0) {
		DEB_TRACE() << "read answer" << DEB_VAR1(size);
		tout = ((int) (size / 1024) + 1) * 1.0;
		m_espia_serial.read(rbuf, size, tout);
	}

	m_espia_serial.read(sret, 1, 1.0);
	iret = sret.at(0) & 0xff;
	if (iret != SERIAL_END) {
		m_espia_serial.flush();
		THROW_HW_ERROR(Error) << "Priam end of transfer not received";
	}
}

void PriamSerial::writeFsr(const string& fsr, string& bid) {
	DEB_MEMBER_FUNCT();
	PriamCodeType reg;

	reg = PriamSerTxCode[PSER_FSR];
	if (fsr.size() != (unsigned long) reg.writeSize)
		THROW_HW_ERROR(InvalidValue) << "Wrong " << DEB_VAR1(fsr.size())
			<< " for Priam transfer; should be " << DEB_VAR1(long(reg.writeSize));

	// Lock here the serial write/read acess to the priam to avoid deadlock due to concurrent access
	AutoMutex lock(m_mutex);

	_writeCommand(reg.writeCode, fsr);
	_readAnswer(reg.writeCode, reg.readSize, bid);
}

void PriamSerial::writeMatrix(const string& input) {
	DEB_MEMBER_FUNCT();

	PriamCodeType reg;
	string rbuf("");

	reg = PriamSerTxCode[PSER_MATRIX];
	if (input.size() != (unsigned long) reg.writeSize)
		THROW_HW_ERROR(InvalidValue) << "Wrong " << DEB_VAR1(input.size())
			<< " for Priam transfer; should be " << DEB_VAR1(long(reg.writeSize));

	DEB_TRACE() << "Writing matrix";
	_writeCommand(reg.writeCode, input);
	DEB_TRACE() << "Handshake Writing matrix";
	_readAnswer(reg.writeCode, 0, rbuf);
}

void PriamSerial::readMatrix(string& output) const {
	DEB_MEMBER_FUNCT();

	PriamCodeType reg;
	string wbuf("");

	// Lock here the serial write/read acess to the priam to avoid deadlock due to concurrent access
	AutoMutex lock(m_mutex);

	reg = PriamSerTxCode[PSER_MATRIX];
	DEB_TRACE() << "Asking matrix";
	_writeCommand(reg.readCode, wbuf);
	DEB_TRACE() << "Reading matrix";
	_readAnswer(reg.readCode, reg.readSize, output);
}

void PriamSerial::writeLut(PriamLut lut, const string& buffer) {
	DEB_MEMBER_FUNCT();

	PriamCodeType reg;
	string wbuf("");
	unsigned int size;

	reg = PriamLutCode[lut];
	size = buffer.size();
	if (size > 256)
		THROW_HW_ERROR(InvalidValue) << "Wrong lookup string " << DEB_VAR1(size) << ", should be <= 256";

	wbuf.append(1, (char) reg.writeCode);
	wbuf.append(1, (char) size);

	// Lock here the serial write/read acess to the priam to avoid deadlock due to concurrent access
	AutoMutex lock(m_mutex);

	m_espia_serial.write(wbuf, true);
	m_espia_serial.write(buffer, false);
	_readAnswer(reg.writeCode, 0, wbuf);
}

void PriamSerial::readLut(PriamLut lut, string& buffer, long size) const {
	DEB_MEMBER_FUNCT();

	PriamCodeType reg;
	string wbuf("");

	if (size > 256)
		THROW_HW_ERROR(InvalidValue) << "Wrong lookup string " << DEB_VAR1(size) << ";should be <= 256";
	wbuf.append(1, (char) reg.readCode);
	wbuf.append(1, (char) (size & 0xff));

	// Lock here the serial write/read acess to the priam to avoid deadlock due to concurrent access
	AutoMutex lock(m_mutex);

	m_espia_serial.write(wbuf, false);
	_readAnswer(reg.readCode, size, buffer);
}

