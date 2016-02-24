/*
 * Timer.h
 *
 *  Created on: 13 Dec 2010
 *      Author: tcn
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>
#if defined(OS_DARWIN)
#include <mach/mach_time.h>
#include <stdint.h>
#elif defined(OS_WIN)
#include <Windows.h>
#endif

#if defined(OS_DARWIN)
	typedef uint64_t timerTime;
#elif defined(OS_WIN)
	typedef unsigned long timerTime;
#else
	typedef struct timespec timerTime;
#endif

class Timer {
public:
	Timer();
	virtual ~Timer();

	void start();
	void stop();
	double elapsed();

private:
	timerTime mStart;
	timerTime mStop;

};

#endif /* TIMER_H_ */
