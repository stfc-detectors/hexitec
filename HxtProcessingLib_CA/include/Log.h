#ifndef LOG_H_
#define LOG_H_

using namespace std;

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>
#if defined(OS_WIN)
#include <time.h>
#include <Windows.h>
#else
//#include <sys/time.h> // Used to be
#include <time.h>
#endif
#include "LogConfig.h"

// Suggested log level mappings

/* META			MIDAS					SYSLOG			FILE/STREAM
 * ----------------------------------------------------------------
 * DEBUG		MDEBUG / MT_DEBUG		LOG_DEBUG		(as meta)
 * INFO			MLOG / MT_LOG			LOG_INFO			"
 * NOTICE		MINFO / MT_INFO			LOG_NOTICE			"
 * WARNING		MINFO / MT_INFO			LOG_WARNING			"
 * ERROR		MERROR / MT_ERROR		LOG_ERR				"
 */

// Define our enum of log levels here
enum LogLevel { logERROR, logWARNING, logNOTICE, logINFO,
                logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4,
				logDEBUG5, logDEBUG6, logDEBUG7, logDEBUG8 };

string Timestamp(void);

class Log
{

	public:
	    Log(LogConfig&);
	    Log(LogConfig&, char* aFileName, char* aFunction, int aLine);
	    virtual ~Log();
	    virtual ostringstream& Get(LogLevel level = logINFO);
	    ostringstream& GetStr(void);
        LogLevel GetLevel(void);
	    void     SetLevel(LogLevel level);
	    static string ToString(LogLevel level);
	protected:
	    ostringstream os;
		char* mFileName;
		char *mFunction;
		int   mLine;
	private:
	    Log(const Log&);				// No copying please!
	    Log& operator =(const Log&);	//		"
	    LogConfig* mlc;
	    string theTime;
	    LogLevel mLogLevel;
};

// shortcut!
#define LOG(config,level) \
if (!((config)->doDebugLog(level))) ; \
else Log(*config, (char*)__FILE__, (char*)__FUNCTION__, __LINE__).Get((LogLevel)level)

#endif /*LOG_H_*/
