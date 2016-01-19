#include "Log.h"

Log::Log(LogConfig &lc)
{
	mlc = &lc;
	theTime = Timestamp();	// Timestamp objects creation
	mFileName = NULL;
	mFunction = NULL;
	mLine     = 0;

}

Log::Log(LogConfig &lc, char *aFileName, char *aFunction, int aLine) {

	mlc = &lc;
	theTime = Timestamp();
	mFileName = aFileName;
	mFunction = aFunction;
	mLine     = aLine;

}

Log::~Log()
{

	if ( ((mLogLevel - logDEBUG1) + 1) <= mlc->getDebugLevel()) {


		// Stream
		if (mlc->logToStdout()) {
			cout << theTime << " " << os.str() << endl;
		}

		// File
		if (mlc->logToFile()) {
			mlc->getFileStream() << theTime << " " << os.str() << endl;
		}

		// Syslog
		if (mlc->logToSyslog()) {
			// TODO: syslog priority set to 1, is this OK?
#if defined(OS_WIN)
#else
			syslog(1, os.str().c_str());
			// Window syslog alternative? http://support.microsoft.com/default.aspx?scid=kb;EN-US;815661
			// .. Or doesn't need to be that complicated? http://www.cplusplus.com/forum/windows/43888/#msg237631
#endif
		}

	}
}

ostringstream& Log::Get(LogLevel level)
{

	mLogLevel = level;

	os << "[";
	os.width(8);
	os.fill(' ');
	//os << left << ToString(level) <<  "] - ";
	os << left << ToString(level) << "] " << mlc->getProcessName() << ": " ;

    return os;
}

ostringstream& Log::GetStr(void) {

	return os;

}

LogLevel Log::GetLevel(void)
{
	return mLogLevel;
}

void Log::SetLevel(LogLevel level) {
	mLogLevel = level;
}

string Log::ToString(LogLevel level)
{
    static const char* const buffer[] = {"ERROR", "WARNING", "NOTICE", "INFO", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4", "DEBUG5", "DEBUG6", "DEBUG7", "DEBUG8"};
    return buffer[level];
}

string Timestamp()
{
    char result[100] = {0};
	
#if defined(OS_WIN)
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf_s(result, "%2d:%2d:%2d:%03ld", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
	char buffer[11];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
#endif
    return result;
}
