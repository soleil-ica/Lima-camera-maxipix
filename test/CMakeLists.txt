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

SET(maxipix_test1_srcs test_maxipix_config_reader.cpp)
SET(maxipix_test2_srcs test_maxipix_acq.cpp)

ADD_EXECUTABLE(test_maxipix_config_reader ${maxipix_test1_srcs})
ADD_EXECUTABLE(test_maxipix_acq ${maxipix_test2_srcs})

TARGET_LINK_LIBRARIES(test_maxipix_config_reader limacore limamaxipix)
TARGET_LINK_LIBRARIES(test_maxipix_acq limacore limamaxipix)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config/mydet_chip_1.edf config/mydet_chip_1.edf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config/tpxatl25_chip_1.bpc
config/tpxatl25_chip_1.bpc COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config/tpxatl25_chip_2.bpc
config/tpxatl25_chip_2.bpc COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config/tpxatl25_chip_3.bpc
config/tpxatl25_chip_3.bpc COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config/tpxatl25_chip_4.bpc
config/tpxatl25_chip_4.bpc COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config/tpxatl25.cfg config/tpxatl25.cfg COPYONLY)
