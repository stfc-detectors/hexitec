/*
 * HxtFrameCorrector.h
 *
 *  Created on: 13 Dec 2010
 *      Author: tcn
 */

#ifndef HXTFRAMECORRECTOR_H_
#define HXTFRAMECORRECTOR_H_

#include <string>
#include "HxtLogger.h"
#include "HxtFrame.h"
#include "HxtDecodedFrame.h"

using namespace std;

namespace hexitech {

class HxtFrameCorrector {
public:
	HxtFrameCorrector(string aName) : mDebug(false), mEventsCorrected(0),  mName(aName), mCaseA(0), mCaseB(0), mCaseC(0), mCaseD(0)
									{ } ;
	virtual ~HxtFrameCorrector() { } ;

    void setDebug(bool aDebug) { mDebug = aDebug; };

	virtual bool apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame, 
						HxtFrame* apSubPixelFrame = NULL) = 0;

	unsigned int getNumEventsCorrected() const
    {
        return mEventsCorrected;
    }

	string getName(void) const
	{
		return mName;
	}

	// Used by child class HxtFrameChargeSharingSubPixelCorrector
	virtual unsigned long long getCaseA() { return mCaseA; }
	virtual unsigned long long getCaseB() {	return mCaseB; }
	virtual unsigned long long getCaseC() {	return mCaseC; }
	virtual unsigned long long getCaseD() {	return mCaseD; }

protected:
	bool mDebug;
	unsigned int mEventsCorrected;

	// Used by child class HxtFrameChargeSharingSubPixelCorrector
	// to track how many of the four different cases found in data
	unsigned long long mCaseA;
	unsigned long long mCaseB;
	unsigned long long mCaseC;
	unsigned long long mCaseD;

private:
	string mName;
};

}

#endif /* HXTFRAMECORRECTOR_H_ */
