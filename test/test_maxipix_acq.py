
# python version
#from Lima.Maxipix.MpxAcq import MpxAcq
#C++ version
from Lima import Maxipix
from Lima.Core import DebParams
from Lima import Core
import time

#DebParams.setTypeFlags(DebParams.AllFlags)
#DebParams.setModuleFlags(DebParams.AllFlags)
#DebParams.setFormatFlags(DebParams.AllFlags)

#DebParams.setModuleFlags(Ac0xffffb7)

cam = Maxipix.Camera(0,'config', 'tpxatl25', True)
hwint = Maxipix.Interface(cam)
  
control = Core.CtControl(hwint)
acq = control.acquisition()


# setting new file parameters and autosaving mode
saving=control.saving()

pars=saving.getParameters()
pars.directory='./'
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
acq_pars.acqNbFrames = 10

acq.setPars(acq_pars)
# here we should have 21 accumalated images per frame
print acq.getAccNbFrames()

#set sps display
display=control.display()
display.setNames('_lima_sps_','lima_sps')
display.setActive(1)

#prepare acquisition
control.prepareAcq()

d=raw_input("Image can be displayed using oxdis on sps lima_sps, press any key to continue: ")


#start acquisition
control.startAcq()


while control.getStatus().ImageCounters.LastImageSaved != 9 :
    time.sleep(1)
    print "Last Image saved: ", control.getStatus().ImageCounters.LastImageSaved
    
    

d=raw_input ("Press any key to quit now: ")
#before exiting delete the top control objects !
del control
del hwint
del cam




