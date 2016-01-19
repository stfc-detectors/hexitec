using namespace std;

#include "LogConfig.h"
#include "Log.h"

#if defined(OS_WIN)

#else
//! Defines the logging facility to use for syslog
const int LogConfig::mLogFacility = LOG_USER;

//! Defines the syslog operating mode(s)
const int LogConfig::mLogOptions = LOG_PID;
#endif

/** Constructor
 */
LogConfig::LogConfig(string processName="Unknown")
{
	// By default, disable all logging
	mLogSyslog = false;
	mLogStdout = false;
	mLogFile = false;
	mInitSyslog = false;
	mInitFile = false;

	// Set default debug level
	mDebugLevel = 0;

	// Set the process name
	mProcessName = processName;
}

/** Destructor
 */
LogConfig::~LogConfig()
{
}

/** Returns log file stream
 * @return filestream
 */
ofstream& LogConfig::getFileStream()
{
	return mFileStream;
}

void LogConfig::setLogSyslog(bool aLog, string aSyslogIdent = "")
{
#if defined(OS_WIN)
#else
	mLogSyslog = aLog;
	mSyslogIdent = aSyslogIdent;
#endif
}

void LogConfig::setLogStdout(bool aLog)
{
	mLogStdout = aLog;
}

void LogConfig::setLogFile(bool aLog, string aLogFilePath = "log.txt")
{
	mLogFilePath = aLogFilePath;

	if (!mInitFile) {
		initFile();
	}
	mLogFile = aLog;
}

void LogConfig::setDebugLevel(int aLevel)
{
	mDebugLevel = aLevel;
}

int LogConfig::getDebugLevel(void)
{
	return mDebugLevel;
}

bool LogConfig::doDebugLog(int aLogLevel)
{
	return (bool) (((aLogLevel - logDEBUG1)+1) <= mDebugLevel);
}

string LogConfig::getProcessName(void)
{
	return mProcessName;
}

bool LogConfig::logToSyslog()
{
	return mLogSyslog;
}

bool LogConfig::logToFile()
{
	return mLogFile;
}

bool LogConfig::logToStdout()
{
	return mLogStdout;
}

void LogConfig::initSyslog()
{
#if defined(OS_WIN)
#else
	if (mSyslogIdent != "") {
		openlog(mSyslogIdent.c_str(), mLogOptions, mLogFacility);
		mInitSyslog = true;
	} else {
		// TODO: Empty ident, throw exception
	}
#endif
}

void LogConfig::initFile()
{
	if (mLogFilePath != "") {
		mFileStream.open(mLogFilePath.c_str(), ios::out | ios::trunc);
		// TODO: Check fstream not bad!
		if (mFileStream) {
			mInitFile = true;
		} else {
			// TODO: Failed, throw exception
		}
	} else {
		// TODO: Empty file path, throw exception
	}
}

void LogConfig::close()
{

	// Cleanly close Syslog, if initialised
	if (mInitSyslog) {
#if defined(OS_WIN)
#else
		closelog();
		mInitSyslog = false;
#endif
	}

	// Close output file, if initialised
	if (mInitFile) {
		mFileStream.close();
		mInitFile = false;
	}
}


