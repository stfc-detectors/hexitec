#include "processingwindow.h"
#include "ui_processingwindow.h"
#include "parameters.h"
#include "badinifiledialog.h"

#include <QDebug>
#include <QThread>
#include <QFileDialog>
#include <QSettings>
// Used by filename manipulation:
#include <QFileInfo>
#include <QDir>
// Used by fileExists() replacement:
#include <sys/stat.h>

ProcessingWindow *ProcessingWindow::processingWindowInstance = 0;
hexitech::HxtProcessing *ProcessingWindow::HxtProcessor = 0;
unsigned int ProcessingWindow::iDebugLevel;
unsigned int ProcessingWindow::iHistoStartVal;
unsigned int ProcessingWindow::iHistoEndVal;
double ProcessingWindow::dHistoBins;
unsigned int ProcessingWindow::iNumberBins;
unsigned int ProcessingWindow::iInterpolationThreshold;
double ProcessingWindow::dInducedNoiseThreshold;
double ProcessingWindow::dGlobalThreshold;
double ProcessingWindow::dDiscWritingInterval;
string ProcessingWindow::sThresholdFileName;
string ProcessingWindow::sOutputFileNameDecodedFrame;
string ProcessingWindow::sGradientsFile;
string ProcessingWindow::sInterceptsFile;
string ProcessingWindow::sMomentumFile;
bool ProcessingWindow::bEnableInCorrector;
bool ProcessingWindow::bEnableCabCorrector;
bool ProcessingWindow::bEnableCsaspCorrector;
bool ProcessingWindow::bEnableCsdCorrector;
bool ProcessingWindow::bEnableIdCorrector;
bool ProcessingWindow::bEnableIpCorrector;
bool ProcessingWindow::bEnableMomCorrector;
bool ProcessingWindow::bEnableDbPxlsCorrector;
//bool ProcessingWindow::bWriteCsvFiles;
bool ProcessingWindow::bEnableVector;
bool ProcessingWindow::bEnableDebugFrame;

ProcessingWindow *ProcessingWindow::instance(MainWindow *mw, QWidget *parent)
{

   if (processingWindowInstance == 0)
   {
      processingWindowInstance = new ProcessingWindow(mw, parent);

      // Create hxtProcessing object
      HxtProcessor = new hexitech::HxtProcessing("Hexitech", 0);

      // Register string, vector<string> before we can connect the first two signals with their slots
      qRegisterMetaType<string>("string");
      qRegisterMetaType<vector<string> >("vector<string>");
      qRegisterMetaType<HxtBuffer>("HxtBuffer");

      // Connect signals and slots
      connect(HxtProcessor, SIGNAL(hexitechConsumedFiles(vector<string>)), processingWindowInstance, SLOT(displayHxtProcessingDatFiles(vector<string>)));
      connect(HxtProcessor, SIGNAL(hexitechConsumedBuffers(vector<unsigned short*>)), processingWindowInstance, SLOT(displayHxtProcessingBuffers(vector<unsigned short*>)));
      connect(HxtProcessor, SIGNAL(hexitechProducedFile(string)),          processingWindowInstance, SLOT(displayHxtProcessingHxtFile(string)));
      /// Temporary connect single & slot to demonstrate when Hxt file name changes
//      connect(HxtProcessor, SIGNAL(hxtProcessedFileNameChanged(string)),
//              processingWindowInstance, SLOT(handleHxtProcessedFileNameChanged(string)));

      connect(processingWindowInstance, SIGNAL(updatePrefixSignal(bool)),           HxtProcessor, SLOT(savePrefix(bool)));
      connect(processingWindowInstance, SIGNAL(updateMotorSignal(bool)),            HxtProcessor, SLOT(saveMotor(bool)));
      connect(processingWindowInstance, SIGNAL(updateTimeStampSignal(bool)),        HxtProcessor, SLOT(saveTimeStamp(bool)));
      connect(processingWindowInstance, SIGNAL(updateSameAsRawFileSignal(bool)),    HxtProcessor, SLOT(saveSameAsRawFile(bool)));
      connect(processingWindowInstance, SIGNAL(updateProcessingConditionSignal(processingCondition)),
              HxtProcessor, SLOT(changeProcessingCondition(processingCondition)));
      connect(processingWindowInstance, SIGNAL(updateProcessingVeto(bool)), HxtProcessor, SLOT(processingVetoed(bool)));
      connect(processingWindowInstance, SIGNAL(removeUnprocessedFiles(bool)), HxtProcessor, SLOT(removeFiles(bool)));
      connect(processingWindowInstance, SIGNAL(customFileSelected(bool)), HxtProcessor, SLOT(customFileSelected(bool)));

      /// Bugfix, if user clears gradients/intercepts/momentum/threshold then associated HxtProcessor setting is cleared
      connect(processingWindowInstance->ui->gradientsPathLineEdit, SIGNAL(textChanged(const QString &)),
              processingWindowInstance, SLOT(gradientsPathLineEditChanged(const QString &)));
      connect(processingWindowInstance->ui->globalThresholdPathLineEdit, SIGNAL(textChanged(const QString &)),
              processingWindowInstance, SLOT(globalThresholdPathLineEditChanged(const QString &)));
      connect(processingWindowInstance->ui->interceptsPathLineEdit, SIGNAL(textChanged(const QString &)),
              processingWindowInstance, SLOT(interceptsPathLineEditChanged(const QString &)));
      connect(processingWindowInstance->ui->momentumPathLineEdit, SIGNAL(textChanged(const QString &)),
              processingWindowInstance, SLOT(momentumPathLineEditChanged(const QString &)));

      /// HexitecGigE Addition: Signal when HDF5, CSV  file(s) (de)selected
      /// For DSoFt: I've begun adding connect statement and  tying to slots within mainwindow,
      ///           But I believe that for you to  sort out.
      connect(processingWindowInstance->ui->csvFileCheckBox, SIGNAL(toggled(bool)),
              reinterpret_cast<const QObject*>(mw), SLOT(handleSaveCsvChanged(bool)));
      connect(processingWindowInstance->ui->hdf5FileCheckBox, SIGNAL(toggled(bool)),
              reinterpret_cast<const QObject*>(mw), SLOT(handleSaveH5Changed(bool)));

      /// HexitecGigE Addition: Add slot so processingLoggingCheckBox toggles logging on/off
      connect(processingWindowInstance->ui->processingLoggingCheckBox,  SIGNAL(toggled(bool)),
              HxtProcessor,                                             SLOT(toggleProcessingLogging(bool)));

      connect(HxtProcessor,                         SIGNAL(hexitechRunning(bool)),
              reinterpret_cast<const QObject*>(mw), SLOT(updateHexitechProcessingStatus(bool)) );
      connect(HxtProcessor,                         SIGNAL(hexitechFilesToDisplay(QStringList)),
              reinterpret_cast<const QObject*>(mw), SLOT(readFiles(QStringList)) );
      connect(HxtProcessor, SIGNAL(hxtProcessedFileNameChanged(QString)),
              reinterpret_cast<const QObject*>(mw), SLOT(handleProcessingComplete(QString)));

      /// HexitecGigE Addition: (The following 2 connections)
      // DSoFt: added filename to indicate when a new image/slice begins as this will change.
      // This is a quick fix and should be reviewed.
      connect(HxtProcessor,                         SIGNAL(hexitechBufferToDisplay(unsigned short*, QString)),
              reinterpret_cast<const QObject*>(mw), SLOT(readBuffer(unsigned short*, QString)));

      connect(reinterpret_cast<const QObject*>(mw), SIGNAL(returnHxtBuffer(unsigned short*)),
              HxtProcessor,                         SLOT(handleReturnHxtBuffer(unsigned short*)));

      /// Connect signal from HxtProcessor to receive summed spectrum filename
      connect(HxtProcessor,                         SIGNAL(hexitechSpectrumFile(QString)),
              reinterpret_cast<const QObject*>(mw), SLOT(handleSpectrumFile(QString)));

      connect(processingWindowInstance,             SIGNAL(updateVisualisationSignal(bool)),
              reinterpret_cast<const QObject*>(mw), SLOT(updateVisualisationTab(bool)) );

      connect(HxtProcessor, SIGNAL(hexitechRunning(bool)),          processingWindowInstance, SLOT(updateStateLabel(bool)) );

      // Allow HxtProcessor to communicate problems to GUI
      connect(HxtProcessor, SIGNAL(hexitechSignalError(QString)),   processingWindowInstance, SLOT(handleWriteError(QString)));

      // Allow HxtProcessor signal manual processing finished
      connect(HxtProcessor, SIGNAL(hexitechSignalManualProcessingFinished()), processingWindowInstance, SLOT(guiProcessNowFinished()));
      connect(HxtProcessor, SIGNAL(hexitechUnprocessedFiles(bool)),           processingWindowInstance, SLOT(guiUnprocessedFilesInQueue(bool)));

      // Allow HxtProcessor signal to mainwindow to remove any excess Slice(s)
      connect(HxtProcessor, SIGNAL(hexitechRemoveAnyExcessSlices()), reinterpret_cast<const QObject*>(mw), SLOT(deleteExcessSlices()));

      // Initialise hxtProcessing object
      initHexitechProcessor();
   }

   return processingWindowInstance;
}

hexitech::HxtProcessing *ProcessingWindow::getHxtProcessor()
{
   return HxtProcessor;
}

ProcessingWindow::ProcessingWindow(MainWindow *mw, QWidget *parent) :
   QWidget(parent),
   ui(new Ui::ProcessingWindow)
{

   mainWindow = new QMainWindow();
   mainWindow->setCentralWidget(this);

   hexitechFilename = Parameters::twoEasyIniFilename;
   QSettings settings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   if (settings.contains("hexitecGigEIniFilename"))
   {
      hexitechFilename = settings.value("hexitecGigEIniFilename").toString();
   }
   if (!fileExists(hexitechFilename.toStdString().c_str()))
   {
       handleWriteError(QString("Initialisation Error: File %1 do not exist!").arg( hexitechFilename));
   }

   hexitechIniFile = new IniFile(hexitechFilename);

   ui->setupUi(this);
   ui->stateLabel->setStyleSheet("QLabel { color : red; }");

   connect(ui->processNowButton,        SIGNAL(clicked()), reinterpret_cast<const QObject*>(mw),   SLOT(processNow()));
   connect(ui->clearUnprocessedButton,  SIGNAL(clicked()),      this, SLOT(clearUnprocessedFiles()));

   connect(ui->debugButton,                 SIGNAL(clicked()), this, SLOT(debugButtonPressed()));
   connect(ui->gradientsBrowseButton,       SIGNAL(clicked()), this, SLOT(gradientsBrowseButtonPressed()));
   connect(ui->momentumBrowseButton,        SIGNAL(clicked()), this, SLOT(momentumBrowseButtonPressed()));
   connect(ui->globalThresholdBrowseButton, SIGNAL(clicked()), this, SLOT(globalThresholdBrowseButtonPressed()));
   connect(ui->interceptsBrowseButton,      SIGNAL(clicked()), this, SLOT(interceptsBrowseButtonPressed()));
   connect(ui->loadSettingsButton,          SIGNAL(clicked()), this, SLOT(loadSettingsButtonPressed()));
   connect(ui->saveSettingsButton,          SIGNAL(clicked()), this, SLOT(saveSettingsButtonPressed()));

   /// Create options and populate GUI's ComboBoxes accordingly

   modesOptions << "Manual" << "Automatic";
   ui->dataProcessingComboBox->addItems(modesOptions);

   frequencyOptions << "Motor Any Step" << "Motor Position Step" << "Motor Time Step" << "Every New File";
   ui->processingFrequencyComboBox->addItems(frequencyOptions);

   updateDisplayOptions << "Manual" << "Processed File";
   ui->updateDisplayComboBox->addItems(updateDisplayOptions);

   calibrationOptions << "None" << "Energy" << "Momentum";
   ui->calibrationComboBox->addItems(calibrationOptions);

   yesNoOptions << "Yes" << "No";
   ui->nextFrameCorrectionComboBox->addItems(yesNoOptions);

   chargeSharingCorrectionOptions << "None" << "Addition" << "Discrimination";
   ui->chargeSharingCorrectionComboBox->addItems(chargeSharingCorrectionOptions);

   /// Connect GUI components' signals with handler functions' slots

   connect(ui->dataProcessingComboBox,          SIGNAL(currentIndexChanged(QString)), this, SLOT(dataProcessingComboBoxChanged(QString)) );
   connect(ui->processingFrequencyComboBox,     SIGNAL(currentIndexChanged(QString)), this, SLOT(processingFrequencyComboBoxChanged(QString)) );
   connect(ui->updateDisplayComboBox,           SIGNAL(currentIndexChanged(QString)), this, SLOT(updateDisplayComboBoxChanged(QString)) );

   connect(ui->filePrefixCheckBox,              SIGNAL(toggled(bool)),                this, SLOT(filePrefixCheckBoxToggled(bool)) );
   connect(ui->motorPositionCheckBox,           SIGNAL(toggled(bool)),                this, SLOT(motorPositionCheckBoxToggled(bool)) );
   connect(ui->dataTimeStampCheckBox,           SIGNAL(toggled(bool)),                this, SLOT(dataTimeStampCheckBoxToggled(bool)) );
   connect(ui->customCheckBox,                  SIGNAL(toggled(bool)),                this, SLOT(customCheckBoxToggled(bool)) );
   connect(ui->sameAsRawFileCheckBox,           SIGNAL(toggled(bool)),                this, SLOT(sameAsRawFileCheckBoxToggled(bool)) );
   connect(ui->calibrationComboBox,             SIGNAL(currentIndexChanged(QString)), this, SLOT(calibrationComboBoxChanged(QString)) );
   connect(ui->nextFrameCorrectionComboBox,     SIGNAL(currentIndexChanged(QString)), this, SLOT(nextFrameCorrectionComboBoxChanged(QString)) );
   connect(ui->chargeSharingCorrectionComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(chargeSharingCorrectionComboBoxChanged(QString)) );
   connect(ui->hxtFileLineEdit,                 SIGNAL(textChanged(QString)),         this, SLOT(hxtFileLineEditChanged(QString)) );
   connect(ui->hxtFileSaveButton,               SIGNAL(clicked()),                    this, SLOT(hxtFileSaveButtonPressed()));
   connect(ui->gradientsPathLineEdit,           SIGNAL(textChanged(QString)),         this, SLOT(gradientsPathLineEditChanged(QString)));
   connect(ui->interceptsPathLineEdit,          SIGNAL(textChanged(QString)),         this, SLOT(interceptsPathLineEditChanged(QString)));
   connect(ui->startBinLineEdit,                SIGNAL(textChanged(QString)),         this, SLOT(startBinLineEditChanged(QString)));
   connect(ui->endBinLineEdit,                  SIGNAL(textChanged(QString)),         this, SLOT(endBinLineEditChanged(QString)));
   connect(ui->binWidthLineEdit,                SIGNAL(textChanged(QString)),         this, SLOT(binWidthLineEditChanged(QString)));
   connect(ui->thresholdLineEdit,               SIGNAL(textChanged(QString)),         this, SLOT(thresholdLineEditChanged(QString)));

   trueFalseOptions << "TRUE" << "FALSE";

   bPrefixChecked       = false;
   bMotorChecked        = false;
   bTimeStampChecked    = false;
   bCustomChecked       = false;
   bUseRawFileChecked   = false;

   // Disable option of custom filename until User ticks associated checkbox
   ui->hxtFileLineEdit->setEnabled(false);
   ui->hxtFileSaveButton->setEnabled(false);

   // Hide debug button
   ui->debugButton->setVisible(false);

   createActions();
   createContextMenus();

   // Suppress information about config changes during initialisation
   bSuppressLoadSettingsInfo = true;

   // How often should files be processed
   currentCondition = conditionNoneMet;

   ui->sameAsRawFileCheckBox->hide();
}

ProcessingWindow::~ProcessingWindow()
{
   delete ui;
   delete HxtProcessor;
   delete hexitechIniFile;
}

QMainWindow *ProcessingWindow::getMainWindow()
{
   return mainWindow;
}

void ProcessingWindow::toggleManualProcessingNow(bool bManualEnabled)
{
    HxtProcessor->setManualProcessing(bManualEnabled);
}

void ProcessingWindow::setRawFilesToProcess(QStringList rawFilesList)
{
    /// MainWindow passes list of file(s) to be manually processed
    ///     following the user pressing the ProcessNow button
    HxtProcessor->setRawFilesToProcess(rawFilesList);
}

void ProcessingWindow::createActions()
{
   clearAction = new QAction(tr("Clear"), this);
   clearAction->setShortcut(QKeySequence(tr("Alt+C")));
   clearAction->setStatusTip(tr("Clear output screen"));
}

void ProcessingWindow::createContextMenus()
{
   outputContextMenu = ui->output->createStandardContextMenu();
   outputContextMenu->addAction(clearAction);
   connect(ui->output, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(outputContextMenuEvent(QPoint)));
}

void ProcessingWindow::outputContextMenuEvent(QPoint point)
{
   outputContextMenu->exec(ui->output->mapToGlobal(point));
}

void ProcessingWindow::handleWriteError(QString message)
{
   ui->output->append(message);
}

void ProcessingWindow::handleWriteMessage(QString message)
{
   ui->output->append(message);
}

void ProcessingWindow::initialiseProcessingWindow()
{
    /// Enables mainwindow.cpp to signal to ProcessingWindow to initialise
    /// itself using default settings from HexitecGigE.ini - Gui crashes
    /// if these functions are called from the CTOR directly

    // Ensure default values communicated to HxtProcessor
    dataProcessingComboBoxChanged("Manual");
    processingFrequencyComboBoxChanged("Motor Any Step");
    updateDisplayComboBoxChanged("Manual");
    calibrationComboBoxChanged("None");
    nextFrameCorrectionComboBoxChanged("Yes");
    chargeSharingCorrectionComboBoxChanged("None");
    loadSettingsButtonPressed();

    // Initialisation complete, stop suppressing configuration changes information
    bSuppressLoadSettingsInfo = false;
    // Start HxtProcessing thread
    HxtProcessor->start();
}

void ProcessingWindow::displayHxtProcessingDatFiles(vector<string> fileNames)
{
    /// Enable HxtProcessing object to signal which raw file(s) being processed
    vector<string>::iterator fileIterator;
    for (fileIterator = fileNames.begin(); fileIterator != fileNames.end(); fileIterator++)
    {
        handleWriteMessage(QString(" - Consumed: %1").arg((*fileIterator).c_str()));
    }
}

void ProcessingWindow::displayHxtProcessingBuffers(vector<unsigned short*> bufferNames)
{
    //cout << "processingwindow.cpp:291 sort out displayHxtProcessingBuffers().." << endl;
    /// Enable HxtProcessing object to signal which buffer(s) being processed
    vector<unsigned short*>::iterator fileIterator;
    for (fileIterator = bufferNames.begin(); fileIterator != bufferNames.end(); fileIterator++)
    {
        ;
//        handleWriteMessage(QString("Consumed: %1").arg((*fileIterator).c_str()));
    }
}

void ProcessingWindow::displayHxtProcessingHxtFile(string fileName)
{
    /// Allow HxtProcessing object to signal the name of the produced .hxt file
    handleWriteMessage(QString(" ! Produced: %1").arg(fileName.c_str()));
}

void ProcessingWindow::updateStateLabel(bool isBusy)
{
    /// Allow HxtProcessing to signal GUI when it's [not] processing
    if (isBusy)
        ui->stateLabel->setText("Busy");
    else
        ui->stateLabel->setText("Idle");
}

void ProcessingWindow::debugButtonPressed()
{
    /// Debugging button (hidden from user)
    /// Debug: List current settings
    dumpHexitechConfig();
}

int ProcessingWindow::doesFilePathExist(string filePath)
{
    /// (http://stackoverflow.com/q/5621944/2903608)
    /// Checks if filePath is a directory (Returns 0), a file (1), or doesn't exist (2).
    if ( _access( filePath.c_str(), 0 ) == 0 )
    {
        struct stat status;
        stat( filePath.c_str(), &status );

        if ( status.st_mode & S_IFDIR )
            return 0;
        else
            return 1;
    }
    else
       return 2;
}

string ProcessingWindow::stripFilenameFromFullPath(string filePath)
{
    /// Strips the file name from a full path.
    ///     E.g. "C:\\temp\\dir\\file.hxt"  =>  "C:\\temp\\dir\\"

    QFileInfo fileInfo((filePath.c_str()));
    QString absolutePath = fileInfo.absoluteDir().absolutePath();

    return absolutePath.toStdString();
}

void ProcessingWindow::guiAutomaticProcessingChosen()
{
    /// Disable all GUI components except dataProcessingComboBox
    ui->processNowButton->setEnabled(false);
    ui->processingFrequencyComboBox->setEnabled(false);
    ui->updateDisplayComboBox->setEnabled(false);
    ui->calibrationComboBox->setEnabled(false);
    ui->filePrefixCheckBox->setEnabled(false);
    ui->motorPositionCheckBox->setEnabled(false);
    ui->dataTimeStampCheckBox->setEnabled(false);
    ui->sameAsRawFileCheckBox->setEnabled(false);
    // Only need to disable custom checkbox if sameAsRawFile checkbox not selected
    if (!bUseRawFileChecked)
        ui->customCheckBox->setEnabled(false);

    // Only need to disable hxtFileLineEdit, hxtFileSaveButton
    //      if custom filename enabled
    if (bCustomChecked)
    {
        ui->hxtFileLineEdit->setEnabled(false);
        ui->hxtFileSaveButton->setEnabled(false);
    }
    ui->startBinLineEdit->setEnabled(false);
    ui->endBinLineEdit->setEnabled(false);
    ui->binWidthLineEdit->setEnabled(false);
    ui->thresholdLineEdit->setEnabled(false);
    ui->nextFrameCorrectionComboBox->setEnabled(false);
    ui->chargeSharingCorrectionComboBox->setEnabled(false);
    ui->gradientsPathLineEdit->setEnabled(false);
    ui->gradientsBrowseButton->setEnabled(false);
    ui->interceptsPathLineEdit->setEnabled(false);
    ui->interceptsBrowseButton->setEnabled(false);
    ui->momentumPathLineEdit->setEnabled(false);
    ui->momentumBrowseButton->setEnabled(false);
    ui->globalThresholdPathLineEdit->setEnabled(false);
    ui->globalThresholdBrowseButton->setEnabled(false);
    ui->loadSettingsButton->setEnabled(false);
    ui->saveSettingsButton->setEnabled(false);
    ui->processingLoggingCheckBox->setEnabled(false);
    ui->hdf5FileCheckBox->setEnabled(false);
    ui->csvFileCheckBox->setEnabled(false);
}

void ProcessingWindow::guiManualProcessingChosen()
{
    /// Enable all GUI components except dataProcessingComboBox
    ui->processNowButton->setEnabled(true);
    ui->processingFrequencyComboBox->setEnabled(true);
    ui->updateDisplayComboBox->setEnabled(true);
    ui->calibrationComboBox->setEnabled(true);
    ui->filePrefixCheckBox->setEnabled(true);
    ui->motorPositionCheckBox->setEnabled(true);
    ui->dataTimeStampCheckBox->setEnabled(true);
    // Only enable custom checkbox if sameAsRawFile not selected
    if (!bUseRawFileChecked)
        ui->customCheckBox->setEnabled(true);
    else
        ui->customCheckBox->setEnabled(false);

    // Enable hxtFileLineEdit, hxtFileSaveButton if custom filename enabled
    if (bCustomChecked)
    {
        ui->hxtFileLineEdit->setEnabled(true);
        ui->hxtFileSaveButton->setEnabled(true);
        // If custom filename selected, don't enable sameAsRawFile checkbox
        ui->sameAsRawFileCheckBox->setEnabled(false);   // Just to double check
    }
    else
    {
        // custom filenames not selected, enable sameAsRawFile checkbox
        ui->sameAsRawFileCheckBox->setEnabled(true);
    }
    ui->startBinLineEdit->setEnabled(true);
    ui->endBinLineEdit->setEnabled(true);
    ui->binWidthLineEdit->setEnabled(true);
    ui->thresholdLineEdit->setEnabled(true);
    ui->nextFrameCorrectionComboBox->setEnabled(true);
    ui->chargeSharingCorrectionComboBox->setEnabled(true);
    ui->gradientsPathLineEdit->setEnabled(true);
    ui->gradientsBrowseButton->setEnabled(true);
    ui->interceptsPathLineEdit->setEnabled(true);
    ui->interceptsBrowseButton->setEnabled(true);
    ui->momentumPathLineEdit->setEnabled(true);
    ui->momentumBrowseButton->setEnabled(true);
    ui->globalThresholdPathLineEdit->setEnabled(true);
    ui->globalThresholdBrowseButton->setEnabled(true);
    ui->loadSettingsButton->setEnabled(true);
    ui->saveSettingsButton->setEnabled(true);
    ui->processingLoggingCheckBox->setEnabled(true);
    ui->hdf5FileCheckBox->setEnabled(true);
    ui->csvFileCheckBox->setEnabled(true);
}

void ProcessingWindow::guiProcessNowStarted()
{
    /// Disabled all GUI components while raw files processed in background
    /// Why reinvent the wheel? Disable all components as when Data Process
    /// set to Automatic..
    guiAutomaticProcessingChosen();
    /// ..and disable dataProcessing itself
    ui->dataProcessingComboBox->setEnabled(false);
}

void ProcessingWindow::guiProcessNowFinished()
{
    /// Enabled all GUI components when raw files finished processing in background
    /// Why reinvent the wheel? Enable all components as when Data Process
    /// set to Manual..
    guiManualProcessingChosen();
    /// ..and enable the processNowButton itself
    ui->dataProcessingComboBox->setEnabled(true);
}

void ProcessingWindow::guiUnprocessedFilesInQueue(bool bFilesPresent)
{
    /// If HxtProcessor contain unprocessed files:
    ///     disable dataProcessingComboBox, enable clearUnprocessedButton
    if (bFilesPresent)
    {
        ui->dataProcessingComboBox->setEnabled(false);
        ui->clearUnprocessedButton->setEnabled(true);
    }
    else    // No unprocessed files, do the reverse
    {
        ui->dataProcessingComboBox->setEnabled(true);
        ui->clearUnprocessedButton->setEnabled(false);
    }
}

void ProcessingWindow::clearUnprocessedFiles()
{
    /// Signal to HxtProcessing to drop all unprocessed raw files from queue
    emit removeUnprocessedFiles(true);
    ui->dataProcessingComboBox->setEnabled(true);           // Re-enable data processing combo box
    ui->clearUnprocessedButton->setEnabled(false);          // Disable this button until data processing set to "Automatic"
    ui->dataProcessingComboBox->setCurrentText("Manual");   // Change data processing to Manual
}


void ProcessingWindow::dataProcessingComboBoxChanged(QString aString)
{
    /// Communicate change of Data Processing to main window and INI file
    if (areStringsEqual(aString, "Automatic"))
    {
        // Automatic - process data, disable tab's GUI components
        guiAutomaticProcessingChosen();
        emit updateProcessingVeto(false);               // Don't override processing
        emit updateMainWindowDataTakingSignal(false);   // Disable MainWindow take data icon?
        // Update Number of Bins in case changed while in "Manual"
        updateNumberOfBins();
        // Re-enable clearUnprocessedButton
        ui->clearUnprocessedButton->setEnabled(true);
        // Disable this GUI element (until user presses clearUnprocessedButton)
        ui->dataProcessingComboBox->setEnabled(false);
    }
    else
    {
        // Manual - Don't process data, enable tab's GUI components
        guiManualProcessingChosen();
        emit updateProcessingVeto(true);                // Override processing
        emit updateMainWindowDataTakingSignal(true);    // Enable  MainWindow Take Data Icon?
        // Disable clearUnprocessedButton
        ui->clearUnprocessedButton->setEnabled(false);
    }

    // Prepare to update INI file entry when saveSettingsButton's pressed
    hexitechIniFile->setParameter( "Processing/DataProcessing", QVariant(aString));
}

void ProcessingWindow::processingFrequencyComboBoxChanged(QString aString)
{    
    if (areStringsEqual(aString, "Motor Any Step"))
    {
        currentCondition = conditionMotorAnyStep;
    }
    else if(areStringsEqual(aString, "Motor Position Step"))
    {
        currentCondition = conditionMotorPositionStep;
    }
    else if(areStringsEqual(aString, "Motor Time Step"))
    {
        currentCondition = conditionMotorTimeStep;
    }
    else
    {
        currentCondition = conditionEveryNewFile;
    }
    emit updateProcessingConditionSignal(currentCondition);

    // Prepare to update INI file entry when saveSettingsButton's pressed
    hexitechIniFile->setParameter("Processing/AutomaticProcessing", QVariant(aString));
}

void ProcessingWindow::updateDisplayComboBoxChanged(QString aString)
{
    /// Do [not] display processed data
    if (areStringsEqual(aString, "Manual"))
    {
        // Manual - Do not update visualisation tab with processed data
        emit updateVisualisationSignal(false);
    }
    else
    {
        // Automatic - update visualisation tab with processed data
        emit updateVisualisationSignal(true);
    }

    // Prepare to update ini file entry when saveSettingsButton's pressed
    hexitechIniFile->setParameter("Processing/UpdateDisplay", QVariant(aString));
}

void ProcessingWindow::filePrefixCheckBoxToggled(bool isChecked)
{
    /// Signal emitted when filePrefix checkbox clicked
    bPrefixChecked = isChecked;
    // Signal to HxtProcessing the new checkbox choice
    emit updatePrefixSignal(bPrefixChecked);
}

void ProcessingWindow::motorPositionCheckBoxToggled(bool isChecked)
{
    /// Signalled when the motorPosition checkbox clicked
    bMotorChecked = isChecked;
    // Signal to HxtProcessing that checkbox toggled
    emit updateMotorSignal(bMotorChecked);
}

void ProcessingWindow::dataTimeStampCheckBoxToggled(bool isChecked)
{
    /// Signalled when the dataTimeStamp checkbox clicked
    bTimeStampChecked = isChecked;
    // Signal to HxtProcessing that checkbox toggled
    emit updateTimeStampSignal(bTimeStampChecked);
}

void ProcessingWindow::customCheckBoxToggled(bool isChecked)
{
    /// Signal emitted when custom checkbox clicked
    ///     custom and sameAsRawFile Checkboxes are mutually exclusive
    ///     if one is enabled, disable the other

    bCustomChecked = isChecked;
    /// HexitecGigE Addition; Signal to HexitecGigE/HxtProcessing status of custom file selection
    customFileSelected(bCustomChecked);

    // If selected, enable option to set/save custom filename
    //      and save default filenames
    if (bCustomChecked)
    {
        ui->sameAsRawFileCheckBox->setEnabled(false);
        ui->hxtFileLineEdit->setEnabled(true);
        ui->hxtFileSaveButton->setEnabled(true);
        defaultDecodedFileName  = "pixelHisto.hxt";
    }
    else
    {
        ui->sameAsRawFileCheckBox->setEnabled(true);
        ui->hxtFileLineEdit->setEnabled(false);
        ui->hxtFileSaveButton->setEnabled(false);
        // Overwrite custom filenames with default values
        HxtProcessor->setOutputFileNameDecodedFrame(defaultDecodedFileName);
        // Update (now locked) hxtFileLineEdit with default filename
        ui->hxtFileLineEdit->setText(QString(defaultDecodedFileName.c_str()));
    }

    // Check that filename is appropriate
    emit hxtFileLineEditChanged(ui->hxtFileLineEdit->text());
}

void ProcessingWindow::sameAsRawFileCheckBoxToggled(bool isChecked)
{
    /// Signal emitted when sameAsRawFileCheckBox clicked
    ///     custom and sameAsRawFile Checkboxes are mutually exclusive
    ///     if one is enabled, disable the other

    bUseRawFileChecked = isChecked;
    // Signal to HxtProcessing that this check box toggled
    emit updateSameAsRawFileSignal(bUseRawFileChecked);

    // If selected, disable custom checkbox
    if (bUseRawFileChecked)
    {
        ui->customCheckBox->setEnabled(false);
    }
    else
    {
        ui->customCheckBox->setEnabled(true);
        // Same as raw file deselected, reset filenames to defaults
        HxtProcessor->setOutputFileNameDecodedFrame(defaultDecodedFileName);
        // Update hxtFileLineEdit with default filename (just to be on the safe side)
        ui->hxtFileLineEdit->setText(QString(defaultDecodedFileName.c_str()));
    }
}

void ProcessingWindow::calibrationComboBoxChanged(QString aString)
{
    /// HexitecGigE: Calibration options now: None (ADU) / Energy (KeV) / Momentum (including Energy; nm^-1)

    /// Prepare to update file entry when saveSettingsButton's subsequently pressed

    QString calibString = "";
    if (aString.toStdString() == calibrationOptions.at(0).toStdString())
    {
        // None
        bEnableMomCorrector = false;
        bEnableCabCorrector = false;
        calibString = "None";
        // Rename Spectrum Options' unit labels to read ADU
        changeSpectrumOptionsUnitLabels("ADU");
    }
    else if (aString.toStdString() == calibrationOptions.at(1).toStdString())
    {
        // Energy
        bEnableMomCorrector = false;
        bEnableCabCorrector = true;
        calibString = "Energy";
        // Rename Spectrum Options' unit labels to read KeV
        changeSpectrumOptionsUnitLabels("KeV");
    }
    else
    {
        // Momentum
        bEnableMomCorrector = true;
        bEnableCabCorrector = true;
        calibString = "Momentum";
        // Rename Spectrum Options' unit labels to read nm^-1
        changeSpectrumOptionsUnitLabels("nm^-1");
    }
    // Update ini file and processing object
    hexitechIniFile->setParameter("Processing/Calibration", QVariant(calibString));
    HxtProcessor->setEnableMomCorrector(bEnableMomCorrector);
    HxtProcessor->setEnableCabCorrector(bEnableCabCorrector);

    // Reload gui's Spectrum Options with default values from HexitecGigE.ini file
    //      but not during GUI initialisation
    if (!bSuppressLoadSettingsInfo)
        reloadSpectrumOptionsDefaultValues();
}

void ProcessingWindow::reloadSpectrumOptionsDefaultValues()
{
    /// HexitecGigE: Calibration options now: None / Energy / Momentum (including Energy)
    ///
    /// Update Spectrum Options with default values from hexitecGigE.ini file
    ///     dependent upon Calibration choice

    // First check if Momentum enabled
    if (bEnableMomCorrector)
    {
        iHistoStartVal   = hexitechIniFile->getInt("Processing/StartNM");
        iHistoEndVal     = hexitechIniFile->getInt("Processing/EndNM");
        dHistoBins       = hexitechIniFile->getDouble("Processing/BinWidthNM");
    }
    else
    {
        /// Because Momentum Calibration includes Energy Calibration, only look for KeV units if Momentum disabled
        if (bEnableCabCorrector)
        {
            iHistoStartVal   = hexitechIniFile->getInt("Processing/StartKEV");
            iHistoEndVal     = hexitechIniFile->getInt("Processing/EndKEV");
            dHistoBins       = hexitechIniFile->getDouble("Processing/BinWidthKEV");
        }
        else
        {
            // Calibration disabled, use ADU units
            iHistoStartVal   = hexitechIniFile->getInt("Processing/StartADU");
            iHistoEndVal     = hexitechIniFile->getInt("Processing/EndADU");
            dHistoBins       = hexitechIniFile->getDouble("Processing/BinWidthADU");
        }
    }
    // Global threshold always in ADU though:
    dGlobalThreshold = hexitechIniFile->getDouble("Processing/ThresholdADU");

    // Update gui spectrum components with processing.any file's values
    ui->startBinLineEdit->setText( QString::number(iHistoStartVal));
    ui->endBinLineEdit->setText(QString::number(iHistoEndVal));
    ui->binWidthLineEdit->setText(QString::number(dHistoBins));
    ui->thresholdLineEdit->setText(QString::number(dGlobalThreshold));

    // Binary Width (dHistoBins) is not the same as Number of Bins (iNumberBins)
    //  Update Number of Bins by calling this function:
    updateNumberOfBins();
}

void ProcessingWindow::nextFrameCorrectionComboBoxChanged(QString aString)
{
    /// Controls the Incomplete Data Corrector
    /// Prepare to update file entry when saveSettingsButton's pressed

    QString frameString = "FALSE";
    // Selected "Yes"?
    if (areStringsEqual(aString, "Yes"))
    {
        bEnableIdCorrector = true;
        frameString = "TRUE";
    }
    else
    {
        bEnableIdCorrector = false;
    }

    // Update ini file and processing object
    hexitechIniFile->setParameter("Processing/IncompleteDataCorrection", QVariant(frameString));
    HxtProcessor->setEnableIdCorrector(bEnableIdCorrector);
}

void ProcessingWindow::chargeSharingCorrectionComboBoxChanged(QString aString)
{
    /// Controls the Charge Sharing and Charge Addition Correctors
    QString cscString = "";
    if (aString.toStdString() == chargeSharingCorrectionOptions.at(0).toStdString())
    {
        // None
        bEnableCsdCorrector = false;
        bEnableCsaspCorrector = false;
        cscString = "None";
    }
    else if (aString.toStdString() == chargeSharingCorrectionOptions.at(1).toStdString())
    {
        // Addition
        bEnableCsdCorrector = false;
        bEnableCsaspCorrector = true;
        cscString = "Addition";
    }
    else
    {
        // Discrimination
        bEnableCsdCorrector = true;
        bEnableCsaspCorrector = false;
        cscString = "Discrimination";
    }
    // Update ini file and processing object
    hexitechIniFile->setParameter("Processing/ChargeSharingCorrection", QVariant(cscString));
    HxtProcessor->setEnableCsdCorrector(bEnableCsdCorrector);
    HxtProcessor->setEnableCsaspCorrector(bEnableCsaspCorrector);
}

void ProcessingWindow::hxtFileLineEditChanged(QString newPrefixString)
{
    /// Signalled when hxt filename changed
    string newPrefixFilename = newPrefixString.toStdString();
    // Check new filename will not overwrite an existing file
    if (fileExists(newPrefixFilename.c_str()))
    {
        handleWriteMessage("WARNING: The input HXT filename already exists.");
    }
    else
    {
        // It's a new filename, update HxtProcessing object and HexitecGigE.ini file object
        sOutputFileNameDecodedFrame = newPrefixFilename;
        HxtProcessor->setOutputFileNameDecodedFrame( sOutputFileNameDecodedFrame);
        hexitechIniFile->setParameter("Processing/DecodedFrameFilename", QVariant(sOutputFileNameDecodedFrame.c_str()));
    }
}

void ProcessingWindow::hxtFileSaveButtonPressed()
{
    QString currentDirectory = QString("");

    QString newHxtFilename = QFileDialog::getSaveFileName(this, tr("Save Hexitech File"), currentDirectory,
                                                          tr("Hexitech Files (*.hxt)"));
    // New filename selected?
    if (!newHxtFilename.toStdString().empty())
    {
        // Update GUI and HxtProcessing settings
        ui->hxtFileLineEdit->setText( newHxtFilename);
        // Check file name valid and communicate changed name onto HexitecGigE.ini, HxtProcessing
        emit hxtFileLineEditChanged( newHxtFilename);
    }
}

void ProcessingWindow::momentumBrowseButtonPressed()
{
   /* Get a valid Momentum absolute filename */
   QString currentDirectory = QString("");

   QString newMomentumFilename = QFileDialog::getOpenFileName(this, tr("Open Momentum Values File"), currentDirectory,
                                                               tr("Momentum Values Files (*.txt)") );
   emit momentumPathLineEditChanged(newMomentumFilename);
}

void ProcessingWindow::momentumPathLineEditChanged(const QString &momentumFile)
{
    if (momentumFile.toStdString().empty())
    {
        // File empty, clear correspondence setting within HxtProcessing
        sMomentumFile = momentumFile.toStdString();
        HxtProcessor->setMomentumFile(sMomentumFile);
        // Check whether momentum specified, if yes warn user momentum file required
        if (ui->calibrationComboBox->currentText()  == calibrationOptions.at(2))
            handleWriteMessage("Warning: \"Momentum File\" cleared but required when Momentum Calibration selected");
    }
    else
    {
        /// Use new momentum file name if it exists
        if (fileExists(momentumFile.toStdString().c_str()))
        {
            ui->momentumPathLineEdit->setText(momentumFile);
            sMomentumFile = momentumFile.toStdString();
            HxtProcessor->setMomentumFile(sMomentumFile);
            // Need not call hexitechIniFile->setParameter() - saveSettingsButton handles this
        }
        else
            handleWriteError("Error: \"Momentum File\" specified file doesn't exist");
    }
}

void ProcessingWindow::globalThresholdBrowseButtonPressed()
{
   /* Get a valid GlobalThreshold absolute filename */
   QString currentDirectory = QString("");

   QString newGlobalThresholdFilename = QFileDialog::getOpenFileName(this, tr("Open Global Threshold Values File"), currentDirectory,
                                                               tr("Global Threshold Values Files (*.txt)") );
   emit globalThresholdPathLineEditChanged(newGlobalThresholdFilename);
}

void ProcessingWindow::globalThresholdPathLineEditChanged(const QString &globalThresholdFile)
{
    if (globalThresholdFile.toStdString().empty())
    {
        // File empty, clear corresponding setting within HxtProcessing
        sThresholdFileName = globalThresholdFile.toStdString();
        HxtProcessor->setThresholdFileName(sThresholdFileName);
    }
    else
    {
        /// Use new globalThreshold file name if it exists
        if (fileExists(globalThresholdFile.toStdString().c_str()))
        {
            ui->globalThresholdPathLineEdit->setText(globalThresholdFile);
            sThresholdFileName = globalThresholdFile.toStdString();
            HxtProcessor->setThresholdFileName(sThresholdFileName);
            // Need not call hexitechIniFile->setParameter() - saveSettingsButton handles this
        }
        else
            handleWriteError("Error: \"Global Threshold File\" specified file doesn't exist");
    }
}

void ProcessingWindow::gradientsBrowseButtonPressed()
{
   /* Get a valid Gradients absolute filename */
   QString currentDirectory = QString("");

   QString newGradientsFilename = QFileDialog::getOpenFileName(this, tr("Open Gradients Values File"), currentDirectory,
                                                               tr("Gradients Values Files (*.txt)") );
   emit gradientsPathLineEditChanged(newGradientsFilename);
}

void ProcessingWindow::gradientsPathLineEditChanged(const QString &gradientsFile)
{
    if (gradientsFile.toStdString().empty())
    {
        sGradientsFile = gradientsFile.toStdString();
        HxtProcessor->setGradientsFile(sGradientsFile);
        if (ui->calibrationComboBox->currentText() == calibrationOptions.at(1))
            handleWriteMessage("Warning: \"Gradients File\" cleared but required when Energy Calibration selected");
    }
    else
    {
        /// Use new gradients file name if it exists
        if (fileExists(gradientsFile.toStdString().c_str()))
        {
            ui->gradientsPathLineEdit->setText(gradientsFile);
            sGradientsFile = gradientsFile.toStdString();
            HxtProcessor->setGradientsFile(sGradientsFile);
            // Need not call hexitechIniFile->setParameter() - saveSettingsButton handles this
        }
        else
            handleWriteError("Error: \"Gradients File\" specified file doesn't exist");
    }
}

void ProcessingWindow::interceptsBrowseButtonPressed()
{
    /* Get a valid Intercepts absolute filename */
    QString currentDirectory = QString("");

    QString newInterceptsFilename = QFileDialog::getOpenFileName(this, tr("Open Intercepts Values File"), currentDirectory,
                                                                 tr("Intercepts Values Files (*.txt)") );
    emit interceptsPathLineEditChanged(newInterceptsFilename);
}

void ProcessingWindow::interceptsPathLineEditChanged(const QString &interceptsFile)
{
    if (interceptsFile.toStdString().empty())
    {
        sInterceptsFile = interceptsFile.toStdString();
        HxtProcessor->setInterceptsFile(sInterceptsFile);
        if (ui->calibrationComboBox->currentText() == calibrationOptions.at(1))
            handleWriteMessage("Warning: \"Intercepts File\" cleared but required when Energy Calibration selected");
    }
    else
    {
        /// Use new intercepts filename if it exists
        if (fileExists(interceptsFile.toStdString().c_str()))
        {
            ui->interceptsPathLineEdit->setText(interceptsFile);
            sInterceptsFile = interceptsFile.toStdString();
            HxtProcessor->setInterceptsFile(sInterceptsFile);
            // Need not call hexitechIniFile->setParameter() - saveSettingsButton handles this
        }
        else
            handleWriteError("Error: \"Intercepts File\" specified file doesn't exist");
    }
}

void ProcessingWindow::startBinLineEditChanged(QString aString)
{
    /// Assign new value in aString to HistoStartVal if it's an integer
    bool errorOk = true;
    // Convert string into int
    int newStartBin = aString.toInt(&errorOk);
    if (errorOk)
    {
        iHistoStartVal = newStartBin;
        // Is it KeV, NM or ADU?
        QString unit = ui->startUnitLabel->text();
        if (areStringsEqual(unit, "KeV"))
            hexitechIniFile->setParameter("Processing/StartKEV", QVariant(iHistoStartVal));
        else if (areStringsEqual(unit, "nm^-1"))
            hexitechIniFile->setParameter("Processing/StartNM", QVariant(iHistoStartVal));
        else  if (areStringsEqual(unit, "ADU"))
            hexitechIniFile->setParameter("Processing/StartADU", QVariant(iHistoStartVal));
        else
            handleWriteError(QString("Error: Bin Start unrecognised unit type: %1").arg(unit));

        // Set new Histogram start value
        HxtProcessor->setHistoStartVal(iHistoStartVal);
    }
    else
        handleWriteError("Error converting \"Start Bin\" GUI entry into an integer.");
}

void ProcessingWindow::endBinLineEditChanged(QString aString)
{
    /// Assign new value in aString to EndStartVal if it's an integer
    bool errorOk = true;
    // Convert string into int
    int newEndBin = aString.toInt(&errorOk);
    if (errorOk)
    {
        iHistoEndVal = newEndBin;
        // Is it KeV, NM or ADU?
        QString unit = ui->endUnitLabel->text();
        if (areStringsEqual(unit, "KeV"))
            hexitechIniFile->setParameter("Processing/EndKEV", QVariant(iHistoEndVal));
        else if (areStringsEqual(unit, "nm^-1"))
            hexitechIniFile->setParameter("Processing/EndNM", QVariant(iHistoEndVal));
        else  if (areStringsEqual(unit, "ADU"))
            hexitechIniFile->setParameter("Processing/EndADU", QVariant(iHistoEndVal));
        else
            handleWriteError(QString("Error: Bin End unrecognised unit type: %1").arg(unit));
        // Set new Histogram end value
        HxtProcessor->setHistoEndVal(iHistoEndVal);
    }
    else
        handleWriteError("Error converting \"End Bin\" GUI entry into an integer.");
}

void ProcessingWindow::binWidthLineEditChanged(QString aString)
{
    /// Assign new value in aString to dHistoBins if it's a double
    bool errorOk = true;
    // Convert string into double
    double newBinWidth = aString.toDouble(&errorOk);
    if (errorOk)
    {
        dHistoBins = newBinWidth;
        // Is it KeV, NM or ADU?
        QString unit = ui->binUnitLabel->text();
        if (areStringsEqual(unit, "KeV"))
            hexitechIniFile->setParameter("Processing/BinWidthKEV", QVariant(dHistoBins));
        else if (areStringsEqual(unit, "nm^-1"))
            hexitechIniFile->setParameter("Processing/BinWidthNM", QVariant(dHistoBins));
        else  if (areStringsEqual(unit, "ADU"))
            hexitechIniFile->setParameter("Processing/BinWidthADU", QVariant(dHistoBins));
        else
            handleWriteError(QString("Error: Bin Width unrecognised unit type: %1").arg(unit));
        /// Bin width is not number of bins ! See function updateNumberOfBins()
    }
    else
        handleWriteError("Error converting \"Bin Width\" GUI entry into a double.");
}

void ProcessingWindow::updateNumberOfBins()
{
    /// Calculate number of bins based upon Spectrum Option values
    iNumberBins = (iHistoEndVal - iHistoStartVal) / dHistoBins;
    HxtProcessor->setHistoBins(iNumberBins);
    //handleWriteMessage(QString("iNumberBins equation: (%1 - %2)/ %3 = %4").arg(iHistoEndVal).arg(iHistoStartVal).arg(dHistoBins).arg(iNumberBins));
}

void ProcessingWindow::thresholdLineEditChanged(QString aString)
{
    /// Assign new value in aString to dGlobalThreshold if it's a float
    bool errorOk = true;
    // Convert string into float
    double newThreshold = aString.toDouble(&errorOk);
    if (errorOk)
    {
        dGlobalThreshold = newThreshold;
        // Global threshold always in ADU
        hexitechIniFile->setParameter("Processing/ThresholdADU", QVariant(dGlobalThreshold));
        // Set new threshold
        HxtProcessor->setGlobalThreshold(dGlobalThreshold);
    }
    else
        handleWriteError("Error converting \"Threshold\" GUI entry into a float.");
}

void ProcessingWindow::updateCalibrationFileEntry(QString gradientsFile, QString interceptsFile)
{
    /// Prepare gradients and intercepts files to be saved into the hexitecGigE.ini file
    QString calibString = gradientsFile + " " + interceptsFile;
    hexitechIniFile->setParameter("Processing/CalibrationFile", QVariant(calibString));
}

bool ProcessingWindow::split(const string &sCalibrationFiles, string* sGradient, string* sIntercept)
{
    /// Extract Gradients and Intercepts filename from key "Calibration File" entry of HexitecGigE.ini file
    stringstream sStreamFile(sCalibrationFiles);
    string sFilename;
    bool errorOk = true;

    if (!getline(sStreamFile, sFilename, ' '))
        errorOk = false;
    else
       *sGradient = sFilename;
    if (!std::getline(sStreamFile, sFilename, ' '))
        errorOk = false;
    else
        *sIntercept = sFilename;
    return errorOk;
}

void ProcessingWindow::changeSpectrumOptionsUnitLabels(QString newUnit)
{
    /// Modify the unit label for "Start Bin", "End Bin", "Bin Width"
    ///     to be KeV or ADU depending on whether Calibration enabled/disabled
    ui->startUnitLabel->setText(newUnit);
    ui->endUnitLabel->setText(newUnit);
    ui->binUnitLabel->setText(newUnit);
    // Global Threshold always in ADU - Do not change it's label
}

void ProcessingWindow::loadSettingsButtonPressed()
{
    /// Reload file setting into memory when button is pressed

    // Check that hexitechFilename actually exists
    if (!fileExists(hexitechFilename.toStdString().c_str()))
    {
        handleWriteError(QString("Error loading settings; No file named: %1").arg( hexitechFilename));
        return;
    }
    // Reopen file in case changes have been made
    delete hexitechIniFile;
    hexitechIniFile = new IniFile(hexitechFilename);

    QString fileDataProcessing = hexitechIniFile->getString("Processing/DataProcessing");
    QString guiDataProcessing  = ui->dataProcessingComboBox->currentText();
    // Only update GUI if file's setting differs from that of the GUI's setting
    if (!areStringsEqual(fileDataProcessing, guiDataProcessing))
    {
        // Determine which index corresponds to file's value
        int index = ui->dataProcessingComboBox->findText(fileDataProcessing);
        if (index == -1)
            handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("DataProcessing", fileDataProcessing));
        else
        {
            ui->dataProcessingComboBox->setCurrentIndex(index);
            if (!bSuppressLoadSettingsInfo)
                handleWriteMessage(QString("Updated DataProcessing to: %1").arg(fileDataProcessing));
        }
    }

    QString fileAutomaticProcessing = hexitechIniFile->getString("Processing/AutomaticProcessing");
    QString guiAutomaticProcessing = ui->processingFrequencyComboBox->currentText();
    // Only update GUI if file's setting differs from that of the GUI's setting
    if (!areStringsEqual(fileAutomaticProcessing, guiAutomaticProcessing))
    {
        // Determine which index corresponds to the file's value
        int index = ui->processingFrequencyComboBox->findText(fileAutomaticProcessing);
        if (index == -1)
            handleWriteError(QString("File: %1 contains invalid value: \"%2").arg("AutomaticProcessing", fileAutomaticProcessing));
        else
        {
            ui->processingFrequencyComboBox->setCurrentIndex(index);
            if (!bSuppressLoadSettingsInfo)
                handleWriteMessage(QString("Updated AutomaticProcessing to: %1").arg(fileAutomaticProcessing));
        }
    }

    QString fileUpdateDisplay = hexitechIniFile->getString("Processing/UpdateDisplay");
    QString guiUpdateDisplay = ui->updateDisplayComboBox->currentText();
    // Only update GUI if file's setting differs from that of the GUI's setting
    if (!areStringsEqual(fileUpdateDisplay, guiUpdateDisplay))
    {
        // Determine which index corresponds to the file's value
        int index = ui->updateDisplayComboBox->findText(fileUpdateDisplay);
        if (index == -1)
            handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("UpdateDisplay", fileUpdateDisplay));
        else
        {
            ui->updateDisplayComboBox->setCurrentIndex(index);
            if (!bSuppressLoadSettingsInfo)
                handleWriteMessage(QString("Updated Update Display to: %1").arg(fileUpdateDisplay));
        }
    }

    QString qsCalibrationKeyName = QString("Processing/CalibrationFile");
    QString sCalibrationFile = hexitechIniFile->getString(qsCalibrationKeyName);
    if (sCalibrationFile > 0)
    {
        // Split sCalibrationFile into sGradientsFile and sInterceptsFile
        if (!split(sCalibrationFile.toStdString(), &sGradientsFile, &sInterceptsFile))
            handleWriteError("Invalid file(s) specified in entry \"CalibrationFile\".");
    }
    else
    {
        // sCalibrationFile empty; Therefore Gradients and Intercepts files empty
        sGradientsFile = "";
        sInterceptsFile = "";
    }

    QString qsMomentumKeyName = QString("Processing/MomentumFile");
    QString qsMomFileName = hexitechIniFile->getString(qsMomentumKeyName);
    if (qsMomFileName > 0)
    {
        sMomentumFile = qsMomFileName.toUtf8().constData();
    }
    else
    {
        sMomentumFile = "";
    }

    // Check files exist; Notify user if file doesn't (return "" if empty/doesn't exist)
    sGradientsFile = validateFileName(&qsCalibrationKeyName, &(QString(sGradientsFile.c_str())));
    sInterceptsFile = validateFileName(&qsCalibrationKeyName, &(QString(sInterceptsFile.c_str())));
    sMomentumFile = validateFileName(&qsMomFileName, &(QString(sMomentumFile.c_str())));

    // Update GUI
    ui->gradientsPathLineEdit->setText(QString(sGradientsFile.c_str()));
    ui->interceptsPathLineEdit->setText(QString(sInterceptsFile.c_str()));
    ui->momentumPathLineEdit->setText(QString(sMomentumFile.c_str()));

    /// Expand Calibrations options to include: None [Calibration=False, Momentum=False]
    ///                                         Energy [Calibration=T, Momentum=F]
    ///                                         Momentum [Calibration=T, Momentum=T]

    QString fileCalibrationCorrection = hexitechIniFile->getString("Processing/Calibration");
    // Checked that file entry is valid
    if (sanityCheckQString(calibrationOptions, fileCalibrationCorrection))
    {
        bool bFileMom = false;
        bool bFileCab = false;
        // File entry valid; is file entry "None"?
        if ( areStringsEqual(fileCalibrationCorrection, "None"))
        {
            // File entry is "None"; set Booleans correspondingly
            //  (both initialised to false so need not change them here)

            // If GUI's settings doesn't both match File's settings, update GUI's settings
            if ( (bEnableMomCorrector != bFileMom) || (bEnableCabCorrector != bFileCab))
            {
                bEnableMomCorrector = false;
                bEnableCabCorrector = false;
                ui->calibrationComboBox->setCurrentText(fileCalibrationCorrection);
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Disabled Calibration.");
            }
        }
        else if ( areStringsEqual(fileCalibrationCorrection, "Energy"))
        {
            // File entry is "Energy" - set Booleans correspondingly
            bFileMom = false;
            bFileCab = true;
            // If GUI's settings doesn't both match File's settings, update GUI's settings
            if ( (bEnableMomCorrector != bFileMom) || (bEnableCabCorrector != bFileCab))
            {
                bEnableMomCorrector = false;
                bEnableCabCorrector = true;
                ui->calibrationComboBox->setCurrentText(fileCalibrationCorrection);
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Selected Energy Calibration.");
            }
        }
        else if ( areStringsEqual(fileCalibrationCorrection, "Momentum"))
        {
            // File entry is "Momentum" - set Booleans correspondingly
            bFileMom = true;
            bFileCab = true;    //false;
            // If GUI's settings doesn't both match File's settings, update GUI's settings
            if ( (bEnableMomCorrector != bFileMom) || (bEnableCabCorrector != bFileCab))
            {
                bEnableMomCorrector = true;
                bEnableCabCorrector = true; //false;
                ui->calibrationComboBox->setCurrentText(fileCalibrationCorrection);
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Selected Energy & Momentum Calibration.");
            }
        }
    }
    else
        handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("Calibration", fileCalibrationCorrection));

//    QString fileCalibrationCorrection = hexitechIniFile->getString("Processing/Calibration");
//    QString calibrationStatus = QString("TRUE");

//    // Check that file entry is valid
//    if (sanityCheckQString(calibrationOptions, fileCalibrationCorrection))
//    {
//        // File entry is valid - Is file entry different from the GUI's?
//        bool bFileCalibrationEnabled = areStringsEqual(fileCalibrationCorrection, calibrationStatus);
//        // Only update GUI if file's setting differs from that of the GUI's setting
//        if (bEnableCabCorrector != bFileCalibrationEnabled)
//        {
//            // File setting differs from GUI; Has it been enabled?
//            if (bFileCalibrationEnabled)
//            {
//                // Calibration enabled; But are Gradients and Intercepts files defined?
//                if ( (sGradientsFile.empty()) || (sInterceptsFile.empty()) )
//                {
//                    handleWriteError("Cannot do Calibration without specifying gradients file and intercepts file");
//                }
//                else
//                {
//                    // Both calibration files specified, and Calibration enabled in file; Enable corrector
//                    bEnableCabCorrector = bFileCalibrationEnabled;
//                    // Update GUI
//                    ui->calibrationComboBox->setCurrentText("Yes");
//                    if (!bSuppressLoadSettingsInfo)
//                        handleWriteMessage("Updated Calibration: Now enabled.");
//                }
//            }
//            else
//            {
//                // Calibration disabled
//                calibrationStatus = QString("FALSE");
//                bEnableCabCorrector = bFileCalibrationEnabled;
//                ui->calibrationComboBox->setCurrentText("No");
//                if (!bSuppressLoadSettingsInfo)
//                    handleWriteMessage("Updated Calibration: Now disabled.");
//            }
//        }
//    }
//    else
//        handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("Calibration", fileCalibrationCorrection));

    /// HexitecGigE: Calibration options now: None / Energy / Momentum (including Energy)

    if (bEnableCabCorrector)
    {
        // Is Calibration both Momentum and Energy?
        if (bEnableMomCorrector)
        {
            iHistoStartVal   = hexitechIniFile->getInt("Processing/StartNM");
            iHistoEndVal     = hexitechIniFile->getInt("Processing/EndNM");
            dHistoBins       = hexitechIniFile->getDouble("Processing/BinWidthNM");
            // Update gui spectrum components' units to read "nm^-1"
            calibrationComboBoxChanged("Momentum");
        }
        else
        {
            // Only Energy Calibration
            iHistoStartVal   = hexitechIniFile->getInt("Processing/StartKEV");
            iHistoEndVal     = hexitechIniFile->getInt("Processing/EndKEV");
            dHistoBins       = hexitechIniFile->getDouble("Processing/BinWidthKEV");
            // Update gui spectrum components' units to read "KeV"
            calibrationComboBoxChanged("Energy");
        }
    }
    else
    {
        // Calibration disabled, use ADU units
        iHistoStartVal   = hexitechIniFile->getInt("Processing/StartADU");
        iHistoEndVal     = hexitechIniFile->getInt("Processing/EndADU");
        dHistoBins       = hexitechIniFile->getDouble("Processing/BinWidthADU");
        // Update gui spectrum components' units to read "ADU"
        calibrationComboBoxChanged("None");
    }

    // global threshold always in ADU
    dGlobalThreshold = hexitechIniFile->getDouble("Processing/ThresholdADU");

    // Update gui spectrum components with processing.any file's values
    ui->startBinLineEdit->setText( QString::number(iHistoStartVal));
    ui->endBinLineEdit->setText(QString::number(iHistoEndVal));
    ui->binWidthLineEdit->setText(QString::number(dHistoBins));
    ui->thresholdLineEdit->setText(QString::number(dGlobalThreshold));

    // Binary Width (dHistoBins) is not the same as Number of Bins (iNumberBins)
    //  Update Number of Bins by calling this function:
    updateNumberOfBins();

    // If Threshold's 0, change it to -1.0;  0 basically means disable
    if (dGlobalThreshold == 0.0)
        dGlobalThreshold = -1.0;

    /// Next Frame Correction = Incomplete Data Correction
    QString fileNextFrameCorrection = hexitechIniFile->getString("Processing/IncompleteDataCorrection");
    QString guiNextFrameCorrection = ui->nextFrameCorrectionComboBox->currentText();
    bool bFileNextFrameEnabled = areStringsEqual(fileNextFrameCorrection, "TRUE");
    bool bGuiNextFrameEnabled = areStringsEqual(guiNextFrameCorrection, "Yes");

    // Check that file entry is valid
    if (sanityCheckQString(trueFalseOptions, fileNextFrameCorrection))
    {
        // Only update GUI if file's setting differs from that of the GUI's setting
        if (bFileNextFrameEnabled != bGuiNextFrameEnabled)
        {
            bEnableIdCorrector = bFileNextFrameEnabled;
            // They differ - Check if file's setting is true or false
            if (bFileNextFrameEnabled)
                ui->nextFrameCorrectionComboBox->setCurrentText("Yes");
            else
                ui->nextFrameCorrectionComboBox->setCurrentText("No");
            if (!bSuppressLoadSettingsInfo)
                handleWriteMessage(QString("Updated NextFrameCorrection to: %1").arg(fileNextFrameCorrection));
        }
    }
    else
        handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("IncompleteDataCorrection", fileNextFrameCorrection));

    QString fileChargeSharingCorrection = hexitechIniFile->getString("Processing/ChargeSharingCorrection");
    // Checked that file entry is valid
    if (sanityCheckQString(chargeSharingCorrectionOptions, fileChargeSharingCorrection))
    {
        bool bFileCsd = false;
        bool bFileCsasp = false;
        // File entry valid; is file entry "None"?
        if ( areStringsEqual(fileChargeSharingCorrection, "None"))
        {
            // File entry is "None"; set Booleans correspondingly
            //  (both initialised to false so need not change them here)

            // If GUI's settings doesn't both match File's settings, update GUI's settings
            if ( (bEnableCsdCorrector != bFileCsd) || (bEnableCsaspCorrector != bFileCsasp))
            {
                bEnableCsdCorrector = false;
                bEnableCsaspCorrector = false;
                ui->chargeSharingCorrectionComboBox->setCurrentText(fileChargeSharingCorrection);
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Disabled Charge Sharing Correction.");
            }
        }
        else if ( areStringsEqual(fileChargeSharingCorrection, "Addition"))
        {
            // File entry is "Addition" - set Booleans correspondingly
            bFileCsd = false;
            bFileCsasp = true;
            // If GUI's settings doesn't both match File's settings, update GUI's settings
            if ( (bEnableCsdCorrector != bFileCsd) || (bEnableCsaspCorrector != bFileCsasp))
            {
                bEnableCsdCorrector = false;
                bEnableCsaspCorrector = true;
                ui->chargeSharingCorrectionComboBox->setCurrentText(fileChargeSharingCorrection);
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Selected Charge Sharing Addition.");
            }
        }
        else if ( areStringsEqual(fileChargeSharingCorrection, "Discrimination"))
        {
            // File entry is "Discrimination" - set Booleans correspondingly
            bFileCsd = true;
            bFileCsasp = false;
            // If GUI's settings doesn't both match File's settings, update GUI's settings
            if ( (bEnableCsdCorrector != bFileCsd) || (bEnableCsaspCorrector != bFileCsasp))
            {
                bEnableCsdCorrector = true;
                bEnableCsaspCorrector = false;
                ui->chargeSharingCorrectionComboBox->setCurrentText(fileChargeSharingCorrection);
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Disabled Charge Sharing Discrimination.");
            }
        }
    }
    else
        handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("ChargeSharingCorrection", fileChargeSharingCorrection));

    QString fileInducedNoiseCorrection = hexitechIniFile->getString("Processing/InducedNoiseCorrection");
    // Check that file entry is valid
    if (sanityCheckQString(trueFalseOptions, fileInducedNoiseCorrection))
    {
        // File entry valid; Is file entry different from GUI's?
        bool bFileInducedNoiseEnabled = areStringsEqual(fileInducedNoiseCorrection, "TRUE");
        if (bEnableInCorrector != bFileInducedNoiseEnabled)
        {
            // File setting differs from GUI - Update GUI
            bEnableInCorrector = bFileInducedNoiseEnabled;
            if (bFileInducedNoiseEnabled)
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Enabled Induced Noise Corrector.");
            else
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Disabled Induced Noise Corrector.");
        }
    }
    else
        handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("InducedNoiseCorrection", fileInducedNoiseCorrection));

    dInducedNoiseThreshold = hexitechIniFile->getFloat("Processing/InducedNoiseFraction");

    QString fileInterpolateCorrection = hexitechIniFile->getString("Processing/InterpolateCorrection");
    //Check that file entry a valid
    if (sanityCheckQString(trueFalseOptions, fileInterpolateCorrection))
    {
        //File entry valid; Is file entry different from GUI's?
        bool bFileInterpolateEnabled = areStringsEqual(fileInterpolateCorrection, "TRUE");
        if (bEnableIpCorrector != bFileInterpolateEnabled)
        {
            // File setting differs from GUI - Update GUI
            bEnableIpCorrector = bFileInterpolateEnabled;
            if (bFileInterpolateEnabled)
            {
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Enabled Interpolate Corrector.");
            else
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Disabled Interpolate Corrector.");
            }
        }
    }
    else
        handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("InterpolateCorrection", fileInterpolateCorrection));

    iInterpolationThreshold = hexitechIniFile->getInt("Processing/InterpolateThreshold");

    QString fileDecodedFilename = hexitechIniFile->getString("Processing/DecodedFrameFilename");
    // Check that fileDecodedFilename isn't empty
    if (!fileDecodedFilename.toStdString().empty())
    {
        // Filename specified; Obtain directory from fileDecodedFilename
        string sDecodedFileDirectory = stripFilenameFromFullPath(fileDecodedFilename.toStdString());
        if (doesFilePathExist(sDecodedFileDirectory) == 0)
        {
            // Directory exists - save this setting in the GUI and HxtProcessing
            sOutputFileNameDecodedFrame = fileDecodedFilename.toStdString();
            HxtProcessor->setOutputFileNameDecodedFrame(sOutputFileNameDecodedFrame);
            ui->hxtFileLineEdit->setText( fileDecodedFilename);
            if (!bSuppressLoadSettingsInfo) handleWriteMessage(QString("Changed DecodedFrameFilename to: \"%1\"").arg(fileDecodedFilename));
        }
        else
            handleWriteError(QString("File entry: %1 contains invalid value: \"%2\".").arg("DecodedFrameFilename", fileDecodedFilename));
    }

//    QString fileHistogramFiles = hexitechIniFile->getString("Processing/DiagnosticHistogramCSVFiles");
//    // Check that file entry is valid
//    if (sanityCheckQString(trueFalseOptions, fileHistogramFiles))
//    {
//        // File entry valid; Is file entry different from GUI's?
//        bool bFileHistogramFilesEnabled = areStringsEqual(fileHistogramFiles, "TRUE");
//        if (bWriteCsvFiles != bFileHistogramFilesEnabled)
//        {
//            // File setting differs from GUI's - Update GUI
//            bWriteCsvFiles = bFileHistogramFilesEnabled;
//            if (bFileHistogramFilesEnabled)
//                if (!bSuppressLoadSettingsInfo)
//                    handleWriteMessage("Enabled CSV Histogram Files.");
//            else
//                if (!bSuppressLoadSettingsInfo)
//                    handleWriteMessage("Disabled CSV Histogram Files.");
//        }
//    }
//    else
//        handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("DiagnosticHistogramCSVFiles", fileHistogramFiles));

    QString fileVector = hexitechIniFile->getString("Processing/VectorIndexing");
    // Check that file entry is valid
    if (sanityCheckQString(trueFalseOptions, fileVector))
    {
        // File entry valid; Is file entry different from GUI's?
        bool bFileVector = areStringsEqual(fileVector, "TRUE");
        if (bEnableVector != bFileVector)
        {
            // File setting differs from GUI - Update GUI
            bEnableVector = bFileVector;
            if (bFileVector)
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Enabled File Vector.");
            else
                if (!bSuppressLoadSettingsInfo)
                    handleWriteMessage("Disabled File Vector");
        }
    }
    else
        handleWriteError(QString("File entry: %1 contains invalid value: \"%2\"").arg("VectorIndexing", fileVector));

    QString qsKeyName = QString("Processing/PixelThresholdFile");
    QString filePixelThresholdFile = hexitechIniFile->getString(qsKeyName);
    sThresholdFileName = validateFileName(&qsKeyName, &filePixelThresholdFile);

    // Configure new settings
    configHexitechSettings();

    /// Grab the DiscWritingInterval variable from ini file
    dDiscWritingInterval = hexitechIniFile->getDouble("Processing/DiscWritingInterval");
    if (dDiscWritingInterval != -1) HxtProcessor->setDiscWritingInterval(dDiscWritingInterval);

    // Check hexitech settings
    switch (HxtProcessor->checkConfigValid())
    {
    case 0:
        // Valid configuration
        break;
    case 1:
        handleWriteError("Cannot combine Charge Sharing Addition with Charge Sharing Discrimination.");
        return;
    case 2:
        handleWriteError("Only one file specified within entry \"CalibrationFile\"; Specify neither or both.");
        return;
    case 3:
        handleWriteError("Cannot combine \"ThresholdADU/ThresholdKeV\" with \"PixelThresholdFile\".");
        return;
    default:
        // Shouldn't be possible..
        handleWriteError("Invalid hexitech configuration - Unknown error [Ask the developer].");
        return;
    }

    if (!bSuppressLoadSettingsInfo)
        handleWriteMessage(QString("Finished loading settings from: %1.").arg(hexitechFilename));
}

bool ProcessingWindow::areStringsEqual(QString s1, QString s2)
{
    /// Compare s1, s2 disregarding case; return true if they match
    bool returnValue = false;
    if ( QString::compare(s1, s2, Qt::CaseInsensitive) == 0)
        returnValue = true;
    return returnValue;
}

bool ProcessingWindow::sanityCheckQString(QStringList aList, QString aString)
{
    /// Iterate over aList list looking for the string aString
    ///     return true if a match found, false if no match
    for (int i = 0; i < aList.size(); ++i)
    {
        if ( QString::compare(aList.at(i), aString, Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

bool ProcessingWindow::fileExists(const char *filename)
{
    /// Check that filename exists
    struct stat buf;
    if (stat(filename, &buf) != -1)
    {
        return true;
    }
    return false;
}

string ProcessingWindow::validateFileName(QString* qsKeyName, QString* qsKeyValue)
{
    /// Check qsKeyValue is a file[path] that exists, Notify user if it doesn't.
    /// If qsKeyValue is empty or doesn't exist, return empty string
    string sFileName = "";
    if (qsKeyValue->length() > 0)
    {
        if (!fileExists(qsKeyValue->toStdString().c_str()))
            handleWriteMessage(QString("Invalid file specified in %1; See entry \"%2\".").arg(hexitechFilename, QString(*qsKeyName)));
        else
            sFileName = qsKeyValue->toStdString();
    }
    return sFileName;
}

void ProcessingWindow::dumpHexitechConfig()
{
    /// Debug function - call it to check hexitech settings in memory
    qDebug() << "Start ADU (iHistoStartVal) " << iHistoStartVal;
    qDebug() << "End ADU   (iHistoEndVal)   " << iHistoEndVal;
    qDebug() << "Bin Width ADU (dHistoBins) " << dHistoBins;
    qDebug() << "Interpolate Threshold      " << iInterpolationThreshold;
    qDebug() << "Induced Noise Fraction:    " << dInducedNoiseThreshold;
    qDebug() << "Global Threshold           " << dGlobalThreshold;
    qDebug() << "Pixel Threshold File:      " << sThresholdFileName.c_str();
    qDebug() << "Decoded Frame Filename:    " << sOutputFileNameDecodedFrame.c_str();
    qDebug() << "CalibrationFile";
    qDebug() << "sGradientsFile:           " << sGradientsFile.c_str();
    qDebug() << "sInterceptsFile:          " << sInterceptsFile.c_str();
    qDebug() << "sMomentumFile:            " << sMomentumFile.c_str();
    qDebug() << "Induced Noise Correction: " << bEnableInCorrector;
    qDebug() << "Calibration:              " << bEnableCabCorrector;
    qDebug() << "Charge Sharing";
    qDebug() << "Addition:                 " << bEnableCsaspCorrector;
    qDebug() << "Discrimination:           " << bEnableCsdCorrector;
    qDebug() << "Incomplete Data:          " << bEnableIdCorrector;
    qDebug() << "Interpolate Correction:   " << bEnableIpCorrector;
//    qDebug() << "Diagnost Histo CSV files: " << bWriteCsvFiles;
    qDebug() << "Vector Indexing:          " << bEnableVector;
}

void ProcessingWindow::saveSettingsButtonPressed()
{
    /// Save settings set from GUI to the opened INI file

    // Check that hexitechFilename exists..
    if (!fileExists(hexitechFilename.toStdString().c_str()))
    {
        handleWriteMessage(QString("Error saving settings; No file named: %1").arg( hexitechFilename));
        return;
    }

    // Prepare to update CalibrationFile entry in hexitecGigE.ini
    updateCalibrationFileEntry(ui->gradientsPathLineEdit->text(), ui->interceptsPathLineEdit->text());

    // Ditto for MomentumFile entry, but more straightforward since it's only one file:
    QString momentumString = QString(sMomentumFile.c_str());
    hexitechIniFile->setParameter("Processing/MomentumFile", QVariant(momentumString));

    hexitechIniFile->writeIniFile();
    handleWriteMessage("Settings Saved.");
}

void ProcessingWindow::initHexitechProcessor()
{
    /// Initialise hexitech settings to sensible values where appropriate
    iDebugLevel                  = 0;
    iHistoStartVal               = 0;       // Assume ADU default values
    iHistoEndVal                 = 8000;    // Assume ADU default values
    dHistoBins                   = 10.0;    // Assume ADU default values
    iInterpolationThreshold      = 0;
    dInducedNoiseThreshold       = 0.0;
    dGlobalThreshold             = -1.0;
    dDiscWritingInterval          = 1.0;
    sThresholdFileName           = "";
    sOutputFileNameDecodedFrame  = "pixelHisto.hxt";
    sGradientsFile               = "";
    sInterceptsFile              = "";
    sMomentumFile                = "";
    bEnableInCorrector           = false;
    bEnableCabCorrector          = false;
    bEnableCsaspCorrector        = false;
    bEnableCsdCorrector          = true;
    bEnableIdCorrector           = true;
    bEnableIpCorrector           = false;
    bEnableMomCorrector          = false;
    bEnableDbPxlsCorrector       = false;
//    bWriteCsvFiles               = false;
    bEnableVector                = false;
    bEnableDebugFrame            = false;
}

void ProcessingWindow::configHexitechSettings()
{
    /// Configure Hexitech with user's settings
    HxtProcessor->setDebugLevel( iDebugLevel);
    HxtProcessor->setHistoStartVal( iHistoStartVal);
    HxtProcessor->setHistoEndVal( iHistoEndVal);
    HxtProcessor->setInterpolationThreshold( iInterpolationThreshold);
    HxtProcessor->setInducedNoiseThreshold( dInducedNoiseThreshold);
    HxtProcessor->setGlobalThreshold( dGlobalThreshold);
    HxtProcessor->setDiscWritingInterval( dDiscWritingInterval);
    HxtProcessor->setThresholdFileName( sThresholdFileName);
    HxtProcessor->setOutputFileNameDecodedFrame( sOutputFileNameDecodedFrame);
    HxtProcessor->setGradientsFile( sGradientsFile);
    HxtProcessor->setInterceptsFile( sInterceptsFile);
    HxtProcessor->setMomentumFile(sMomentumFile);
    HxtProcessor->setEnableInCorrector( bEnableInCorrector);
    HxtProcessor->setEnableCabCorrector( bEnableCabCorrector);
    HxtProcessor->setEnableCsaspCorrector( bEnableCsaspCorrector);
    HxtProcessor->setEnableCsdCorrector( bEnableCsdCorrector);
    HxtProcessor->setEnableIdCorrector( bEnableIdCorrector);
    HxtProcessor->setEnableIpCorrector( bEnableIpCorrector);
    HxtProcessor->setEnableDbPxlsCorrector( bEnableDbPxlsCorrector);
//    HxtProcessor->setWriteCsvFiles( bWriteCsvFiles);
    HxtProcessor->setEnableVector( bEnableVector);
    HxtProcessor->setEnableDebugFrame( bEnableDebugFrame);
}

/// Temporary implementation to prove signalling changed HXT file name working
void ProcessingWindow::handleHxtProcessedFileNameChanged(string hxtFileNameCompleted)
{
    qDebug() << " ProcessingWindow::handleHxtProcessedFileNameChanged() received: " << hxtFileNameCompleted.c_str();
}
