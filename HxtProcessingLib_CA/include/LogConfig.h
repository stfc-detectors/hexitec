#ifndef LOGCONFIG_H_
#define LOGCONFIG_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#if defined(OS_WIN)	// TEST
#else
#include <syslog.h>
#endif
class LogConfig
{

	public:

		LogConfig(string processName);
		~LogConfig();

		ofstream& getFileStream();

		// Setters
		void setLogSyslog(bool, string aSyslogIdent);
		void setLogFile(bool, string aLogFilePath);
		void setLogStdout(bool);

		void setDebugLevel(int aLevel);

		// Getters
		bool logToSyslog();
		bool logToFile();
		bool logToStdout();
		int  getDebugLevel(void);
		bool doDebugLog(int  aLogLevel);

		string      getProcessName();

		void close();

	protected:


	private:

		string mSyslogIdent;
		string mLogFilePath;

		string mProcessName;

		ofstream mFileStream;

		bool mLogSyslog, mLogStdout, mLogFile;			// Represents what endpoints log messages should go to
		bool mInitSyslog, mInitFile;				// Represents initialisation status

		static const int mLogFacility;
		static const int mLogOptions;

		int mDebugLevel;

		void initSyslog();
		void initFile();
};

#endif /*LOGCONFIG_H_*/
