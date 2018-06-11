.. _camera-maxipix:

Maxipix
-------

.. image:: Maxipix.png

Intoduction
```````````
MAXIPIX is a high spatial resolution (small pixels), high frame rate, photon-counting pixel detector developed by ESRF. MAXIPIX is based on MEDIPIX2/TIMEPIX readout ASICs developed by CERN and the MEDIPIX2 collaboration. The active detector element consists of a hybrid pixel circuit glued on a chipboard and connected to it with microwire connections. The hybrid pixel circuit consists itself of a pixelated semiconductor sensor connected to one or several readout ASICs by individual micro solder bumps on each pixel. Various module formats are available and may implement either MEDIPIX2 or TIMEPIX ASICs. Both ASICs have identical pixel geometries but different characteristics as regards principally the lowest energy threshold, the discriminator range, and the available detection modes.

We provide today Maxipix 5x1, 4x1 and 1x1 formats based on both TIMEPIX and MEDIPIX2 ASICs.

Beamlines are equiped with the detector, Espia card and a specific computer running centOS 5 x86_64.

Installation & Module configuration
````````````````````````````````````

Follow the generic instructions in :ref:`build_installation`. If using CMake directly, add the following flag:

.. code-block:: sh

 -DLIMACAMERA_MAXIPIX=true

For the Tango server installation, refers to :ref:`tango_installation`.

Initialisation and Capabilities
````````````````````````````````

Implementing a new plugin for new detector is driven by the LIMA framework but the developer has some freedoms to choose which standard and specific features will be made available. This section is supposed to give you the correct information regarding how the camera is exported within the LIMA framework.

Camera initialisation
......................

The camera will be initialized within the :cpp:class:`Maxipix::Camera`  class. Camera contructor aims to load the configuration and calibration data to the detector backend electronic (Priam card).

There are so many hardware parameters you can set, but refer to the maxipix documentation for a good pratice.

  set/getSignalLevel()
  set/getReadLevel()
  set/getTriggerLevel()
  set/getShutterLevel()
  set/getReadyMode()
  set/getGateMode()
  set/getFillMode()
  set/getEnergy()

Std capabilites
................

This plugin has been implemented in respect of the mandatory capabilites but with some limitations which are due to the camera. We only provide here extra information for a better understanding
of the capabilities for Maxipix cameras.

* HwDetInfo

  getCurrImageType/getDefImageType(): always 16bpp.

  setCurrImageType(): this method do not change the image type which is fixed to 16bpp.

* HwSync

  get/setTrigMode():  supported modes are IntTrig, IntTrigMult,ExtTrigSingle, ExtTrigMult and ExtGate.


Optional capabilites
........................
In addition to the standard capabilities, we make the choice to implement some optional capabilities which
are supported by this detector. A Shutter control.

* HwShutter

  setMode(): only ShutterAuto and ShutterManual modes are supported.

Configuration
`````````````

Only provided configuration files (``.cfg`` and ``.bpc``) must be used for your detector, you must not change those files. Each detector has its own set of files. Please contact ESRF Detector group for help.

How to use
````````````

This is a python code example of a simple acquisition:

.. code-block:: python

  from Lima.Maxipix import Maxipix
  from lima import Core

  #------------------------------------------------------------------------+
  #                                             config name (.cfg file)    |
  #------------------------+                                               |
  #     config path        |                                               |
  #------------------- +   |                                               |
  #    espia channel   |   |                                               |
  #                    v   v                                               v
  cam = Maxipix.Camera(0, '/users/blissadm/local/maxipix/calib/tpxatl25', 'tpxatl25X')

  hwint = Maxipix.Interface(cam)
  ct = Core.CtControl(hwint)

  acq = ct.acquisition()

  # set some low level configuration
  # see maxipix documentationf for more information
  hwint.setEnergyThreshold(10.0)
  hwint.setFillMode(cam.DISPATCH)
  hwint.setShutterLevel(cam.HIGH_RISE)


  # setting new file parameters and autosaving mode
  saving=ct.saving()

  pars=saving.getParameters()
  pars.directory='/buffer/lcb18012/opisg/test_lima'
  pars.prefix='test1_'
  pars.suffix='.edf'
  pars.fileFormat=Core.CtSaving.EDF
  pars.savingMode=Core.CtSaving.AutoFrame
  saving.setParameters(pars)

  # set accumulation mode

  acq_pars= acq.getPars()

  #0-normal,1-concatenation,2-accumu
  acq_pars.acqMode = 2
  acq_pars.accMaxExpoTime = 0.05
  acq_pars.acqExpoTime =1
  acq_pars.acqNbFrames = 1

  acq.setPars(acq_pars)
  # here we should have 21 accumalated images per frame
  print acq.getAccNbFrames()

  # now ask for 2 sec. exposure and 10 frames
  acq.setAcqExpoTime(2)
  acq.setNbImages(10)

  ct.prepareAcq()
  ct.startAcq()

  # wait for last image (#9) ready
  lastimg = ct.getStatus().ImageCounters.LastImageReady
  while lastimg !=9:
    time.sleep(1)
    lastimg = ct.getStatus().ImageCounters.LastImageReady

  # read the first image
  im0 = ct.ReadImage(0)
