/*
 * Timer.cpp
 *
 *  Created on: 13 Dec 2010
 *      Author: tcn
 */

#include "Timer.h"

Timer::Timer() {

	// Initialise start and stop fields to zero
//#if defined(OS_DARWIN)
//	mStart = 0;
//	mStop  = 0;
//#elif defined(OS_WIN)
//	mStart = 0;
//	mStop  = 0;
//#else
//	mStart = {0};
//	mStop =  {0};
//#endif

}

Timer::~Timer() {

}

void Timer::start(void) {

#if defined(OS_DARWIN)
	mStart = mach_absolute_time();
#elif defined(OS_WIN)
	mStart = (unsigned int)GetTickCount();
#else
	clock_gettime(CLOCK_REALTIME, &mStart);	
#endif
}

void Timer::stop(void) {

#if defined(OS_DARWIN)
	mStop = mach_absolute_time();
#elif defined(OS_WIN)
	mStop = (unsigned int)GetTickCount();
#else
	clock_gettime(CLOCK_REALTIME,&mStop);
#endif
}

double Timer::elapsed(void) {

	double elapsed = 0.0;

#if defined(OS_DARWIN)
	uint64_t difference = mStop - mStart;

	static mach_timebase_info_data_t info = {0,0};
	if (info.denom == 0) {
		mach_timebase_info(&info);
	}

	uint64_t elapsednano = difference * (info.numer / info.denom);
	elapsed = elapsednano * 1e-9;
#elif defined(OS_WIN)
	unsigned long differenceMs = mStop - mStart;
	elapsed = (double)differenceMs / 1000.0;
#else
	unsigned long long startNs = (unsigned long long)mStart.tv_sec * 1000000000 + mStart.tv_nsec;
	unsigned long long stopNs  = (unsigned long long)mStop.tv_sec  * 1000000000 + mStop.tv_nsec;
	unsigned long long elapsedNs = stopNs - startNs;
	elapsed = (double)elapsedNs / 1.0E9;
#endif
	return elapsed;

}
