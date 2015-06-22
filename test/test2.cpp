#include "lima/CtControl.h"
#include "lima/CtSaving.h"
#include "lima/CtAcquisition.h"
#include "lima/HwInterface.h"

#include "MaxipixCamera.h"
#include "MaxipixReconstruction.h"
#include "MaxipixInterface.h"



using namespace lima;
using namespace lima::Maxipix;
using namespace std;

DEB_GLOBAL(DebModTest);

int main()
{
      DEB_GLOBAL_FUNCT();

      Camera *m_camera;
      Interface *m_interface;
      CtControl* m_control;

      //      DebParams::setModuleFlags(DebParams::AllFlags);
      //      DebParams::setTypeFlags(DebParams::AllFlags);
      //      DebParams::setFormatFlags(DebParams::AllFlags);

      string p= "/users/blissadm/local/maxipix/tpxatl25/";
      string u = "tpxatl25";

      m_camera = new Camera(0, p, u, true);
      m_interface = new Interface(*m_camera);
      m_control = new CtControl(m_interface);

      // next 4 lines here only for debugging
      //m_camera->setExpTime(2.0);
      CtAcquisition* m_ct_acq = m_control->acquisition();
      CtImage* m_ct_image = m_control->image();
      m_ct_acq->setAcqExpoTime(2.0);

      string type;
      m_camera->getDetectorType(type);
      cout << "Detector type : " << type << endl;
      string model;
      m_camera->getDetectorModel(model);
      cout << "Detector model: " << model << endl;

      HwInterface::StatusType status;
      m_interface->getStatus(status);
      cout << "Detector Status: " << status << endl;

      int nframes = 2;
      // setup fileformat and data saving info
      CtSaving* saving = m_control->saving();
      saving->setDirectory("/users/gmant");
      saving->setFramesPerFile(nframes);
      saving->setFormat(CtSaving::EDF);
      saving->setPrefix("maxipix_");
      saving->setSuffix(".edf");
      saving->setSavingMode(CtSaving::AutoFrame);
      saving->setOverwritePolicy(CtSaving::Append);

      // do acquisition
      m_control->acquisition()->setAcqExpoTime(2.0);
      m_control->acquisition()->setLatencyTime(0.01);
      m_control->acquisition()->setAcqNbFrames(nframes);
      m_control->prepareAcq();
      m_control->startAcq();
      while(1) {
	cout << "sleeping ...." <<  endl;
	usleep(100);
	if (!m_camera->isAcqRunning())
	  break;
      }
      sleep(5); //all done!

      return(1);
}
