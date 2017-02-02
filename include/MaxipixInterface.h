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
#ifndef MPXINTERFACE_H
#define MPXINTERFACE_H

#include "lima/HwInterface.h"
#include "lima/HwReconstructionCtrlObj.h"
#include "EspiaBufferMgr.h"
#include "MaxipixCamera.h"
#include "lima/ThreadUtils.h"
#include "lima/Constants.h"

namespace lima {
namespace Maxipix {

/*******************************************************************
 * \class DetInfoCtrlObj
 * \brief Control object providing Maxipix detector info interface
 *******************************************************************/

class DetInfoCtrlObj: public HwDetInfoCtrlObj {
DEB_CLASS_NAMESPC(DebModCamera, "DetInfoCtrlObj", "Maxipix");

public:
	DetInfoCtrlObj(Camera& cam);
	virtual ~DetInfoCtrlObj();

	virtual void getMaxImageSize(Size& size);
	virtual void getDetectorImageSize(Size& size);

	virtual void getDefImageType(ImageType& image_type);
	virtual void getCurrImageType(ImageType& image_type);
	virtual void setCurrImageType(ImageType image_type);

	virtual void getPixelSize(double& x_size, double& y_size);
	virtual void getDetectorType(std::string& type);
	virtual void getDetectorModel(std::string& model);

	virtual void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
	virtual void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);

private:
	Camera& m_cam;
};


/*******************************************************************
 * \class SyncCtrlObj
 * \brief Control object providing Maxipix synchronization interface
 *******************************************************************/

class SyncCtrlObj: public HwSyncCtrlObj {
DEB_CLASS_NAMESPC(DebModCamera, "SyncCtrlObj", "Maxipix");

public:
	SyncCtrlObj(Camera& cam);
	virtual ~SyncCtrlObj();

	virtual bool checkTrigMode(TrigMode trig_mode);
	virtual void setTrigMode(TrigMode trig_mode);
	virtual void getTrigMode(TrigMode& trig_mode);

	virtual void setExpTime(double exp_time);
	virtual void getExpTime(double& exp_time);

	virtual void setLatTime(double lat_time);
	virtual void getLatTime(double& lat_time);

	virtual void setNbHwFrames(int nb_frames);
	virtual void getNbHwFrames(int& nb_frames);

	virtual void getValidRanges(ValidRangesType& valid_ranges);

private:
	bool _checkTrigMode(TrigMode trig_modei, bool with_acq_mode = false);
	Camera& m_cam;
};

/*******************************************************************
 * \class ShutterCtrlObj
 * \brief Control object providing shutter interface
 *******************************************************************/

class ShutterCtrlObj : public HwShutterCtrlObj
{
  DEB_CLASS_NAMESPC(DebModCamera, "ShutterCtrlObj", "Maxipix");

public:
	ShutterCtrlObj(Camera& cam);
	virtual ~ShutterCtrlObj();

	virtual bool checkMode(ShutterMode shut_mode) const;
	virtual void getModeList(ShutterModeList&  mode_list) const;
	virtual void setMode(ShutterMode  shut_mode);
	virtual void getMode(ShutterMode& shut_mode) const;

	virtual void setState(bool  shut_open);
	virtual void getState(bool& shut_open) const;

	virtual void setOpenTime (double  shut_open_time);
	virtual void getOpenTime (double& shut_open_time) const;
	virtual void setCloseTime(double  shut_close_time);
	virtual void getCloseTime(double& shut_close_time) const;

 private:
    Camera& m_cam;
};


/*******************************************************************
 * \class ReconstructionCtrlObj
 * \brief Control object providing reconstruction interface
 *******************************************************************/
class ReconstructionCtrlObj: public HwReconstructionCtrlObj {
DEB_CLASS_NAMESPC(DebModCamera, "ReconstructionCtrlObj", "Maxipix");
public:
	ReconstructionCtrlObj();
	~ReconstructionCtrlObj();

	virtual LinkTask* getReconstructionTask() {
		return m_reconstruction_task;
	}
	void setReconstructionTask(LinkTask* task);

private:
	LinkTask* m_reconstruction_task;
};


/*******************************************************************
 * \class Interface
 * \brief Maxipix hardware interface
 *******************************************************************/
class Interface: public HwInterface {
DEB_CLASS_NAMESPC(DebModCamera, "Interface", "Maxipix");

public:
 Interface(Camera& cam);
	virtual ~Interface();

	// From HwInterface
	virtual void getCapList(CapList&) const;
	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbHwAcquiredFrames();
	void setConfigFlag(bool flag);

	// Wrapping to export Camera methods
	void setPath(const std::string& path){m_cam.setPath(path);}
	void loadConfig(const std::string& name, bool reconstruction = true);

	void getFillMode(MaxipixReconstruction::Type& type) {m_cam.getFillMode(type);}
	void setFillMode(MaxipixReconstruction::Type type) {m_cam.setFillMode(type);}

	void setEnergy(double energy){m_cam.setEnergy(energy);}
	void getEnergy(double& energy){m_cam.getEnergy(energy);}

	PriamAcq* priamAcq() {return m_cam.priamAcq(); }

    //! get the camera object to access it directly from client
    Camera& getCamera() { return m_cam;}	
private:
	class _ConfigThread;
	friend class _ConfigThread;
	_ConfigThread* m_conf_thread;

	Camera& m_cam;
	CapList m_cap_list;
	DetInfoCtrlObj m_det_info;
	SyncCtrlObj m_sync;
	ShutterCtrlObj m_shutter;
	ReconstructionCtrlObj m_reconstructionCtrlObj;
	LinkTask* m_reconstructionTask;
	bool m_config_flag;
	std::string m_config_name;
	bool m_reconstuct_flag;
};

} // namespace Maxipix

} // namespace lima

#endif // MPXINTERFACE_H
