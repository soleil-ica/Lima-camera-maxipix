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
SET(NAME "maxipix")

SET(${NAME}_srcs src/PriamSerial.cpp  src/PriamAcq.cpp
	 src/PixelArray.cpp src/MaxipixReconstruction.cpp
	 src/MaxipixCamera.cpp src/MaxipixInterface.cpp
	 src/MpxDetConfig.cpp src/MpxCommon.cpp src/MpxChipConfig.cpp src/MpxDacs
	 tools/src/INIReader.cpp tools/ini.c)

ADD_LIBRARY(lima${NAME} SHARED ${${NAME}_srcs})

TARGET_INCLUDE_DIRECTORIES(lima${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES(lima${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tools/src")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/cmake ${CMAKE_MODULE_PATH})
include(FindEspiaDrv)

#set paths for espia driver and limaespialib and include files
TARGET_INCLUDE_DIRECTORIES(lima${NAME} PUBLIC "${ESPIA_DRV_INCLUDE_DIR}")
TARGET_INCLUDE_DIRECTORIES(lima${NAME}PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/include")

LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../common/espia")

TARGET_LINK_LIBRARIES(lima${NAME} limacore)
TARGET_LINK_LIBRARIES(lima${NAME} ${ESPIA_DRV_LIB})
TARGET_LINK_LIBRARIES(lima${NAME} limaespia)

include(CameraVERSION)
INSTALL(TARGETS lima${NAME} LIBRARY DESTINATION lib)

IF(LIMA_ENABLE_PYTHON)
    SET(INCLUDES)
    FILE(GLOB sipfiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/sip" "${CMAKE_CURRENT_SOURCE_DIR}/sip/*.sip")
    FOREACH(sipfile ${sipfiles})
        SET(INCLUDES 
"${INCLUDES} 
%Include ${sipfile}"
)
    ENDFOREACH()

    SET(IMPORTS
"%Import limacore.sip
%Import limaespia.sip"
)

    IF(SIP_VERSION_STR VERSION_LESS "4.12")
    	CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sip/limamodules_before_4_12.sip.in sip/limamaxipix.sip)
    ELSE()
    	CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sip/limamodules.sip.in sip/limamaxipix.sip)
    ENDIF()

    SET(SIP_CONCAT_PARTS 1)
    SET(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip"
        "${CMAKE_BINARY_DIR}/sip/core"
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/tasks/sip"
        "${CMAKE_SOURCE_DIR}/common/sip"
        "${CMAKE_SOURCE_DIR}/hardware/sip"
        "${CMAKE_SOURCE_DIR}/control/sip"
        "${CMAKE_SOURCE_DIR}/control/software_operation/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/sip"	
	"${CMAKE_BINARY_DIR}/camera/common/espia/sip")
    set(SIP_DISABLE_FEATURES WITH_CONFIG)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/sip/lima${NAME}.sip PROPERTIES OBJECT_DEPENDS ${sipfiles})
    set(SIP_CHECK_EXC ON)
    ADD_SIP_PYTHON_MODULE(limamaxipix ${CMAKE_CURRENT_BINARY_DIR}/sip/limamaxipix.sip)
    TARGET_INCLUDE_DIRECTORIES(python_module_limamaxipix PRIVATE
                               ${PYTHON_INCLUDE_DIRS}
                               "${CMAKE_SOURCE_DIR}/sip"
                               "${CMAKE_SOURCE_DIR}/sip/core"
			       "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip")
    TARGET_LINK_LIBRARIES(python_module_limamaxipix limamaxipix)

    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Maxipix")
ENDIF()
