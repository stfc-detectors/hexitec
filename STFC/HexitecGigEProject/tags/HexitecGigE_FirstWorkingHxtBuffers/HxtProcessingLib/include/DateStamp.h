/*
 * DateStamp.h
 *
 *  Created on: 5 Jan 2011
 *      Author: tcn
 */

#ifndef DATESTAMP_H_
#define DATESTAMP_H_

#include <time.h>
#include <string>

using namespace std;

class DateStamp {
public:
		DateStamp();
		virtual ~DateStamp();

		string GetDateStamp(void);
private:
		time_t mTimeNow;
		const static unsigned int kMaxDateStamp = 32;

};
#endif /* DATESTAMP_H_ */
