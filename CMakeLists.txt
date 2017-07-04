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
set(NAME "maxipix")

set(${NAME}_srcs src/PriamSerial.cpp  src/PriamAcq.cpp
	 src/PixelArray.cpp src/MaxipixReconstruction.cpp
	 src/MaxipixCamera.cpp src/MaxipixInterface.cpp
	 src/MpxDetConfig.cpp src/MpxCommon.cpp src/MpxChipConfig.cpp src/MpxDacs
	 tools/src/INIReader.cpp tools/ini.c)

add_library(lima${NAME} SHARED ${${NAME}_srcs})

target_include_directories(lima${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(lima${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tools/src")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/cmake ${CMAKE_MODULE_PATH})
include(FindEspiaDrv)

#set paths for espia driver and limaespialib and include files
target_include_directories(lima${NAME} PUBLIC "${ESPIA_DRV_INCLUDE_DIR}")
target_include_directories(lima${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/include")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../common/espia")

target_link_libraries(lima${NAME} limacore)
target_link_libraries(lima${NAME} ${ESPIA_DRV_LIB})
target_link_libraries(lima${NAME} limaespia)

limatools_set_library_soversion(lima${NAME} "VERSION")
install(TARGETS lima${NAME} LIBRARY DESTINATION lib)

if(LIMA_ENABLE_PYTHON)
    set(NAME "maxipix")
    set(IMPORTS
"%Import limaespia.sip")
    set(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/sip"
	"${CMAKE_BINARY_DIR}/camera/common/espia/sip")
    limatools_run_sip_for_camera(${NAME}) 
    install(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Maxipix")
endif()

if(LIMA_ENABLE_TESTS)
	add_subdirectory(test)
endif()
