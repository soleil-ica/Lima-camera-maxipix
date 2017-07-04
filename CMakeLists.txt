############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2017
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

SET(maxipix_srcs src/PriamSerial.cpp  src/PriamAcq.cpp
	 src/PixelArray.cpp src/MaxipixReconstruction.cpp
	 src/MaxipixCamera.cpp src/MaxipixInterface.cpp
	 src/MpxDetConfig.cpp src/MpxCommon.cpp src/MpxChipConfig.cpp src/MpxDacs
	 tools/src/INIReader.cpp tools/ini.c)

FILE(STRINGS "VERSION" maxipix_vers)
ADD_LIBRARY(limamaxipix SHARED ${maxipix_srcs})

TARGET_INCLUDE_DIRECTORIES(limamaxipix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES(limamaxipix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tools/src")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/cmake ${CMAKE_MODULE_PATH})
include(FindEspiaDrv)

#set paths for espia driver and limaespialib and include files
TARGET_INCLUDE_DIRECTORIES(limamaxipix PUBLIC "${ESPIA_DRV_INCLUDE_DIR}")
TARGET_INCLUDE_DIRECTORIES(limamaxipix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/include")

LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../common/espia")

TARGET_LINK_LIBRARIES(limamaxipix limacore)
TARGET_LINK_LIBRARIES(limamaxipix ${ESPIA_DRV_LIB})
TARGET_LINK_LIBRARIES(limamaxipix limaespia)

SET_TARGET_PROPERTIES(limamaxipix PROPERTIES VERSION ${maxipix_vers} SOVERSION "1")

INSTALL(TARGETS limamaxipix LIBRARY DESTINATION lib)

IF(COMPILE_SIP)
    SET(NAME "maxipix")
    SET(IMPORTS
"%Import limaespia.sip")
    SET(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/sip"
	"${CMAKE_BINARY_DIR}/camera/common/espia/sip")
    INCLUDE(CameraRunSIP)
    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Maxipix")
ENDIF()

IF(COMPILE_TEST)
	ADD_SUBDIRECTORY(test/)
ENDIF()
