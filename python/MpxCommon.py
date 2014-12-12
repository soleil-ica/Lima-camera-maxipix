############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2014
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
from Lima import Core
from Lima import Espia
from limamaxipix import Maxipix

import types

MpxVersion= [	Maxipix.MaxipixDet.DUMMY,
		Maxipix.MaxipixDet.MPX2,
		Maxipix.MaxipixDet.MXR2,
		Maxipix.MaxipixDet.TPX1 ]
MpxTypes= ["DUMMY", "MPX2", "MXR2", "TPX1"]

MpxPolarity= [	Maxipix.MaxipixDet.NEGATIVE,
		Maxipix.MaxipixDet.POSITIVE ]
MpxPolarityTypes= ["NEGATIVE","POSITIVE"]

MpxRotationTypes=[0,1,2,3,-1,-2,-3]

MpxRotation= {0: Core.Rotation_0,
              1: Core.Rotation_90,
              2: Core.Rotation_180,
              3: Core.Rotation_270,
              -1: Core.Rotation_270,
              -2: Core.Rotation_180,
              -3: Core.Rotation_90
              }
MpxLayout= [Maxipix.MaxipixReconstruction.L_NONE,
            Maxipix.MaxipixReconstruction.L_2x2,
            Maxipix.MaxipixReconstruction.L_5x1,
            Maxipix.MaxipixReconstruction.L_FREE,
            Maxipix.MaxipixReconstruction.L_GENERAL]
MpxLayoutTypes= ["L_NONE", "L_2X2", "L_5X1", "L_FREE", "L_GENERAL"]
             
def mpxPolarity(polarity):
    if type(polarity)==types.StringType:
	if polarity.upper() not in MpxPolarityTypes:
	    raise MpxError("Invalid Maxipix Polarity String <%s>"%polarity)
	return MpxPolarity[MpxPolarityTypes.index(polarity.upper())]
    elif type(polarity)==types.IntType:
	if polarity not in range(len(MpxPolarity)):
	    raise MpxError("Invalid Maxipix Polarity value <%d>"%polarity)
	return MpxPolarity[polarity]
    else:	
	if polarity not in MpxPolarity:
	    raise MpxError("Invalid Maxipix Polarity <%s>"%str(polarity))
	return polarity

def mpxVersion(version):
    if type(version)==types.StringType:
	if version.upper() not in MpxTypes:
	    raise MpxError("Invalid Maxipix Version string <%s>"%version)
	return MpxVersion[MpxTypes.index(version.upper())]
    elif type(version)==types.IntType:
	if version not in range(len(MpxVersion)):
	    raise MpxError("Invalid Maxipix Version value <%d>"%version)
	return MpxVersion[version]
    else:	
	if version not in MpxVersion:
	    raise MpxError("Invalid Maxipix Version <%s>"%str(version))
	return version

def mpxLayout(layout):
    if type(layout)==types.StringType:
	if layout.upper() not in MpxLayoutTypes:
	    raise MpxError("Invalid Maxipix Layout string <%s>"%layout)
	return MpxLayout[MpxLayoutTypes.index(layout.upper())]
    elif type(layout)==types.IntType:
	if layout not in range(len(MpxLayout)):
	    raise MpxError("Invalid Maxipix Layout value <%d>"%layout)
	return MpxLayout[layout]
    else:	
	if layout not in MpxLayout:
	    raise MpxError("Invalid Maxipix Layout <%s>"%str(layout))
	return layout

class MpxError(Exception):
    def __init__(self, msg):
        self.message= msg
    def __str__(self):
        return self.message

