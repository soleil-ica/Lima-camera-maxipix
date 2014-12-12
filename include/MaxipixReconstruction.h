//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2014
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#ifndef MAXIPIXRECONSTRUCTION_H
#define MAXIPIXRECONSTRUCTION_H

#include "LinkTask.h"
#include <list>
#include "Constants.h"
#include <SizeUtils.h>

namespace lima
{

  namespace Maxipix
  {

    class MaxipixReconstruction : public LinkTask
    {
      DEB_CLASS_NAMESPC(DebModCamera, "MaxipixReconstruction", "Maxipix");
    public:

      struct Position
      {
	Point origin;
	RotationMode rotation;
      };

      typedef std::list<Position> PositionList;

      enum Type {RAW, ZERO, DISPATCH, MEAN};
      enum Layout {L_NONE, L_2x2, L_5x1, L_FREE, L_GENERAL};
      explicit MaxipixReconstruction(Layout = L_NONE, Type = RAW);
      MaxipixReconstruction(const MaxipixReconstruction&);
      ~MaxipixReconstruction();
      
      void setType(Type);
      void setXnYGapSpace(int xSpace,int ySpace);
      void setChipsRotation(const RotationModeList&);
      void setChipsPosition(const PositionList&);
      Size getImageSize() const;
      
      virtual Data process(Data &aData);
    private:
      Size _getImageSize(int, int, int, int) const;

      Type	m_type;
      Layout	m_layout;
      int	m_xgap;
      int	m_ygap;
      PositionList	m_chips_position;
    };

    inline std::ostream& operator <<(std::ostream& os, const MaxipixReconstruction::Position& p)
      {
	os << "<"
	   << p.origin << ","
	   << p.rotation
	   << ">";
	return os;
      }
  } // namespace Maxipix
} // namespace lima
#endif	// MAXIPIXRECONSTRUCTION_H
