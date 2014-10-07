############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2011
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
import os
import libconfig
import MpxDacs
from MpxCommon import *

class MpxDetConfig:

    def __init__(self, path=None, name=None):
	self.path= None
	self.reset()

	if path is not None:
	    self.setPath(path)
	if name is not None:
	    self.loadConfig(name)

    def reset(self):
	self.name= None
	self.cfgFile= None

	self.mpxCfg= None
	self.priamPorts= None
	self.dacs= None

    def setPath(self, path):
	spath= os.path.normpath(path)
	if not os.path.isdir(spath):
	    raise MpxError("<%s> is not a directory"%path)
	if not os.access(spath, os.R_OK):
	    raise MpxError("no read permission on <%s>"%path)
	self.path= spath

    def loadConfig(self, name):
	cfgFile= self.__getConfigFile(name)
	self.loadDetectorConfig(cfgFile)
	self.cfgFile= cfgFile
	self.name= name

    def __getConfigFile(self, name):
	fname= "%s.cfg"%name
	if self.path is not None:
	    fname= "%s/%s"%(self.path, fname)
	return fname

    def __checkConfigFile(self, fname):
	if not os.path.isfile(fname):
            raise MpxError("<%s> is not a valid file"%fname)
        if not os.access(fname, os.R_OK):
            raise MpxError("No read permission on <%s>"%fname)

        try:
            cfg = libconfig.Config(fname)
            cfg.load()
        except RuntimeError:
            raise MpxError("<%s> is not a valid libconfig file"%fname)

        try:
            config = cfg['config']
        except KeyError:
            raise MpxError("<%s> is not a valid maxipix file"%fname)
        
        return cfg

    def __setParamError(self, msg):
	txt = "ConfigFile <%s>"%self.cfgFile
	if self.__section is not None:
	    txt= txt + " : Section <%s>"%self.__section
	txt= txt + " : " + msg
	raise MpxError(txt)

    def __getParamNeeded(self, pars, name, chklist=None):
        try:
            param = pars[name]
        except KeyError:
	    self.__setParamError("Missing mandatory parameter <%s>"%name)
            return None
	if chklist is not None:
	    if param not in chklist:
		self.__setParamError("<%s> has an invalid value. Should be in %s"%(name, str(chklist)))
	return param

    def __getParamOptional(self, pars, name, chklist=None, default=None):
        try:
            param = pars[name]
        except KeyError:
	    if default is not None:
		return default
	    else:
	        return None
	if chklist is not None:
	    if param not in chklist:
		self.__setParamError("<%s> has an invalid value. Should be in %s"%(name, str(chklist)))
	return param

    def loadDetectorConfig(self, fname):
	cfg = self.__checkConfigFile(fname)

	self.__section= None
	self.__parseDetModuleSection(cfg)
        self.__parseLayoutStardardSection(cfg)
	self.__parseDacsSection(cfg)
	self.__parseCalibrationSection(cfg)

    def __parseDetModuleSection(self, cfg):
        try:
            pars = cfg["detmodule"]
        except KeyError:
	    self.__setParamError("No <detmodule> section found")
            
	self.__section= "detmodule"        
	self.mpxCfg= {}
	self.mpxCfg["asic"]= self.__getParamNeeded(pars, "asic", MpxTypes)
	self.mpxCfg["version"]= mpxVersion(self.mpxCfg["asic"])
	self.mpxCfg["polarity"]= mpxPolarity(self.__getParamNeeded(pars, "polarity", MpxPolarityTypes))
	self.mpxCfg["frequency"]= self.__getParamNeeded(pars, "frequency")
	self.mpxCfg["nchips"]= self.__getParamNeeded(pars, "nchips")

	self.priamPorts= range(self.mpxCfg["nchips"])

        for idx in range(self.mpxCfg["nchips"]):
            name= "chip_%d"%(idx+1)
            self.priamPorts[idx]= self.__getParamOptional(pars, name, 
                                                          range(5), self.priamPorts[idx]) -1 

    def __parseLayoutStardardSection(self, cfg):
        try:
            pars = cfg['layout_standard']
        except KeyError:
            self.__setParamError('No <layout_standard> section found')

        self.__section= 'layout_standard'
	self.mpxCfg["xchips"]= self.__getParamNeeded(pars, "xchips", range(1,6))
	self.mpxCfg["ychips"]= self.__getParamOptional(pars, "ychips", [1,2], 1)

	self.mpxCfg["xgap"]= self.__getParamOptional(pars, "xgap", None, 0)
	self.mpxCfg["ygap"]= self.__getParamOptional(pars, "ygap", None, 0)

        # set the per chip rotation if any, only valid for maxipix without gap reconstruction
        self.mpxCfg["rotations"]=[]
        if self.mpxCfg["xgap"] == 0 and self.mpxCfg["ygap"] ==0:
            for idx in range(self.mpxCfg["nchip"]):
                name= "rot_%d"%(idx+1)
                self.mpxCfg[name]= self.__getParamOptional(pars, name, 
                                                          MpxRotationTypes, 0)
                self.mpxCfg["rotations"].append(MpxRotation[self.mpxCfg[name]])
            
            
    def __parseDacsSection(self, cfg):
        try:
            pars = cfg['dacs']
        except KeyError:
            self.__setParamError("No <dacs> section found")
            
	self.__section= "dacs"
	self.dacs= MpxDacs.MpxDacs(self.mpxCfg["version"], self.mpxCfg["nchips"])
        setDacs= {}
        fsrKeys= MpxDacs.getMpxFsrDef(self.mpxCfg["version"]).listKeys()
        for key in pars.keys():
            if key not in fsrKeys:
                self.__setParamError("Invalid key <%s>"%key)
            else:
                setDacs[key]= pars[key]
                
        self.dacs.setDacs(0, setDacs)


    def __parseCalibrationSection(self, cfg):
        try:
            pars = cfg['calibration']
        except KeyError:
            self.__setParamError('No <calibration> section found')
        
        self.__section= 'calibration'
        thlnoise = []; thlxray = []
        for idx in range(self.mpxCfg['nchips']):
            name= "thlnoise_%d"%(idx+1)
            thlnoise.append(self.__getParamOptional(pars, name, None, 0))
            name = 'thlxray_%d'%(idx+1)
            thlxray.append(self.__getParamOptional(pars, name, None, 0))
        energy = self.__getParamOptional(pars, 'energy', None, 0)

        self.dacs.setThlNoise(thlnoise)
        self.dacs.setThlXray(thlxray)
        self.dacs.setEnergyCalibration(energy)

    def getPath(self):
	return self.path

    def getFilename(self):
	return self.cfgFile

    def getName(self):
	return self.name

    def getMpxCfg(self):
	return self.mpxCfg

    def getPriamPorts(self):
	return self.priamPorts

    def getDacs(self):
	return self.dacs

if __name__=="__main__":
    import sys
    if len(sys.argv)!=3:
	print "Usage: %s <path> <config_name>"%sys.argv[0]
    else:
	def printDict(pars):
	    for (key,val) in pars.items():
		print "\t", key, "=", val
	    print

	cfg= MpxDetConfig(path= sys.argv[1], name= sys.argv[2])
	print
	print "> Path       =", cfg.getPath()
	print "> ConfigName =", cfg.getName()
	print "> FileName   =", cfg.getFilename()
	print 
	print "[detmodule]"
	printDict(cfg.getMpxCfg())
	print "ports =", str(cfg.getPriamPorts())
	print "[calibration]"
	print "thlnoise = ", str(cfg.getDacs().getThlNoise())
        print "thlxray = ", str(cfg.getDacs().getThlXray())
        
	print "energy calibration = ", cfg.getDacs().getEnergyCalibration()
        print "energy = ", cfg.getDacs().getEnergy()
	print "[dacs]"
	printDict(cfg.getDacs().getDacs(0))

