/*
 * appMain.cpp - main application code for Hexitech raw data processing
 *
 *  Created on: 10 Dec 2010
 *      Author: Tim Nicholls
 */

#include <iostream>

#include "HxtLogger.h"
#include "HxtRawDataProcessor.h"
#include "HxtFrameIncompleteDataCorrector.h"
#include "HxtFrameChargeSharingDiscCorrector.h"
#include "HxtFrameChargeSharingSubPixelCorrector.h"		  
#include "HxtFrameInducedNoiseCorrector.h"
//#include "HxtFrameInterpolateCorrector.h"
#include "HxtFrameCalibrationCorrector.h"
#include "HxtPixelThreshold.h"
#include "DateStamp.h"
// Development purposes only:
#include "HxtFrameDoublePixelsCorrector.h"


const unsigned int kHxtSensorCols = 80;
const unsigned int kHxtSensorRows = 80;

using namespace std;
using namespace hexitech;

vector<string> gRawFileNames;
unsigned int gDebugLevel = 0;
unsigned int gHistoStartVal = 0;
unsigned int gHistoEndVal   = 10000;
unsigned int gHistoBins     = 1000;
unsigned int gInterpolationThreshold = 0;
double gInducedNoiseThreshold = 0.0;
double gGlobalThreshold     = -1.0;			// old default was no global threshold ie: -1.0;
string gThresholdFileName;
string gOutputFileNameDecodedFrame = "pixelHisto.hx3";
//string gOutputFileNameSubPixelFrame = "pixelSubResolutionHisto.hxt";
string gGradientsFile;
string gInterceptsFile;
bool gEnableInCorrector = false;
bool gEnableCabCorrector = false;
bool gEnableCsaspCorrector = false;
bool gEnableCsdCorrector = true;
bool gEnableIdCorrector = true;
bool gEnableIpCorrector = false;
// Development purposes only:
bool gEnableDbPxlsCorrector = false;
// Normal variables continued

bool gWriteCsvFiles = false;


// Testing using vectors rather than looping every single pixel in frame
bool gEnableVector = false;

// Declaration of global log configuration used throughout - must be initialised
LogConfig* gLogConfig;

// Forward declarations
bool parseArgs(int argc, char** argv);
void printUsage(string aAppName);
char* baseName(char* aPath);

/// Main entry point for application - parses arguments, creates raw data processor and corrector objects,
/// invokes processor on specified files and writes output data files
int main(int argc, char** argv) {

	// Get application name from argument list
	string appName = string(baseName(argv[0]));

	// Parse command line arguments
	if (!parseArgs(argc, argv)) {
		printUsage(appName);
		return -1;
	}

	// Create new log file name
	ostringstream logFileStream;
	DateStamp* now = new DateStamp();
	logFileStream << "hexitech_log_";
	logFileStream << now->GetDateStamp();
	logFileStream << ".txt";
	delete(now);

	// Create new log configuration
	gLogConfig = new LogConfig(appName);
	gLogConfig->setLogStdout(true);
	gLogConfig->setLogFile(true, logFileStream.str());
	gLogConfig->setDebugLevel(gDebugLevel);

	LOG(gLogConfig, logNOTICE) << "Starting up";
	
	// Display user settings
	LOG(gLogConfig, logINFO) << "Histogram start: " << gHistoStartVal << " End: " << gHistoEndVal << " Bins: " << gHistoBins;
	if (gGlobalThreshold != -1.0)
		LOG(gLogConfig, logINFO) << "Global threshold set as: " << gGlobalThreshold;
	else
		LOG(gLogConfig, logINFO) << "Global threshold not set";

	if (!gThresholdFileName.empty())
		LOG(gLogConfig, logINFO) << "Global threshold file selected: " << gThresholdFileName;

	if (gEnableCabCorrector)
		LOG(gLogConfig, logINFO) << "Calibration selected, Gradients file: " << gGradientsFile << " Intercepts file: " << gInterceptsFile;
	else
		LOG(gLogConfig, logINFO) << "Calibration not selected";

	if (gWriteCsvFiles)
		LOG(gLogConfig, logINFO) << "Selected to write csv files";

	LOG(gLogConfig, logINFO) << "Selected correctors:" << 
		((gEnableInCorrector) ? (" InducedNoise") : "") << 
		((gEnableCabCorrector) ? (" Calibration") : "") << 
		((gEnableCsaspCorrector) ? (" ChargeSharingAddition") : "") <<
		((gEnableCsdCorrector) ? (" ChargeSharingDiscrimination") : "") << 
		((gEnableIdCorrector) ? (" IncompleteData") : "") << 
		((gEnableIpCorrector) ? (" Interpolate") : "");
		

	LOG(gLogConfig, logINFO) << "Writing to log file " << logFileStream.str();

	// Load a threshold file
	HxtPixelThreshold* pixelThreshold = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
	if (gGlobalThreshold != -1.0) {
		pixelThreshold->setGlobalThreshold(gGlobalThreshold);
	}
	if (!gThresholdFileName.empty()) {
		pixelThreshold->loadThresholds(gThresholdFileName);
	}


	// Create new raw data processor instance
	HxtRawDataProcessor* dataProcessor = new HxtRawDataProcessor(kHxtSensorRows, kHxtSensorCols, gHistoStartVal, gHistoEndVal, gHistoBins);
	
	// Test setting up vector implementation
	dataProcessor->setVector(gEnableVector);

	// Set debug flags if debug level non-zero
	if (gDebugLevel) dataProcessor->setDebug(true);

	// Apply thresholds to raw data processor
	dataProcessor->applyPixelThresholds(pixelThreshold);

	// Create and register frame correctors with raw data processor if enabled

	// 1. Induced Noise Corrector
	HxtFrameInducedNoiseCorrector* inCorrector = new HxtFrameInducedNoiseCorrector(gInducedNoiseThreshold);
	// Register it, enable debug if needed
	if (gDebugLevel) inCorrector->setDebug(true);
	if (gEnableInCorrector) {
		dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(inCorrector));
		LOG(gLogConfig, logINFO) << "Applying " << inCorrector->getName() << " corrector to data";
	}
	
	// 2. Calibration
	
	// Create pointer to Gradients file
	HxtPixelThreshold* gradientsContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
	// Create pointer to Intercepts file
	HxtPixelThreshold* interceptsContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);	
	HxtFrameCalibrationCorrector* cabCorrector = 0;

	// Load Gradients file and Intercepts file contents to memory	
	if (gEnableCabCorrector) {		
		// Load files' contents into memory
		gradientsContents->loadThresholds(gGradientsFile);
		interceptsContents->loadThresholds(gInterceptsFile);

		// Create Calibration Corrector
		cabCorrector = new HxtFrameCalibrationCorrector(gradientsContents, interceptsContents, kHxtSensorRows, kHxtSensorCols);
		// Register it, enabled debug if needed
		if (gDebugLevel) cabCorrector->setDebug(true);

		dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(cabCorrector));
		LOG(gLogConfig, logINFO) << "Applying " << cabCorrector->getName() << " corrector to data";
	}


	// 3. CS Addition / O R / CS Discriminator
	// 3.1 subpixel
	// Get pointer to subpixel frame
//	HxtFrame* subPixelFrame = dataProcessor->getSubPixelFrame();
	
	// Set Charge Sharing Sub Pixel flag if enabled
	dataProcessor->setCsaCorrector(gEnableCsaspCorrector);

	// Create subpixel corrector
    HxtFrameChargeSharingSubPixelCorrector* subCorrector = new HxtFrameChargeSharingSubPixelCorrector(/*subPixelFrame*/);
	// Register it, enable debug if needed
	if (gDebugLevel) subCorrector->setDebug(true);
	if (gEnableCsaspCorrector) {
		dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(subCorrector));
		LOG(gLogConfig, logINFO) << "Applying " << subCorrector->getName() << " corrector to data";
	}

	// 3.2 CS Addition
	HxtFrameChargeSharingDiscCorrector* csdCorrector = new HxtFrameChargeSharingDiscCorrector();
	if (gDebugLevel) csdCorrector->setDebug(true);
	if (gEnableCsdCorrector) {
		dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(csdCorrector));
		LOG(gLogConfig, logINFO) << "Applying " << csdCorrector->getName() << " corrector to data";
	}

	// 4. Incomplete Data
	HxtFrameIncompleteDataCorrector* idCorrector = new HxtFrameIncompleteDataCorrector();
	if (gDebugLevel) idCorrector->setDebug(true);
	if (gEnableIdCorrector) {
		dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(idCorrector));
		LOG(gLogConfig, logINFO) << "Applying " << idCorrector->getName() << " corrector to data";
	}

	// x. Development purposes only, check for pixels read out more than once
	HxtFrameDoublePixelsCorrector* dbpxlCorrector = new HxtFrameDoublePixelsCorrector();
	if (gEnableDbPxlsCorrector)  {
		if (gDebugLevel) dbpxlCorrector->setDebug(true);
		dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(dbpxlCorrector));
		LOG(gLogConfig, logINFO) << "Applying " << dbpxlCorrector->getName() << " corrector to data";
	}

	// Parse the specified raw files
	dataProcessor->parseFile(gRawFileNames);

	// Flush last frames through processor to output
	dataProcessor->flushFrames();

	// Interpolate pixel histograms if enabled
	if (gEnableIpCorrector) dataProcessor->InterpolateDeadPixels(gInterpolationThreshold);	

	// Write output files
	dataProcessor->writePixelOutput(gOutputFileNameDecodedFrame);
	
        // Write CSV diagnostic histograms if selected
	if (gWriteCsvFiles)	dataProcessor->writeCsvFiles();
	
	//// Delete objects
	delete dataProcessor;
	delete idCorrector;
	delete csdCorrector;
	delete subCorrector;
	delete inCorrector;
	//delete ipCorrector;
	delete pixelThreshold;
	delete gradientsContents;
	delete interceptsContents;
	delete dbpxlCorrector;
	if (cabCorrector != 0) delete cabCorrector;

	LOG(gLogConfig, logNOTICE) << "Finished";

	// Close log configuration and any associated files
	gLogConfig->close();
	delete gLogConfig;

	return 0;
}

/// parseArgs - parse command line arguments (Unix format) and set appropriate flags, values in global variables
/// @arg argc standard number of arguments in list
/// @arg argv list of char arrays of arguments
/// @return boolean flag indicating if parsing OK
bool parseArgs(int argc, char** argv) {

	bool argsOK = true;

	if (argc < 2) {
		cout << "Not enough arguments given" << endl;
		argsOK = false;
	}

	int iarg = 1;
	while (iarg < argc) {
		string arg(argv[iarg++]);

		// Help flag
		if (( arg == "--help") || (arg == "-h")) {
			return false;
		}

		// Debug flag
		if (( arg == "--debug") || (arg == "-d")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			//TODO: sanity check that arg is actually number
			string debugStr = argv[iarg++];
			gDebugLevel = atoi(debugStr.c_str());
			continue;
		}

		// Histogram start flag
		if ((arg == "--start") || (arg == "-s")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			//TODO: sanity check that arg is actually number
			string startStr = argv[iarg++];
			gHistoStartVal = atoi(startStr.c_str());
			continue;
		}

		// Histogram end flag
		if ((arg == "--end") || (arg == "-e")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			//TODO: sanity check that arg is actually number
			string endStr = argv[iarg++];
			gHistoEndVal = atoi(endStr.c_str());
			continue;
		}

		// Histogram number of bins flag
		if ((arg == "--nbins") || (arg == "-n")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			//TODO: sanity check that arg is actually number
			string binsStr = argv[iarg++];
			gHistoBins = atoi(binsStr.c_str());
			continue;
		}

		// Global pixel threshold flag
		if ((arg == "--gthreshold") || (arg == "-gt")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			if (!gThresholdFileName.empty()) {
				cerr << "Cannot specify both a global threshold and a threshold file" << endl;
				argsOK = false;
				continue;
			}
			string globalThreshold = argv[iarg++];
			gGlobalThreshold = strtod(globalThreshold.c_str(), 0);
			continue;
		}

		// Pixel threshold file flag
		if ((arg == "--thresholdfile") || (arg == "-tf")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			if (gGlobalThreshold != -1.0) {
				cerr << "Cannot specify both a global threshold and a threshold file" << endl;
				argsOK = false;
				continue;
			}
			gThresholdFileName = string(argv[iarg++]);
			continue;
		}

		// Disable ID corrector flag (Incomplete Data)
		if ((arg == "--noidcor") || (arg == "-ni")) {
			gEnableIdCorrector = false;
			continue;
		}

		// Disable CSD corrector flag (Charge Sharing Discriminator)
		if ((arg == "--nocsdcor") || (arg == "-nc")) {
			gEnableCsdCorrector = false;
			continue;
		}

		// Enable CA corrector flag (Charged Sharing Addition + Sub Pixel)
		if ((arg == "--cacor") || (arg == "-ca")) {
			gEnableCsaspCorrector = true;
			continue;
		}

		// Disable IN corrector flag (Induced Noise)
		if ((arg == "--incor") || (arg == "-in")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			gEnableInCorrector = true;
			//TODO: sanity check that arg is actually a number
			string inducedNoiseStr = argv[iarg++];
			gInducedNoiseThreshold = atof(inducedNoiseStr.c_str());
			continue;
		}

		// Enable Interpolate corrector flag
		if ((arg == "--ipcor") || (arg == "-ip")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			gEnableIpCorrector = true;
			//TODO: sanity check that arg is actually a number
			string interpolationStr = argv[iarg++];
			gInterpolationThreshold = atoi(interpolationStr.c_str());
			continue;
		}

		// Gradients file flag
		if (( arg == "--gradfile") || (arg == "-gf")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			//TODO: sanity check that arg is actually number
			gGradientsFile = string(argv[iarg++]);
			continue;
		}

		// Intercepts file flag
		if (( arg == "--interfile") || (arg == "-if")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			//TODO: sanity check that arg is actually number
			gInterceptsFile = string(argv[iarg++]);
			continue;
		}


		// Output pixel histogram file name
		if ((arg == "--outputpixelfile") || (arg == "-opf")) {
			if (iarg == argc) {
				cerr << "Missing value for option: " << arg << endl;
				argsOK = false;
				continue;
			}
			gOutputFileNameDecodedFrame = string(argv[iarg++]);
			continue;
		}
		
		// Enable diagnostic CSV file output
		if (( arg == "--csvfiles") || (arg == "-c")) {
			gWriteCsvFiles = true;
			continue;
		}
		
		// Enable vector of hit pixels, rather than looping all 6400 pixels per frame
		// This boost performance of ID, CSD and IN correctors
		if (( arg == "--vector") || (arg == "-v")) {
			gEnableVector = true;
			continue;
		}

		// For developer purposes only, check if any pixel read out twice within same frame
		if (arg == "--DoublePixels") {
			gEnableDbPxlsCorrector = true;
			continue;
		}

		// Cannot run Charge Sharing Addition and Charge Sharing Discriminator together
		if ( gEnableCsaspCorrector && gEnableCsdCorrector )
		{
			cerr << "Cannot combine Charge Sharing Addition and Charge Sharing Discriminator" << endl;
			argsOK = false;
			continue;
		}

		// Are both calibration file specified?
		if ( (!gGradientsFile.empty()) && (!gInterceptsFile.empty()) )
		{
			// Both calibration files specified, enable calibration
			gEnableCabCorrector = true;
		}
		// Is one calibration file specified, but not the other?
		else if ( (gGradientsFile.empty() && !gInterceptsFile.empty()) 
			|| (!gGradientsFile.empty() && gInterceptsFile.empty()) ) {
			cerr << "Must specify both Gradients and Intercepts files for calibration" << endl;
			argsOK = false;
			continue;
		}

		// Filenames is/are last argument(s)
		gRawFileNames.push_back(arg);
	}
	return argsOK;
}



/// printUsage - print usage string
/// @arg aAppName string of application name
void printUsage(string aAppName) {

	cout << endl
		 << aAppName
	     << " is a standalone application to parse Hexitech raw data files, apply" << endl
		 << "corrections to decoded frames, fill and output per-pixel histograms"  << endl
		 << "in a convenient format." << endl
		 << endl
		 << "Command syntax: " << aAppName << " {option option ...} <list of files>" << endl
		 << endl
		 << "where option is one of the following:" << endl
		 << endl
		 << "   --help|-h                        Print this help" << endl
		 << "   --debug|-d <value>               Set debug output level" << endl
		 << "   --start|-s <value>               Start value (lower limit) of output histograms" << endl
		 << "   --end|-e <value>                 End value (upper limit) of output histograms" << endl
	     << "   --nbins|-n <value>               Number of bins in output histograms" << endl
	     << "   --gthreshold|-gt <value>         Global threshold to apply to all pixels" << endl
	     << "   --thresholdfile|-tf <file>       Pixel threshold file to load" << endl
		 << "   --incor|-in <fraction>           Enable and set induced noise corrector threshold (fraction) level" << endl
		 << "   --gradfile|-gf <file>            Gradients file to load for calibration" << endl
		 << "   --interfile|-if <file>           Intercepts file to load for calibration" << endl
		 << "   --cacor|-ca                      Enable charge sharing addition corrector" << endl
		 << "   --nocsdcor|-nc                   Disable charge sharing discrimination corrector" << endl
		 << "   --noidcor|-ni                    Disable incomplete data corrector" << endl
		 << "   --ipcor|-ip <threshold>          Enable and set interpolate corrector threshold level" << endl
	     << "   --outputpixelfile|-opf <name>    Output pixel file name (defaults to " << gOutputFileNameDecodedFrame << ")" << endl
	     << "   --csvfiles|-c                    Enable output of diagnostic histogram CSV files" << endl
		 << "   --vector|-v                      Enable vector indexing (boosts ID, CSD and IN performance)" << endl
	     << endl
	     << "and <list of files> is a space delimited list of files (including paths) to process."
	     << endl;
}

char* baseName(char* aPath) {
#if defined(OS_WIN)
	char delimiter = '\\';
#else
	char delimiter = '/';
#endif
	char *ptr = strrchr(aPath, delimiter);
	return ptr ? ptr + 1 : (char*)aPath;
}
