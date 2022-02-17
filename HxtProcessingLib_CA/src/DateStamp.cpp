/*
 * DateStamp.cpp
 *
 *  Created on: 5 Jan 2011
 *      Author: tcn
 */

#include "DateStamp.h"

DateStamp::DateStamp() {

	mTimeNow = time(NULL);

}

DateStamp::~DateStamp() {

}

string DateStamp::GetDateStamp(void)
{
	char theDateStamp[kMaxDateStamp];
	tm theGmTime;
#if defined(OS_WIN)
	gmtime_s(&theGmTime, &mTimeNow);
#else
	gmtime_r(&mTimeNow, &theGmTime);
#endif
	strftime(theDateStamp, kMaxDateStamp, "%Y-%m-%d_%H-%M-%S", &theGmTime);
	return string(theDateStamp);
}
