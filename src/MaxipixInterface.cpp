//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
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
#include "MaxipixInterface.h"

using namespace std;
using namespace lima;
using namespace lima::Maxipix;


DetInfoCtrlObj::DetInfoCtrlObj(MaxipixDet& det)
	       :m_det(det)
{
    DEB_CONSTRUCTOR();
}

DetInfoCtrlObj::~DetInfoCtrlObj()
{
    DEB_DESTRUCTOR();
}

void DetInfoCtrlObj::getMaxImageSize(Size& size)
{
    DEB_MEMBER_FUNCT();
    m_det.getImageSize(size);
}

void DetInfoCtrlObj::getDetectorImageSize(Size& size)
{
    DEB_MEMBER_FUNCT();
    m_det.getImageSize(size);
}

void DetInfoCtrlObj::getDefImageType(ImageType& image_type)
{
    DEB_MEMBER_FUNCT();
    image_type= Bpp16;
}

void DetInfoCtrlObj::getCurrImageType(ImageType& image_type)
{
    DEB_MEMBER_FUNCT();
    image_type= Bpp16;
}

void DetInfoCtrlObj::setCurrImageType(ImageType image_type)
{
    DEB_MEMBER_FUNCT();
    ImageType valid_image_type;
    getDefImageType(valid_image_type);
    if (image_type != valid_image_type)
	THROW_HW_ERROR(Error) << "Cannot change to " 
			      << DEB_VAR2(image_type, valid_image_type);
}

void DetInfoCtrlObj::getPixelSize(double& x_size,double& y_size)
{
    DEB_MEMBER_FUNCT();
    m_det.getPixelSize(x_size, y_size);
}

void DetInfoCtrlObj::getDetectorType(std::string& type)
{
    DEB_MEMBER_FUNCT();
    m_det.getDetectorType(type);
}

void DetInfoCtrlObj::getDetectorModel(std::string& model)
{
    DEB_MEMBER_FUNCT();
    m_det.getDetectorModel(model);
}

void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
    DEB_MEMBER_FUNCT();
    m_det.registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
    DEB_MEMBER_FUNCT();
    m_det.unregisterMaxImageSizeCallback(cb);
}


BufferCtrlObj::BufferCtrlObj(BufferCtrlMgr& buffer_mgr)
        : m_buffer_mgr(buffer_mgr)
{
        DEB_CONSTRUCTOR();
}

BufferCtrlObj::~BufferCtrlObj()
{
        DEB_DESTRUCTOR();
}

void BufferCtrlObj::setFrameDim(const FrameDim& frame_dim)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.setFrameDim(frame_dim);
}

void BufferCtrlObj::getFrameDim(FrameDim& frame_dim)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.getFrameDim(frame_dim);
}

void BufferCtrlObj::setNbBuffers(int nb_buffers)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.setNbBuffers(nb_buffers);
}

void BufferCtrlObj::getNbBuffers(int& nb_buffers)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.getNbBuffers(nb_buffers);
}
                
void BufferCtrlObj::setNbConcatFrames(int nb_concat_frames)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.setNbConcatFrames(nb_concat_frames);
}

void BufferCtrlObj::getNbConcatFrames(int& nb_concat_frames)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.getNbConcatFrames(nb_concat_frames);
}

void BufferCtrlObj::getMaxNbBuffers(int& max_nb_buffers)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.getMaxNbBuffers(max_nb_buffers);

}

void *BufferCtrlObj::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
        DEB_MEMBER_FUNCT();
        return m_buffer_mgr.getBufferPtr(buffer_nb, concat_frame_nb);
}

void *BufferCtrlObj::getFramePtr(int acq_frame_nb)
{
        DEB_MEMBER_FUNCT();
        return m_buffer_mgr.getFramePtr(acq_frame_nb);
}

void BufferCtrlObj::getStartTimestamp(Timestamp& start_ts)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.getStartTimestamp(start_ts);
}

void BufferCtrlObj::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.getFrameInfo(acq_frame_nb, info);
}

void BufferCtrlObj::registerFrameCallback(HwFrameCallback& frame_cb)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.registerFrameCallback(frame_cb);
}

void BufferCtrlObj::unregisterFrameCallback(HwFrameCallback& frame_cb)
{
        DEB_MEMBER_FUNCT();
        m_buffer_mgr.unregisterFrameCallback(frame_cb);
}





SyncCtrlObj::SyncCtrlObj(Espia::Acq& acq, PriamAcq& priam)
	:HwSyncCtrlObj(), 
	 m_acq(acq), 
	 m_priam(priam)
{
    DEB_CONSTRUCTOR();
    m_priam.setTimeUnit(PriamAcq::UNIT_S);
}

SyncCtrlObj::~SyncCtrlObj()
{
    DEB_DESTRUCTOR();
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
    DEB_MEMBER_FUNCT();

    // Get the Acquisition mode, some trigger mode
    // are not valid in Accumulation.
    
    AcqMode acqMode;
    this->getAcqMode(acqMode);

    bool valid_mode;
    switch (trig_mode) {
    case IntTrig:
    case IntTrigMult:
    case ExtTrigSingle:
      valid_mode = true;
      break;
    case ExtTrigMult:
    case ExtGate:
      valid_mode = (acqMode != Accumulation);
      break;
    case ExtStartStop:
      // Priam doesn not implement the start-stop mode, only Gate
    default:
	valid_mode = false;
    }
    return valid_mode;
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
    DEB_MEMBER_FUNCT();
    if (!checkTrigMode(trig_mode))
	THROW_HW_ERROR(InvalidValue) << "Invalid " << DEB_VAR1(trig_mode);
    m_priam.setTriggerMode(trig_mode);
}

void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
    DEB_MEMBER_FUNCT();
    m_priam.getTriggerMode(trig_mode);
}

void SyncCtrlObj::setExpTime(double  exp_time)
{
    DEB_MEMBER_FUNCT();
    double set_time;
    m_priam.setExposureTime(exp_time, set_time);
}
void SyncCtrlObj::getExpTime(double& exp_time)
{
    DEB_MEMBER_FUNCT();
    m_priam.getExposureTime(exp_time);
}

void SyncCtrlObj::setLatTime(double  lat_time)
{
    DEB_MEMBER_FUNCT();
    double set_time;
    m_priam.setIntervalTime(lat_time, set_time);
}
void SyncCtrlObj::getLatTime(double& lat_time)
{
    DEB_MEMBER_FUNCT();
    m_priam.getIntervalTime(lat_time);
}

void SyncCtrlObj::setNbHwFrames(int  nb_frames)
{
    DEB_MEMBER_FUNCT();
    m_acq.setNbFrames(nb_frames);

    int priamNbFrames = nb_frames;
    TrigMode trig_mode;
    m_priam.getTriggerMode(trig_mode);
    AcqMode acqMode;
    this->getAcqMode(acqMode);

    if(trig_mode == IntTrigMult &&
       acqMode != Accumulation)
      priamNbFrames = 1;

    m_priam.setNbFrames(priamNbFrames);
}
void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
    DEB_MEMBER_FUNCT();
    m_acq.getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
    DEB_MEMBER_FUNCT();

    double tmin, tmax;

    m_priam.getExposureTimeRange(tmin, tmax);
    valid_ranges.min_exp_time= tmin;
    valid_ranges.max_exp_time= tmax;

    m_priam.getIntervalTimeRange(tmin, tmax);
    valid_ranges.min_lat_time= tmin;
    valid_ranges.max_lat_time= tmax;

    DEB_RETURN() << DEB_VAR2(valid_ranges.min_exp_time, valid_ranges.max_exp_time);
    DEB_RETURN() << DEB_VAR2(valid_ranges.min_lat_time, valid_ranges.max_lat_time);
}

/*******************************************************************
 * \brief ShutterCtrlObj constructor
 *******************************************************************/

ShutterCtrlObj::ShutterCtrlObj(PriamAcq& priam)
	: m_priam(priam)
{
	DEB_CONSTRUCTOR();
}

ShutterCtrlObj::~ShutterCtrlObj()
{
	DEB_DESTRUCTOR();
}

bool ShutterCtrlObj::checkMode(ShutterMode shut_mode) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(shut_mode);

	bool valid_mode;
	switch (shut_mode) {	
	case ShutterAutoFrame:
	case ShutterAutoSequence:
		valid_mode = true;
		break;
	default:
	// No Manual mode for Maxipix !
		valid_mode = false;
	}

	DEB_RETURN() << DEB_VAR1(valid_mode);
	return valid_mode;
}

void ShutterCtrlObj::getModeList(ShutterModeList& mode_list) const
{
	DEB_MEMBER_FUNCT();
	mode_list.push_back(ShutterAutoFrame);
	mode_list.push_back(ShutterAutoSequence);
}

void ShutterCtrlObj::setMode(ShutterMode shut_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(shut_mode);

	if (!checkMode(shut_mode))
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(shut_mode);

	PriamAcq::ShutterMode cam_mode;
	cam_mode = (shut_mode == ShutterAutoFrame) ? PriamAcq::FRAME : PriamAcq::SEQUENCE;
	m_priam.setShutterMode(cam_mode);
}

void ShutterCtrlObj::getMode(ShutterMode& shut_mode) const
{
	DEB_MEMBER_FUNCT();

	PriamAcq::ShutterMode cam_mode;
	m_priam.getShutterMode(cam_mode);
	shut_mode = (cam_mode == PriamAcq::FRAME) ? ShutterAutoFrame : ShutterAutoSequence;
	DEB_RETURN() << DEB_VAR1(shut_mode);
}

void ShutterCtrlObj::setState(bool open)
{
	DEB_MEMBER_FUNCT();

	THROW_HW_ERROR(NotSupported) << "No manual mode for Maxipix";
}

void ShutterCtrlObj::getState(bool& open) const
{
	DEB_MEMBER_FUNCT();

	THROW_HW_ERROR(NotSupported) << "No manual mode for Maxipix";

}

void ShutterCtrlObj::setOpenTime(double shut_open_time)
{
	DEB_MEMBER_FUNCT();
	double settime;
			
	m_priam.setShutterTime(shut_open_time, settime);
}

void ShutterCtrlObj::getOpenTime(double& shut_open_time) const
{
	DEB_MEMBER_FUNCT();
	m_priam.getShutterTime(shut_open_time);
	DEB_RETURN() << DEB_VAR1(shut_open_time);
}

void ShutterCtrlObj::setCloseTime(double shut_close_time)
{
	DEB_MEMBER_FUNCT();
	double settime;
	m_priam.setShutterTime(shut_close_time, settime);
}

void ShutterCtrlObj::getCloseTime(double& shut_close_time) const
{
	DEB_MEMBER_FUNCT();
	m_priam.getShutterTime(shut_close_time);
}

/*******************************************************************
 * \brief Hw Interface constructor
 *******************************************************************/

Interface::Interface(Espia::Acq& acq, BufferCtrlMgr& buffer_mgr,
                     PriamAcq& priam, MaxipixDet& det)
        : m_acq(acq), m_buffer_mgr(buffer_mgr),
          m_priam(priam), m_acq_end_cb(priam), m_det_info(det), 
	  m_buffer(buffer_mgr), m_sync(acq, m_priam),
	  m_shutter(priam), m_prepare_flag(false), m_config_flag(false)
{
        DEB_CONSTRUCTOR();

	m_acq.registerAcqEndCallback(m_acq_end_cb);

       	HwDetInfoCtrlObj *det_info = &m_det_info;
        m_cap_list.push_back(HwCap(det_info));

        HwBufferCtrlObj *buffer = &m_buffer;
        m_cap_list.push_back(HwCap(buffer));

        HwSyncCtrlObj *sync = &m_sync;
        m_cap_list.push_back(HwCap(sync));

	HwShutterCtrlObj *shutter = &m_shutter;
	m_cap_list.push_back(HwCap(shutter));

        reset(SoftReset);
}

Interface::~Interface()
{
        DEB_DESTRUCTOR();
}

void Interface::getCapList(HwInterface::CapList &cap_list) const
{
        DEB_MEMBER_FUNCT();
        cap_list = m_cap_list;
}

void Interface::reset(ResetLevel reset_level)
{
        DEB_MEMBER_FUNCT();
        DEB_PARAM() << DEB_VAR1(reset_level);

	stopAcq();

        if (reset_level == HardReset) {
                DEB_TRACE() << "Performing chip hard reset";
                m_priam.resetAllChip();
        }
	m_priam.resetAllFifo();
}

void Interface::prepareAcq()
{
        DEB_MEMBER_FUNCT();
	m_prepare_flag = true;
}

void Interface::startAcq()
{
        DEB_MEMBER_FUNCT();
	AcqMode acqMode;
    	m_sync.getAcqMode(acqMode);

       	if(m_prepare_flag || acqMode == Accumulation)
	{
	  m_buffer_mgr.setStartTimestamp(Timestamp::now());
          m_acq.start();
	  m_prepare_flag = false;
	}
        m_priam.startAcq();
}

void Interface::stopAcq()
{
        DEB_MEMBER_FUNCT();
        m_priam.stopAcq();
        m_acq.stop();
	m_prepare_flag = false;
}

int Interface::getNbHwAcquiredFrames()
{
        DEB_MEMBER_FUNCT();
        Espia::Acq::Status acq_status;
        m_acq.getStatus(acq_status);
        int nb_hw_acq_frames = acq_status.last_frame_nb + 1;
        DEB_RETURN() << DEB_VAR1(nb_hw_acq_frames);
        return nb_hw_acq_frames;
}

void Interface::updateValidRanges()
{
  // Call here the HwSyncObj validRangesChanged() method about the
  // change on ranges, then the CtAcquisition callback will be activated
  // in order to update the new ranges.
   HwSyncCtrlObj::ValidRangesType validRanges;

  m_sync.getValidRanges(validRanges);
  m_sync.validRangesChanged(validRanges);
}

void Interface::getStatus(StatusType& status)
{
        DEB_MEMBER_FUNCT();
       	static const DetStatus det_mask = 
		(DetWaitForTrigger | DetExposure | DetReadout);
       	status.det_mask = det_mask;
        if (!m_config_flag) {
        	Espia::Acq::Status acq;
        	m_acq.getStatus(acq);
        	status.acq = acq.running ? AcqRunning : AcqReady;


		m_priam.getStatus(status.det);
        } else {
		status.acq = AcqConfig;
		status.det = DetIdle;
	}

        DEB_RETURN() << DEB_VAR1(status);
}

Interface::AcqEndCallback::AcqEndCallback(PriamAcq& priam)
        : m_priam(priam)
{
        DEB_CONSTRUCTOR();
}

Interface::AcqEndCallback::~AcqEndCallback()
{
        DEB_DESTRUCTOR();
}

void Interface::AcqEndCallback::acqFinished(const HwFrameInfoType& /*finfo*/)
{
        DEB_MEMBER_FUNCT();
        m_priam.stopAcq();
}


void Interface::setConfigFlag(bool flag)
{
	m_config_flag = flag;
}
