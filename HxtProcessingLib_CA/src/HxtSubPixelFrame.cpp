/*
 * HxtSubPixelFrame.cpp - subclass inheriting from HxtFrame
 *
 *  Created on: 10 February 2012
 *      Author: ckd
 */

#include <iostream>
#include <iomanip>

#include "HxtSubPixelFrame.h"

using namespace std;

namespace hexitech {

HxtSubPixelFrame::HxtSubPixelFrame(unsigned int aRows, unsigned int aCols) :
								HxtFrame(aRows, aCols),
								mInternalRows(aRows),
								mInternalCols(aCols)
{
	// Nothing to do
}

HxtSubPixelFrame::~HxtSubPixelFrame() 
{
	// Nothing to do
}

}
