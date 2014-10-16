//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
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
#include "MaxipixReconstruction.h"

using namespace lima::Maxipix;

static const int MAXIPIX_LINE_SIZE = 256 * 2;
static const int MAXIPIX_NB_LINE = 256;
static const int MAXIPIX_NB_COLUMN = 256;

//----------------------------------------------------------------------------
//			  5x1 copy function
//----------------------------------------------------------------------------
#define COPY_5x1				\
  int maxipix_line_size = MAXIPIX_LINE_SIZE;	\
  int depth = src.depth();			\
  if(depth == 4)				\
    maxipix_line_size <<= 1;				\
  /* set aSrcPt to the last line of the 5th chip */	\
  unsigned char *aSrcPt = ((unsigned char*)src.data()) +		\
			  (maxipix_line_size * 5 * MAXIPIX_NB_LINE) - maxipix_line_size; \
  /*set aDstPt to the last line of the 5th chip including the gap */	\
  unsigned char *aDstPt = ((unsigned char*)dst.data()) +		\
			  ((maxipix_line_size * 5 + xSpace * depth * 4) * MAXIPIX_NB_LINE) - maxipix_line_size;	\
									\
  int aNbLine = MAXIPIX_NB_LINE;					\
  do									\
    {									\
      int MAXIPIX_LINE_SIZE_COPY = maxipix_line_size;			\
									\
      int aNbChipCopyWithGap = 4;					\
      do								\
	{								\
	  memmove(aDstPt,aSrcPt,MAXIPIX_LINE_SIZE_COPY);		\
	  GAP_COPY_FUNCTION						\
	  aDstPt -= maxipix_line_size + xSpace * depth;		        \
	  aSrcPt -= maxipix_line_size;					\
	}								\
      while(--aNbChipCopyWithGap);					\
      memmove(aDstPt,aSrcPt,MAXIPIX_LINE_SIZE_COPY);			\
      aDstPt -= maxipix_line_size;					\
      aSrcPt -= maxipix_line_size;					\
    }									\
while(--aNbLine);

static void _raw_5x1(Data &src,Data &dst,
		     int xSpace)
{
  if(src.depth() == 4)		// Accumulation
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION			\
      int *aPixel = ((int*)aDstPt) - 1;		\
      for(int i = xSpace;i;--i,--aPixel)	\
	*aPixel = 0;
      COPY_5x1;
    }
  else
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION					\
      unsigned short *aPixel = ((unsigned short*)aDstPt) - 1;	\
      for(int i = xSpace;i;--i,--aPixel)			\
	*aPixel = 0;
      COPY_5x1;
    }
}

static inline void _zero_5x1(Data &src,Data &dst,
			     int xSpace)
{
  if(src.depth() == 4)
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION				\
      MAXIPIX_LINE_SIZE_COPY = maxipix_line_size - 4;	\
      int*aPixel = ((int*)aDstPt);			\
      for(int i = xSpace + 2;i;--i,--aPixel)		\
	*aPixel = 0;
      COPY_5x1;
    }
  else
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION					\
      MAXIPIX_LINE_SIZE_COPY = maxipix_line_size - 2;		\
      unsigned short *aPixel = ((unsigned short*)aDstPt);	\
      for(int i = xSpace + 2;i;--i,--aPixel)			\
	*aPixel = 0;
      COPY_5x1;
    }
}
static inline void _dispatch_5x1(Data &src,Data &dst,
				 int xSpace)
{
  int nbPixelDispatch = (xSpace >> 1) + 1; // (xSpace / 2) + 1
  if(src.depth() == 4)
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION						\
      MAXIPIX_LINE_SIZE_COPY = maxipix_line_size - 4;			\
      int *aPixel = ((int*)aDstPt);					\
      int aPixelValue = *aPixel / nbPixelDispatch;			\
      for(int i = nbPixelDispatch;i;--i,--aPixel)			\
	*aPixel = aPixelValue;						\
									\
      int *aSrcPixel = ((int*)aSrcPt) - 1;	\
      aPixelValue = *aSrcPixel / nbPixelDispatch;			\
      for(int i = nbPixelDispatch;i;--i,--aPixel)			\
	*aPixel = aPixelValue; 
      COPY_5x1;
    }
  else
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION						\
      MAXIPIX_LINE_SIZE_COPY = maxipix_line_size - 2;			\
      unsigned short *aPixel = ((unsigned short*)aDstPt);		\
      unsigned short aPixelValue = *aPixel / nbPixelDispatch;		\
      for(int i = nbPixelDispatch;i;--i,--aPixel)			\
	*aPixel = aPixelValue;						\
									\
      unsigned short *aSrcPixel = ((unsigned short*)aSrcPt) - 1;	\
      aPixelValue = *aSrcPixel / nbPixelDispatch;			\
      for(int i = nbPixelDispatch;i;--i,--aPixel)			\
	*aPixel = aPixelValue; 
      COPY_5x1;
    }
}

static inline void _mean_5x1(Data &src,Data &dst,
			     int xSpace)
{
  int nbPixelMean = (xSpace >> 1) + 1; // (xSpace / 2) + 1
  if(src.depth() == 4)
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION						\
      MAXIPIX_LINE_SIZE_COPY = maxipix_line_size - 4;			\
      int*aPixel = ((int*)aDstPt);					\
      int aFirstPixelValue = *aPixel / nbPixelMean;			\
      *aPixel = aFirstPixelValue;--aPixel;				\
									\
      int *aSrcPixel = ((int*)aSrcPt) - 1;				\
      int aSecondPixelValue = *aSrcPixel / nbPixelMean;			\
      float aStepValue = (aSecondPixelValue - aFirstPixelValue) / (xSpace + 1); \
      float aPixelValue = aFirstPixelValue + aStepValue;		\
      for(int i = xSpace + 1;i;--i,aPixelValue += aStepValue,--aPixel)	\
	*aPixel = (int)aPixelValue;
      COPY_5x1;
    }
  else
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION						\
      MAXIPIX_LINE_SIZE_COPY = maxipix_line_size - 2;			\
      unsigned short *aPixel = ((unsigned short*)aDstPt);		\
      unsigned short aFirstPixelValue = *aPixel / nbPixelMean;		\
      *aPixel = aFirstPixelValue;--aPixel;				\
									\
      unsigned short *aSrcPixel = ((unsigned short*)aSrcPt) - 1;	\
      unsigned short aSecondPixelValue = *aSrcPixel / nbPixelMean;	\
      float aStepValue = (aSecondPixelValue - aFirstPixelValue) / (xSpace + 1); \
      float aPixelValue = aFirstPixelValue + aStepValue;		\
      for(int i = xSpace + 1;i;--i,aPixelValue += aStepValue,--aPixel)	\
	*aPixel = (unsigned short)aPixelValue;
      COPY_5x1;
    }
}
//----------------------------------------------------------------------------
//			     2x2 function
//----------------------------------------------------------------------------
template<class type>
static inline void copy_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  int aTotalHeight = (MAXIPIX_NB_LINE * 2) + ySpace;
  type *aSrcPt = (type*)src.data();
  type *aDstPt = ((type*)dst->data) + (aTotalWidth * aTotalHeight) - aTotalWidth;

  int aJump2LeftChip = aTotalWidth - 1;
  
  int aColumnIterNumber = MAXIPIX_NB_COLUMN;
  do
    {
      //copy left chips
      for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt -= aTotalWidth,++aSrcPt)
	*aDstPt = *aSrcPt;
 
      aDstPt -= aTotalWidth * ySpace;
      
      for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt -= aTotalWidth,++aSrcPt)
	*aDstPt = *aSrcPt;
 
      aDstPt += aJump2LeftChip + aTotalWidth;
      --aJump2LeftChip;
      //copy right chips
      for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += aTotalWidth,++aSrcPt)
	*aDstPt = *aSrcPt;

      aDstPt += aTotalWidth * ySpace;

      for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += aTotalWidth,++aSrcPt)
	*aDstPt = *aSrcPt;

      aDstPt -= aJump2LeftChip + aTotalWidth;
      --aJump2LeftChip;
    }
  while(--aColumnIterNumber);

}

template<class type>
static inline void _raw_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  copy_2x2<type>(src,dst,xSpace,ySpace);

  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  type *aDstPt = ((type*)dst->data) + MAXIPIX_NB_COLUMN;
  for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += aTotalWidth)
    for(int k = 0;k < xSpace;++k)
      aDstPt[k] = 0;
  
  aDstPt -= MAXIPIX_NB_COLUMN;
  int aGapSize = aTotalWidth * ySpace;
  memset(aDstPt,0,aGapSize * sizeof(type));
  aDstPt += aGapSize;

  aDstPt += MAXIPIX_NB_COLUMN;
  for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += aTotalWidth)
    for(int k = 0;k < xSpace;++k)
      aDstPt[k] = 0;
}

template<class type>
static inline void _zero_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  copy_2x2<type>(src,dst,xSpace,ySpace);
  
  type *aDstPt = ((type*)dst->data) + MAXIPIX_NB_COLUMN - 1;
  for(int i = MAXIPIX_NB_LINE - 1;i;--i,aDstPt += aTotalWidth)
    for(int k = 0;k < xSpace + 2;++k)
      aDstPt[k] = 0;
  
  aDstPt -= MAXIPIX_NB_COLUMN - 1;
  int aGapSize = aTotalWidth * (ySpace + 2);
  memset(aDstPt,0,aGapSize * sizeof(type));
  aDstPt += aGapSize;

  aDstPt += MAXIPIX_NB_COLUMN - 1;
  for(int i = MAXIPIX_NB_LINE - 1;i;--i,aDstPt += aTotalWidth)
    for(int k = 0;k < xSpace + 2;++k)
      aDstPt[k] = 0;
}

template<class type>
static inline void _dispatch_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  copy_2x2<type>(src,dst,xSpace,ySpace);
  
  type *aDstPt = ((type*)dst->data) + MAXIPIX_NB_COLUMN - 1;
  int aNbPixel2Dispatch = (xSpace >> 1) + 1;

  for(int lineId = MAXIPIX_NB_LINE - 1;lineId;--lineId)
    {
      type aPixelValue = *aDstPt / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;

      aPixelValue = aDstPt[aNbPixel2Dispatch - 1] / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;
  
      aDstPt += aTotalWidth - (xSpace + 2);
    }
  

  aDstPt -= MAXIPIX_NB_COLUMN - 1;
  int aNbPixel2DispatchInY = (ySpace >> 1) + 1;
  
  int aPart = 2;
  /* aPart == 2 => upper chip
     aPart == 1 => bottom chip
  */
  do
    {
      type aPixelValue;
      //Last line or first line of left chip (depend on aPart)
      for(int columnIter = MAXIPIX_NB_COLUMN - 1;columnIter;--columnIter,++aDstPt)
	{
	  aPixelValue = *aDstPt / aNbPixel2DispatchInY;
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aTotalWidth * y] = aPixelValue;
	}
  
      //Bottom or top right corner of left chip
      aPixelValue = *aDstPt / (aNbPixel2DispatchInY * aNbPixel2Dispatch);
      for(int columnIter = aNbPixel2Dispatch;columnIter;--columnIter,++aDstPt)
	{
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aTotalWidth * y] = aPixelValue;
	}
      //Bottom or top left corner of right chip
      aPixelValue = aDstPt[aNbPixel2Dispatch - 1] / (aNbPixel2DispatchInY * aNbPixel2Dispatch);
      for(int columnIter = aNbPixel2Dispatch;columnIter;--columnIter,++aDstPt)
	{
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aTotalWidth * y] = aPixelValue;
	}
      //Last or first line of right chip
      for(int columnIter = MAXIPIX_NB_COLUMN - 1;columnIter;--columnIter,++aDstPt)
	{
	  aPixelValue = *aDstPt / aNbPixel2DispatchInY;
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aTotalWidth * y] = aPixelValue;
	}
      
      //Set Variable for second part iteration (bottom chip)
      aDstPt += aTotalWidth * ySpace;
      aTotalWidth = -aTotalWidth;
    }
  while(--aPart);

  aDstPt = ((type*)dst->data) + aTotalWidth * (MAXIPIX_NB_LINE + ySpace + 1) +
    MAXIPIX_NB_COLUMN - 1;
  
  for(int lineId = MAXIPIX_NB_LINE - 1;lineId;--lineId)
    {
      type aPixelValue = *aDstPt / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;

      aPixelValue = aDstPt[aNbPixel2Dispatch - 1] / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;
  
      aDstPt += aTotalWidth - (xSpace + 2);
    }
}

template<class type>
static inline void _mean_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  copy_2x2<type>(src,dst,xSpace,ySpace);
  type *aDstPt = ((type*)dst->data) + MAXIPIX_NB_COLUMN - 1;
  int aNbXPixel2Dispatch = (xSpace >> 1) + 1;
  int aNbYPixel2Dispatch = (ySpace >> 1) + 1;

  for(int aLineIter = MAXIPIX_NB_LINE - 1;aLineIter;--aLineIter,aDstPt += aTotalWidth)
    {
      float aFirstValue = *aDstPt / float(aNbXPixel2Dispatch);
      float aLastValue = aDstPt[xSpace + 1] / float(aNbXPixel2Dispatch);
      float anInc = (aLastValue - aFirstValue) / (xSpace + 1);
      *aDstPt = (type)aFirstValue;
      aDstPt[xSpace + 1] = (type)aLastValue;
      aFirstValue += anInc;

      for(int i = 0;i < xSpace;++i,aFirstValue += anInc)
	aDstPt[1 + i] = (type)aFirstValue;
    }

  //corner
  int aNbPixel2Dispatch = aNbXPixel2Dispatch + aNbYPixel2Dispatch;
  float a1ftCornerValue = *aDstPt / float(aNbPixel2Dispatch);
  float a2ndCornerValue = aDstPt[xSpace + 1] / float(aNbPixel2Dispatch);
  float a3thCornerValue = aDstPt[(ySpace + 1) * aTotalWidth]  / float(aNbPixel2Dispatch);
  float a4thCornerValue = aDstPt[(ySpace + 1) * aTotalWidth + xSpace + 1] / float(aNbPixel2Dispatch);
  
  float *aFirstLinePt = new float[xSpace +2];
  aFirstLinePt[0] = a1ftCornerValue;
  aFirstLinePt[xSpace + 1] = a2ndCornerValue;
  float anInc = (a2ndCornerValue - a1ftCornerValue) / (xSpace + 1);
  for(int i = 0;i < xSpace;++i)
    aFirstLinePt[i + 1] = aFirstLinePt[i] + anInc;

  float *aLastLinePt = new float[xSpace + 2];
  aLastLinePt[0] = a3thCornerValue;
  aLastLinePt[xSpace + 1] = a4thCornerValue;
  anInc = (a4thCornerValue - a3thCornerValue) / (xSpace + 1);
  for(int i = 0;i < xSpace;++i)
    aLastLinePt[i + 1] = aLastLinePt[i] + anInc;
  
  float *anIncBuffer = new float[xSpace + 2];
  for(int i = 0;i < xSpace + 2;++i)
    anIncBuffer[i] = (aLastLinePt[i] - aFirstLinePt[i]) / (ySpace + 1);
  
  
  for(int lineIter = ySpace + 1;lineIter;--lineIter,aDstPt += aTotalWidth)
    {
      for(int colId = 0;colId < xSpace + 2;++colId)
	{
	  aDstPt[colId] = (type)aFirstLinePt[colId];
	  aFirstLinePt[colId] += anIncBuffer[colId];
	}
    }
  for(int colId = 0;colId < xSpace + 2;++colId)
    aDstPt[colId] = (type)aLastLinePt[colId];

  delete [] aFirstLinePt;
  delete [] aLastLinePt;
  delete [] anIncBuffer;

  //center lines
  aDstPt -= (aTotalWidth * (ySpace + 1)) + MAXIPIX_NB_COLUMN - 1;
  
  type *aFirstValuePt = aDstPt;
  type *aSecondValuePt = aDstPt + (aTotalWidth * (ySpace + 1));
  
  int aNbChip = 2;
  do
    {
      for(int i = MAXIPIX_NB_COLUMN - 1;i;--i,++aFirstValuePt,++aSecondValuePt)
	{
	  float aFirstValue = *aFirstValuePt / float(aNbYPixel2Dispatch);
	  float aLastValue = *aSecondValuePt / float(aNbYPixel2Dispatch);
	  anInc = (aLastValue - aFirstValue) / (ySpace + 1);
	  *aFirstValuePt = (type)aFirstValue;
	  *aSecondValuePt = (type)aLastValue;

	  aFirstValue += anInc;

	  for(int lineIter = 0;lineIter < ySpace;++lineIter,aFirstValue += anInc)
	    aFirstValuePt[(1 + lineIter) * aTotalWidth] = (type)aFirstValue;
	}
      aFirstValuePt += xSpace + 2;
      aSecondValuePt += xSpace + 2;
    }
  while(--aNbChip);

  aDstPt += MAXIPIX_NB_COLUMN - 1 + aTotalWidth;
  for(int aLineIter = MAXIPIX_NB_LINE - 1;aLineIter;--aLineIter,aDstPt += aTotalWidth)
    {
      float aFirstValue = *aDstPt / float(aNbXPixel2Dispatch);
      float aLastValue = aDstPt[xSpace + 1] / float(aNbXPixel2Dispatch);
      float anInc = (aLastValue - aFirstValue) / (xSpace + 1);
      *aDstPt = (type)aFirstValue;
      aDstPt[xSpace + 1] = (type)aLastValue;
      aFirstValue += anInc;

      for(int i = 0;i < xSpace;++i,aFirstValue += anInc)
	aDstPt[1 + i] = (type)aFirstValue;
    }
  
}

template<class type>
static inline void _copy_chip(int i,int nb_chips,Data &aData,char* aBuffer)
{
  int aTotalWidth = nb_chips * MAXIPIX_NB_COLUMN;
  type *src = (type*)aData.data() + i * MAXIPIX_NB_COLUMN;
  type *dst = (type*)aBuffer;

  for(int lineId = 0;lineId < MAXIPIX_NB_LINE;
      ++lineId,dst += MAXIPIX_NB_COLUMN,src += aTotalWidth)
    memcpy(dst,src,sizeof(type)*MAXIPIX_NB_COLUMN);
}

template<class type>
static inline void _rot_chip_90(int chip_id,int nb_chips,
				char *aBuffer,Data &dst_data)
{
  int aTotalWidth = (nb_chips - 1) * MAXIPIX_NB_COLUMN;
  type *src = (type*)aBuffer;
  type *dst = (type*)dst_data.data();
  dst += chip_id * MAXIPIX_NB_COLUMN;
  for(int lineId = 0;lineId < MAXIPIX_NB_LINE;++lineId,dst += aTotalWidth)
    {
      type *src_column = src + ((MAXIPIX_NB_LINE - 1) * MAXIPIX_NB_COLUMN) + lineId;
      for(int colId = 0;colId < MAXIPIX_NB_COLUMN;++colId,++dst,src_column -= MAXIPIX_NB_COLUMN)
	*dst = *src_column;
    }
}

template<class type>
static inline void _rot_chip_180(int chip_id,int nb_chips,
				 char *aBuffer,Data &dst_data)
{
  int aTotalWidth = (nb_chips - 1) * MAXIPIX_NB_COLUMN;
  type *src = (type*)aBuffer;
  type *dst = (type*)dst_data.data();
  dst += chip_id * MAXIPIX_NB_COLUMN;

  for(int lineId = 0;lineId < MAXIPIX_NB_LINE;++lineId,dst += aTotalWidth)
    {
      type *src_line = src + (MAXIPIX_NB_LINE - lineId) * MAXIPIX_NB_COLUMN - 1;
      for(int colId = 0;colId < MAXIPIX_NB_COLUMN;++colId,++dst,--src_line)
	*dst = *src_line;
    }  
}

template<class type>
static inline void _rot_chip_270(int chip_id,int nb_chips,
				 char *aBuffer,Data &dst_data)
{
  int aTotalWidth = (nb_chips - 1) * MAXIPIX_NB_COLUMN;
  type *src = (type*)aBuffer;
  type *dst = (type*)dst_data.data();
  dst += chip_id * MAXIPIX_NB_COLUMN;
  
  for(int lineId = 0;lineId < MAXIPIX_NB_LINE;++lineId,dst += aTotalWidth)
    {
      type *src_column = src + (MAXIPIX_NB_COLUMN - 1) - lineId;
      for(int colId = 0;colId < MAXIPIX_NB_COLUMN;++colId,++dst,src_column += MAXIPIX_NB_COLUMN)
	*dst = *src_column;
    }
}

MaxipixReconstruction::MaxipixReconstruction(MaxipixReconstruction::Model aModel,
					     MaxipixReconstruction::Type aType) :
  mType(aType),mModel(aModel),mXSpace(4),mYSpace(4)
{
}

MaxipixReconstruction::MaxipixReconstruction(const MaxipixReconstruction &other) :
  mType(other.mType),mModel(other.mModel),
  mXSpace(other.mXSpace),mYSpace(other.mYSpace)
{
}

MaxipixReconstruction::~MaxipixReconstruction()
{
}

void MaxipixReconstruction::setType(MaxipixReconstruction::Type aType)
{
  mType = aType;
}
void MaxipixReconstruction::setModel(MaxipixReconstruction::Model aModel)
{
	mModel = aModel;
}
void MaxipixReconstruction::setXnYGapSpace(int xSpace,int ySpace)
{
  mXSpace = xSpace,mYSpace = ySpace;
}

void MaxipixReconstruction::setChipsRotation(const RotationModeList& rList)
{
  mModel = M_FREE;
  mXSpace = mYSpace = 0;
  mChipsRotation = rList;
}

void MaxipixReconstruction::setChipsPosition(const MaxipixReconstruction::PositionList& pList)
{
  mModel = M_GENERAL;
  mChipsPosition = pList;
}

lima::Size MaxipixReconstruction::getImageSize() const
{  
  DEB_MEMBER_FUNCT();
     
  switch(mModel)
    {
    case M_2x2:
      return _getImageSize(2,2);
    case M_5x1:
      return _getImageSize(5,1);
    case M_FREE:
      return _getImageSize(mChipsRotation.size(),1);
    case M_GENERAL:
      {
      int xMin,xMax;
      int yMin,yMax;
      PositionList::const_iterator chip_pos = mChipsPosition.begin();
      if(chip_pos == mChipsPosition.end())
	THROW_HW_ERROR(Error) << "No chips position set";

      //Init
      xMin = chip_pos->origin.x;
      xMax = xMax + MAXIPIX_NB_COLUMN;

      yMin = chip_pos->origin.y;
      yMax = yMin + MAXIPIX_NB_LINE;
      ++chip_pos;

      for(;chip_pos != mChipsPosition.end();++chip_pos)
	{
	  int localXmin,localXMax,localYmin,localYMax;
	  localXmin = chip_pos->origin.x;
	  localXMax = localXmin + MAXIPIX_NB_COLUMN;

	  localYmin = chip_pos->origin.y;
	  localYMax = localYmin + MAXIPIX_NB_LINE;

	  if(localXmin < xMin) xMin = localXmin;
	  if(localXMax > xMax) xMax = localXMax;

	  if(localYmin < yMin) yMin = localYmin;
	  if(localYMax > yMax) yMax = localYMax;
	}
      return Size(xMax-xMin,yMax-yMin);
      }
    default:
      THROW_HW_ERROR(Error) << "Reconstruction type unknown";
    }
}

lima::Size MaxipixReconstruction::_getImageSize(int x_chip,int y_chip) const
{
  int w= x_chip*256 + (x_chip-1)*mXSpace;
  int h= y_chip*256 + (y_chip-1)*mYSpace;
  return Size(w, h);
}

Data MaxipixReconstruction::process(Data &aData)
{
  Data aReturnData;
  aReturnData = aData;

  if(mModel == M_5x1)
    {
      aReturnData.dimensions[0] = MAXIPIX_NB_COLUMN * 5 + 4 * mXSpace;
      if(!_processingInPlaceFlag)
	{
	  Buffer *aNewBuffer = new Buffer(aReturnData.size());
	  aReturnData.setBuffer(aNewBuffer);
	  aNewBuffer->unref();
	}
      switch(mType)
	{
	case RAW:
	  _raw_5x1(aData,aReturnData,mXSpace);break;
	case ZERO: 
	  _zero_5x1(aData,aReturnData,mXSpace);break;
	case DISPATCH:
	  _dispatch_5x1(aData,aReturnData,mXSpace);break;
	case MEAN:
	  _mean_5x1(aData,aReturnData,mXSpace);break;
	default:		// ERROR
	  break;
	}
    }
  else if(mModel == M_2x2)			// Model 2x2
    {
      aReturnData.dimensions[0] = MAXIPIX_NB_COLUMN * 2 + mXSpace;
      aReturnData.dimensions[1] = MAXIPIX_NB_LINE * 2 + mYSpace;

      Buffer *aNewBuffer = new Buffer(aReturnData.size());
      switch(mType)
	{
	case RAW:
	  if(aReturnData.depth() == 4)
	    _raw_2x2<int>(aData,aNewBuffer,mXSpace,mYSpace);
	  else
	    _raw_2x2<unsigned short>(aData,aNewBuffer,mXSpace,mYSpace);
	  break;
	case ZERO:
	  if(aReturnData.depth() == 4)
	    _zero_2x2<int>(aData,aNewBuffer,mXSpace,mYSpace);
	  else
	    _zero_2x2<unsigned short>(aData,aNewBuffer,mXSpace,mYSpace);
	  break;
	case DISPATCH:
	  if(aReturnData.depth() == 4)
	    _dispatch_2x2<int>(aData,aNewBuffer,mXSpace,mYSpace);
	  else
	    _dispatch_2x2<unsigned short>(aData,aNewBuffer,mXSpace,mYSpace);
	  break;
	case MEAN:
	  if(aReturnData.depth() == 4)
	    _mean_2x2<int>(aData,aNewBuffer,mXSpace,mYSpace);
	  else
	    _mean_2x2<unsigned short>(aData,aNewBuffer,mXSpace,mYSpace);
	  break;
	default:		// ERROR
	  break;
	}

      if(_processingInPlaceFlag)
	{
	  unsigned char *aSrcPt = (unsigned char*)aNewBuffer->data;
	  unsigned char *aDstPt = (unsigned char*)aData.data();
	  memcpy(aDstPt,aSrcPt,aReturnData.size());
	}
      else
	aReturnData.setBuffer(aNewBuffer);
      aNewBuffer->unref();
    }
  else if(mModel == M_FREE)
    {
      if(_processingInPlaceFlag)
	{
	  char aBuffer[MAXIPIX_NB_LINE * MAXIPIX_NB_COLUMN * 4];

	  std::list<RotationMode>::iterator cRot = mChipsRotation.begin();
	  int nb_chips = mChipsRotation.size();
	  for(int i = 0;cRot != mChipsRotation.end();++cRot,++i)
	    {
	      switch(*cRot)
		{
		case Rotation_90:
		  if(aReturnData.depth() == 4)
		    {
		      _copy_chip<int>(i,nb_chips,aData,aBuffer);
		      _rot_chip_90<int>(i,nb_chips,aBuffer,aData);
		    }
		  else
		    {
		      _copy_chip<unsigned short>(i,nb_chips,aData,aBuffer);
		      _rot_chip_90<unsigned short>(i,nb_chips,aBuffer,aData);
		    }
		  break;
		case Rotation_180:
		  if(aReturnData.depth() == 4)
		    {
		      _copy_chip<int>(i,nb_chips,aData,aBuffer);
		      _rot_chip_180<int>(i,nb_chips,aBuffer,aData);
		    }
		  else
		    {
		      _copy_chip<unsigned short>(i,nb_chips,aData,aBuffer);
		      _rot_chip_180<unsigned short>(i,nb_chips,aBuffer,aData);
		    }
		  break;
		case Rotation_270:
		  if(aReturnData.depth() == 4)
		    {
		      _copy_chip<int>(i,nb_chips,aData,aBuffer);
		      _rot_chip_270<int>(i,nb_chips,aBuffer,aData);
		    }
		  else
		    {
		      _copy_chip<unsigned short>(i,nb_chips,aData,aBuffer);
		      _rot_chip_270<unsigned short>(i,nb_chips,aBuffer,aData);
		    }
		  break;
		default:
		  break; // Nothing to do
		}
	    }
	}
      else			// M_GENERAL
	{
	  Buffer *aNewBuffer = new Buffer(aReturnData.size());
	  memset(aNewBuffer,0,aReturnData.size());
	  int chip_id = 0;
	  int nb_chips = mChipsPosition.size();
	  for(PositionList::iterator chip_iter = mChipsPosition.begin();
	      chip_iter != mChipsPosition.end();++chip_iter,++chip_id)
	    {
	      int depth = aReturnData.depth();
	      int src_line_stride = nb_chips * MAXIPIX_NB_COLUMN * depth;
	      int dst_line_stride = aReturnData.dimensions[0] * depth;
	      unsigned char* dst = ((unsigned char*)aNewBuffer->data) + chip_iter->origin.y * dst_line_stride +
		chip_iter->origin.x * depth;

	      switch(chip_iter->rotation)
		{
		case Rotation_90:
		  for(int col_id = 0;col_id < MAXIPIX_NB_COLUMN;++col_id)
		    {
		      unsigned char* src = ((unsigned char*)aData.data()) + (chip_id * MAXIPIX_NB_COLUMN * depth + 
									     (MAXIPIX_NB_COLUMN - 1 - col_id) * depth);
		      unsigned char* local_dst = dst + dst_line_stride * col_id;
		      for(int line_id = 0;line_id < MAXIPIX_NB_LINE;++line_id,src += src_line_stride,dst += depth)
			memcpy(local_dst,src,depth);
		    }
		  break;
		case Rotation_180:
		  for(int line_id = 0;line_id < MAXIPIX_NB_LINE;++line_id)
		    {
		      unsigned char* src = ((unsigned char*)aData.data()) + (chip_id * MAXIPIX_NB_COLUMN * depth + 
									     (MAXIPIX_NB_LINE - 1 - line_id) * src_line_stride +
									     (MAXIPIX_NB_COLUMN - 1 ) * depth);
		      unsigned char* local_dst = dst + dst_line_stride * line_id;
		      for(int col_id = 0;col_id < MAXIPIX_NB_COLUMN;++col_id,local_dst += depth,src -= depth)
			memcpy(local_dst,src,depth);
		    }
		  break;
		case Rotation_270:
		  for(int col_id = 0;col_id < MAXIPIX_NB_COLUMN;++col_id)
		    {
		      unsigned char* src = ((unsigned char*)aData.data()) + (chip_id * MAXIPIX_NB_COLUMN * depth + 
									     (MAXIPIX_NB_LINE * src_line_stride) - 
									     (MAXIPIX_NB_COLUMN + col_id) * depth);
		      unsigned char* local_dst = dst + dst_line_stride * col_id;
		      for(int line_id = 0;line_id < MAXIPIX_NB_LINE;++line_id,src -= src_line_stride,dst += depth)
			memcpy(local_dst,src,depth);
		    }
		  break;
		default:
		  {
		    unsigned char* src = ((unsigned char*)aData.data()) + chip_id * MAXIPIX_NB_COLUMN * depth;
		    for(int ligne_id = 0;ligne_id < MAXIPIX_NB_LINE;++ligne_id,src += src_line_stride,dst += dst_line_stride)
		      memcpy(dst,src,MAXIPIX_NB_COLUMN * depth);
		  }
		  break;
		}
	    }
	  

	  if(_processingInPlaceFlag)
	    {
	      unsigned char *aSrcPt = (unsigned char*)aNewBuffer->data;
	      unsigned char *aDstPt = (unsigned char*)aData.data();
	      memcpy(aDstPt,aSrcPt,aReturnData.size());
	    }
	  else
	    aReturnData.setBuffer(aNewBuffer);
	  aNewBuffer->unref();
	}
    }

  return aReturnData;
}
