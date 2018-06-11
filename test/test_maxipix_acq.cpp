#include <unistd.h>
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

      //DebParams::setModuleFlags(DebParams::AllFlags);
      //DebParams::setTypeFlags(DebParams::AllFlags);
      //DebParams::setFormatFlags(DebParams::AllFlags);

      string path = "config/";
      string filename = "tpxatl25";

      m_camera = new Camera(0, path, filename, true);
      m_interface = new Interface(*m_camera);
      m_control = new CtControl(m_interface);

      // next 2 lines here only for debugging
      CtAcquisition* m_ct_acq = m_control->acquisition();
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

      int nframes = 10;
      // setup fileformat and data saving info
      CtSaving* saving = m_control->saving();
      saving->setDirectory("./");
      saving->setFramesPerFile(nframes);
      saving->setFormat(CtSaving::EDF);
      saving->setPrefix("maxipix_");
      saving->setSuffix(".edf");
      saving->setSavingMode(CtSaving::AutoFrame);
      saving->setOverwritePolicy(CtSaving::Overwrite);

      // do acquisition
      cout << "Will acquire 10 frames 1 sec. exposure and save in EDF format, overwrite mode and single file maxipix_0000.edf" << endl;
      m_control->acquisition()->setAcqExpoTime(1.0);
      m_control->acquisition()->setLatencyTime(0.01);
      m_control->acquisition()->setAcqNbFrames(nframes);
      m_control->prepareAcq();
      m_control->startAcq();
      CtControl::ImageStatus im_status;
      while(1) {
        m_control->getImageStatus(im_status);
        cout << "Acquired " << im_status.LastImageAcquired+1 <<  " Saved  " << im_status.LastImageSaved+1 << "\r";
	cout.flush();
	sleep(1);
	if (im_status.LastImageSaved == 9)
	  break;
      }
      cout << "Acquired " << im_status.LastImageAcquired+1 <<  " Saved  " << im_status.LastImageSaved+1 << endl;
      cout << ">>>>> Done ! <<<<<" << endl;
      delete m_control;
      delete m_interface;
      delete m_camera;
      return(1);
}
