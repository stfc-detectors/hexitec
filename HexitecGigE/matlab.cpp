#include "matlab.h"
#include "mex.h"

#include <iostream>
#include <Windows.h>
using namespace std;
matlab *matlab::matlabInstance = 0;

matlab *matlab::instance()
{
   if (matlabInstance == 0)
   {
      matlabInstance = new matlab();
   }

   return matlabInstance;
}

matlab::matlab(QObject *parent) :
   QThread(parent)
{
   startScriptFlag = false;
   QSettings settings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   if (settings.contains("MATLAB"))
   {
      if (settings.value("MATLAB").toString() == "On")
      {
         activeMatlab = true;
      }
      else
      {
         activeMatlab = false;
         emit writeWarning("Matlab is disabled in this session");
      }
   }

   if (activeMatlab)
   {
      emit writeMessage("Starting MATLAB...");
      if (open(NULL))
      {
         if (setVisible())
         {
            connect(this, SIGNAL(checkRequired()), this, SLOT(checkConnection()));
            //cout << "Main thread = " << QThread::currentThreadId() << endl;
            mainThreadFlag = false;
            start();
         }
      }
      else
      {
         emit writeError("Failed to start MATLAB");
      }
   }
}

matlab::~matlab()
{
   if (engine != NULL)
      close();
}

void matlab::run()
{
   while (true)
   {
      QThread::sleep(1);
      //cout << "Sub thread " << QThread::currentThreadId() << endl;
      if (!mainThreadFlag)
      {
         emit checkRequired();
      }
      if (startScriptFlag)
      {
         startScriptFlag = false;
         emit scriptDone(evalString(script));
      }
   }
}

void matlab::checkConnection()
{
   if (activeMatlab)
   {
      if (engEvalString(engine, "hostid") != 0)
      {
         activeMatlab = false;
         emit writeError("Lost MATLAB connection!");
      }
      emit matlabStatus(activeMatlab);
   }
}

bool matlab::open(const char *startcmd)
{
   bool success = false;

   engine = engOpen(startcmd);

   if (engine != NULL)
   {
      success = true;
      setOutputBuffer(engine);
   }

   return success;
}

bool matlab::close()
{
   bool success = false;

   if (engClose(engine) == 0)
   {
      engine=NULL;
      success = true;
   }

   return success;
}

bool matlab::setOutputBuffer(Engine *engine)
{
   bool success = false;

   // We follow the matlab example program engwindemo.c exactly by having buffer of (BUFFER_SIZE + 1) chars and
   // initializing it by setting the LAST character to '\0' and passing only BUFFER_SIZE to engOutputBuffer.
   buffer[BUFFER_SIZE] = '\0';
   if (engOutputBuffer(engine, buffer, BUFFER_SIZE) == 0)
   {
      success = true;
   }

   return success;
}

bool matlab::setVisible(bool visibility)
{
   bool success = false;

   if (engSetVisible(engine, visibility) == 0)
   {
      success = true;
   }

   return success;
}

bool matlab::evalString(QString string)
{
   bool success = false;
   int status;

   if (activeMatlab)
   {
       Sleep(2000);
      // Temporary fix - open a new connection to the engine in case we are in a separate thread.
      Engine *localEngine = engOpen(NULL);
      setOutputBuffer(localEngine);

      status = engEvalString(localEngine, string.toLatin1());
      //cout << "Status " << status << endl;
      if (status == 0)
      {
         success = true;
         emit updateOutput(getBuffer());
      }
      else
      {
         emit writeError("Error evaluating MATLAB string " + string.toLatin1());
      }
      engClose(localEngine);
   }

   return success;
}

QString matlab::getBuffer()
{
   return QString::fromLatin1(buffer);
}

void matlab::setScript(QString script)
{
   this->script = script;
}

void matlab::startScript()
{
   startScriptFlag = true;
}

int matlab::sendVectorToMatlab(QVector <double> &vector, QString varName)
{
   int status = SUCCESS, k= 0;
   if (activeMatlab)
   {
      mainThreadFlag = true;
      // Temporary fix - open a new connection to the engine in case we are in a separate thread.
      Engine *localEngine = engOpen(NULL);
      mwSize *dims;
      mxArray *T = NULL;
      dims = (mwSize *) mxMalloc (1 * sizeof(mwSize));
      dims[0] = vector.size();
      emit writeMessage("Size of data to write : [" + QString::number(vector.size()) +"]");
      T =  mxCreateNumericArray((int) 1, dims, mxDOUBLE_CLASS, mxREAL);
      if (T == NULL)
      {
         emit writeError("Could not create mxArray containing vector");
         status = CREATE_ERR;
      }
      else
      {
         for (int j = 0 ; j < vector.size() ;  ++j)
         {
            ((double*)mxGetPr(T))[k] = vector[j];
            k++;
         }

         int engineStatus = engPutVariable(localEngine, varName.toLatin1(), T);

         // Check that the engPutVariable actually worked
         if (engineStatus == 0)
         {
            emit writeMessage("variable '" + varName + "' written to Matlab\n");
         }
         else
         {
            emit writeMessage("FAILED to write variable '" + varName + "' to Matlab\n");
         }
      }
      // Close the local engine
      engClose(localEngine);
      mainThreadFlag = false;
   }

   return status;
}

int matlab::sendArrayToMatlab(int *dimensions, double *data, QString varName)
{
   int status = SUCCESS;
   if (activeMatlab)
   {
      mainThreadFlag = true;
      // Temporary fix - open a new connection to the engine in case we are in a separate thread.
      Engine *localEngine = engOpen(NULL);
      mwSize *dims;
      mxArray *T = NULL;
      dims = (mwSize *) mxMalloc (3 * sizeof(mwSize));
      dims[0] = dimensions[0];
      dims[1] = dimensions[1];
      dims[2] = dimensions[2];
      emit writeMessage("Size of data to write : [" + QString::number(dims[0]) + "," + QString::number(dims[1]) + ","+ QString::number(dims[2])+"]");
      T =  mxCreateNumericArray((int) 3, dims, mxDOUBLE_CLASS, mxREAL);

      if (T == NULL)
      {
         emit writeError("Could not create mxArray containing slice");
         status = CREATE_ERR;
      }
      else
      {

         for (int i = 0; i < dims[0] * dims[1] * dims[2]; i++)
         {
            ((double*)mxGetPr(T))[i] = data[i];
         }

         int engineStatus = engPutVariable(localEngine, varName.toLatin1(), T);

         // Check that the engPutVariable actually worked
         if (engineStatus == 0)
         {
            emit writeMessage("variable '" + varName + "' written to Matlab\n");
         }
         else
         {
            emit writeMessage("FAILED to write variable '" + varName + "' to Matlab\n");
         }
      }
      // Close the local engine
      engClose(localEngine);
      mainThreadFlag = false;
   }

   return status;
}

int matlab::getVectorFromMatlab(QVector <double> &vector, QString varName)
{
   int status = SUCCESS, k = 0;

   mxArray *result = NULL;
   if (activeMatlab)
   {
      mainThreadFlag = true;
      // Temporary fix - open a new connection to the engine in case we are in a separate thread.
      Engine *localEngine = engOpen(NULL);
      result = engGetVariable(localEngine, varName.toLatin1());
      if (result == NULL)
      {
         emit writeError("Cannot not retrieve variable " + varName);
         status = RETRIEVE_ERR;
      }
      else
      {
         if (mxGetNumberOfDimensions(result) != 2)
         {
            emit writeError("Dimension mis-match");
            status = DIMENSION_ERR;
         }
         else
         {
            mwSize const * dims ;
            dims = mxGetDimensions(result);
            if (dims[0] > 1 && dims[1] > 1)
            {
               emit writeWarning("variable '" + varName + "'is not a vector");
               status = DIMENSION_ERR;
               return status;
            }
            int dimensionToUse;
            if (dims[0] > 1)
               dimensionToUse = (int) dims[0];
            if (dims[1] > 1)
               dimensionToUse = (int) dims[0];

            emit writeMessage("Size of data read: ["+ QString::number(dims[0]) + "," + QString::number(dims[1])+ "]");
            vector.resize(0);
            for (int j = 0 ; j < dimensionToUse ;  ++j)
            {
               vector.push_back(((double*)mxGetPr(result))[k]) ;
               k++;
            }
            emit writeMessage("variable '" + varName + "' read from Matlab\n");
         }
      }
      // Close the local engine
      engClose(localEngine);
      mainThreadFlag = false;
   }

   return status;
}

double *matlab::createTestMask()
{
   int width = 43;
   int height = 43;
   double *testMask = (double *)malloc(sizeof(double) * width * height);
   double *ptr = testMask;
   int i, j;

   for(i = 0; i < width ; i++)
   {
      for(j = 0; j < height; j++)
      {
         if (i > 19 && i < 23)
         {
            *ptr = 0.0;
         }
         else if (j > 19 && j < 23)
         {
            *ptr = 2.0;
         }
         else
         {
            *ptr = 1.0;
         }
         ptr++;
      }
   }
   return testMask;

}

/* This will eventually get the Matlab variable 'name' and put it into a C-style array. At the moment it only
  exists to test the Slice.veil/mask function so it returns a test mask whatever the input.
  */
double *matlab::getArray(QString varName)
{
   double *array = NULL;

   if (varName.compare("testMask") == 0)
   {
      array = createTestMask();
   }
   else
   {
      mxArray *result;
      Engine *localEngine = engOpen(NULL);
      result = engGetVariable(localEngine, varName.toLatin1());
      if (result != NULL)
      {
         array = (double*)mxGetPr(result);
      }
   }

   return array;
}

int matlab::getSliceFromMatlab(Slice *slice, QString varName)
{
   int status = SUCCESS;
   mxArray *result = NULL;

   if (activeMatlab)
   {
      mainThreadFlag = true;
      // Temporary fix - open a new connection to the engine in case we are in a separate thread.
      Engine *localEngine = engOpen(NULL);
      result = engGetVariable(localEngine, varName.toLatin1());
      if (result == NULL)
      {
         emit writeError("Cannot retrieve variable " + varName);
         status = RETRIEVE_ERR;
      }
      else
      {
         if (mxGetNumberOfDimensions(result) != 3)
         {
            emit writeError("Dimension mis-match");
            status = DIMENSION_ERR;
         }
         else
         {
            mwSize const * dims ;
            dims = mxGetDimensions(result);
            emit writeMessage("Size of data read: ["+ QString::number(dims[0]) + "," + QString::number(dims[1])+ "," + QString::number(dims[2])+"]");
            slice->resize((int) dims[2], (int) dims[1]);
            slice->setVoxelDataLen((int) dims[0]);
            slice->setData((double*)mxGetPr(result));
            emit writeMessage("variable '" + varName + "' read from Matlab\n");
         }
      }
      // Close the local engine
      engClose(localEngine);
      mainThreadFlag = false;
   }

   return status;
}

QStringList matlab::listVariables()
{
   QStringList vars = QStringList();
   // Method evalString is partly duplicated here for two reasons. Firstly it outputs its result by emitting
   // updateOutput and we don't want that here. Secondly, and more importantly, we want to use the buffer before
   // engClose writes a '\0' to its first character.
   if (activeMatlab)
   {
      Engine *localEngine = engOpen(NULL);
      setOutputBuffer(localEngine);

      if (engEvalString(localEngine, "whos") == 0)
      {
         QString nb = getBuffer();
         if (nb.compare("") != 0)
         {
            vars = nb.split('\n');
            // The first two lines of 'whos' output are the header and a blank
            // and the last two lines are blank. Unless vars is empty.
            if (vars.size() > 4)
            {
               vars.removeFirst();
               vars.removeFirst();
               vars.removeLast();
               vars.removeLast();
            }
         }
      }
      else
      {
         emit writeError("Error evaluating MATLAB string whos");
      }
      engClose(localEngine);
   }

   return vars;
}
