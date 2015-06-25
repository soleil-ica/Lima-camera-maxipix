############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2013
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
include ../../../config.inc
include ../../common/espia/include/espia.inc

SRCS = test.cpp  test2.cpp

CXXFLAGS = -I../include \
	   -I../tools/cpp \
	   -Wall -pthread -g

INCLUDES = -I../include \
           -I../tools/src \
           -I../../../common/include \
           -I../../../control/include \
           -I../../../hardware/include \
           -I../../common/espia/include \
           -I$(ESPIA_DRV_INCL) \
           -I../../../third-party/Processlib/core/include \
           -I../../../third-party/libconfig/lib

LDFLAGS = -pthread -L../../../build  -L../../../third-party/Processlib/build -L$(ESPIA_DRV_LIB)
LDLIBS = -llimacore  -lprocesslib -llimaespia -lespia 

test-progs = test test2

all: 	$(test-progs)

test:		test.o ../tools/src/libINIReader.o
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

test2:		test2.o ../src/Maxipix.o
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

clean:
	rm -f *.o *.P test test2

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) $(INCLUDES) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
