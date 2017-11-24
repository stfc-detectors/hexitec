#define _AFXDLL

#include "newportxpsmotorcontroller.h"
#include <QDebug>
#include <afxsock.h>		// MFC socket extensions

CAsyncSocket m_sConnectSocket[MAX_NB_SOCKETS];
BOOL UsedSocket[MAX_NB_SOCKETS];
int NbSockets;
bool initialised = FALSE;

void initialise()
{
   if (!initialised)
   {
      for (int i = 0; i < MAX_NB_SOCKETS; i++)
      {
         UsedSocket[i] = FALSE;
      }
      NbSockets = 0;
      initialised = TRUE;
   }
}

NewportXpsMotorController::~NewportXpsMotorController()
{
   TCP_CloseSocket(moveSocketId);
   TCP_CloseSocket(statusSocketId);
   TCP_CloseSocket(positionSocketId);
   TCP_CloseSocket(stopSocketId);
   TCP_CloseSocket(messageSocketId);
}

NewportXpsMotorController::NewportXpsMotorController(char *pIPAddress, int port, double timeout)
{
   active = false;
   this->timeout = timeout;

   initialise();
   notInitCodes << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9
                << 50 << 52 << 60 << 61 << 63 << 66 << 67 << 71 << 72 << 83;
   buffer[0] = '\0';
   moveSocketId = TCP_ConnectToServer(pIPAddress, port, 2.0);
   statusSocketId = TCP_ConnectToServer(pIPAddress, port, 2.0);
   positionSocketId = TCP_ConnectToServer(pIPAddress, port, 2.0);
   stopSocketId = TCP_ConnectToServer(pIPAddress, port, 2.0);
   messageSocketId = TCP_ConnectToServer(pIPAddress, port, 2.0);
   if (moveSocketId != -1 &&
       statusSocketId != -1 &&
       positionSocketId != -1 &&
       stopSocketId != -1 &&
       messageSocketId != -1)
   {
      active = true;
      //FirmwareVersionGet(messageSocketId, buffer);
      //qDebug() << "Firmware version" << buffer;
   }
   else
   {
      qDebug() << "Error connecting to controller";
   }
   TCP_SetTimeout(moveSocketId, timeout);
   TCP_SetTimeout(statusSocketId, timeout);
   TCP_SetTimeout(positionSocketId, timeout);
   TCP_SetTimeout(stopSocketId, timeout);
   TCP_SetTimeout(messageSocketId, timeout);
}

void NewportXpsMotorController::configure(int speed)
{
}

int NewportXpsMotorController::initialiseMotor(char *axis)
{
   int errorCode;
   int groupStatus;

   mutex.lock();
   errorCode = -1;

   errorCode = GroupStatusGet(statusSocketId, axis, &groupStatus);
   if ((errorCode == 0) && isNotInit(groupStatus))
   {
      errorCode = GroupInitialize(moveSocketId, axis);
   }
   errorCode = GroupStatusGet(statusSocketId, axis, &groupStatus);
   if ((errorCode == 0) && (groupStatus == 42))
   {
      errorCode = GroupHomeSearch(moveSocketId, axis);
   }
   mutex.unlock();

   return errorCode;
}

int NewportXpsMotorController::startMotor(char *axis, double desiredPosition)
{
   int errorCode;
   double position[1];

   mutex.lock();
   errorCode = -1;
   position[0] = desiredPosition;
   errorCode = GroupMoveAbsolute(moveSocketId, axis, 1, position);
   if (errorCode == -2)
   {
      errorCode = 0;
   }
   mutex.unlock();

   return errorCode;
}

char *NewportXpsMotorController::getErrorString(int errorCode)
{
   int status;

   status = ErrorStringGet(messageSocketId, errorCode, buffer);

   return buffer;
}

void NewportXpsMotorController::stopMotor(char *axis)
{
   int errorCode;

   errorCode = GroupMoveAbort(stopSocketId, axis);
}

double NewportXpsMotorController::getPosition(char *axis)
{
   int errorCode;
   double doublePosition[1];

   mutex.lock();
   errorCode = -1;
   if ((errorCode = GroupPositionCurrentGet(positionSocketId, axis, 1, doublePosition)) != 0)
   {
      qDebug() <<"GroupPositionCurrentGet returned " << errorCode;
   }
   mutex.unlock();

   return doublePosition[0];
}

int NewportXpsMotorController::getStatus(char *axis)
{
   int errorCode;
   int status;

   mutex.lock();
   errorCode = -1;
   status = -1;
   if ((errorCode = GroupStatusGet(statusSocketId, axis, &status)) != 0)
   {
      qDebug() <<"GroupStatusGet returned " << errorCode;
   }
   mutex.unlock();

   return status;
}

bool NewportXpsMotorController::isActive()
{
   return active;
}

bool NewportXpsMotorController::isNotInit(int code)
{
   return notInitCodes.contains(code);
}

int NewportXpsMotorController::TCP_ConnectToServer(char *Ip_Address, int Ip_Port, double TimeOut)
{
   return (ConnectToServer(Ip_Address, Ip_Port, TimeOut));
}

void NewportXpsMotorController::TCP_CloseSocket(int SocketIndex)
{
   CloseSocket(SocketIndex);
}

const char * NewportXpsMotorController::TCP_GetError(int SocketIndex)
{
   return (GetError(SocketIndex));
}

void NewportXpsMotorController::TCP_SetTimeout(int SocketIndex, double Timeout)
{
   SetTCPTimeout(SocketIndex, Timeout);
}

/***********************************************************************
 * GroupInitialize :  Start the initialization
 *
 *     - Parameters :
 *            int SocketIndex
 *            char *GroupName
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::GroupInitialize (int SocketIndex, char * GroupName)
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));

   /* Convert to string */
   sprintf (ExecuteMethod, "GroupInitialize (%s)", GroupName);

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}

/***********************************************************************
 * GroupHomeSearch :  Start home search sequence
 *
 *     - Parameters :
 *            int SocketIndex
 *            char *GroupName
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::GroupHomeSearch (int SocketIndex, char * GroupName)
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));

   /* Convert to string */
   sprintf (ExecuteMethod, "GroupHomeSearch (%s)", GroupName);

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}

/***********************************************************************
 * GroupStatusGet :  Return group status
 *
 *     - Parameters :
 *            int SocketIndex
 *            char *GroupName
 *            int *Status
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::GroupStatusGet (int SocketIndex, char * GroupName, int * Status)
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));

   /* Convert to string */
   sprintf (ExecuteMethod, "GroupStatusGet (%s,int *)", GroupName);

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (ret == 0)
   {
      char * pt;
      char * ptNext;

      pt = ReturnedValue;
      ptNext = NULL;
      if (pt != NULL) pt = strchr (pt, ',');
      if (pt != NULL) pt++;
      if (pt != NULL) sscanf (pt, "%d", Status);
   }
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}


/***********************************************************************
 * GroupStatusStringGet :  Return the group status string corresponding to the group status code
 *
 *     - Parameters :
 *            int SocketIndex
 *            int GroupStatusCode
 *            char *GroupStatusString
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::GroupStatusStringGet (int SocketIndex, int GroupStatusCode, char * GroupStatusString)
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_NOMINAL+1));

   /* Convert to string */
   sprintf (ExecuteMethod, "GroupStatusStringGet (%d,char *)", GroupStatusCode);

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_NOMINAL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (ret == 0)
   {
      char * pt;
      char * ptNext;

      pt = ReturnedValue;
      ptNext = NULL;
      if (pt != NULL) pt = strchr (pt, ',');
      if (pt != NULL) pt++;
      if (pt != NULL) strcpy (GroupStatusString, pt);
      ptNext = strchr (GroupStatusString, ',');
      if (ptNext != NULL) *ptNext = '\0';
   }
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}


/***********************************************************************
 * GroupMoveAbort :  Abort a move
 *
 *     - Parameters :
 *            int SocketIndex
 *            char *GroupName
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::GroupMoveAbort (int SocketIndex, char * GroupName)
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));

   /* Convert to string */
   sprintf (ExecuteMethod, "GroupMoveAbort (%s)", GroupName);

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}


/***********************************************************************
 * GroupMoveAbortFast :  Abort quickly a move
 *
 *     - Parameters :
 *            int SocketIndex
 *            char *GroupName
 *            int AccelerationMultiplier
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::GroupMoveAbortFast (int SocketIndex, char * GroupName, int AccelerationMultiplier)
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));

   /* Convert to string */
   sprintf (ExecuteMethod, "GroupMoveAbortFast (%s,%d)", GroupName, AccelerationMultiplier);

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}


/***********************************************************************
 * GroupMoveAbsolute :  Do an absolute move
 *
 *     - Parameters :
 *            int SocketIndex
 *            char *GroupName
 *            double TargetPosition
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::GroupMoveAbsolute (int SocketIndex, char * GroupName, int NbElements, double TargetPosition[])
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));
   char temp[SIZE_NOMINAL+1];

   /* Convert to string */
   sprintf (ExecuteMethod, "GroupMoveAbsolute (%s,", GroupName);
   for (int i = 0; i < NbElements; i++)
   {
      sprintf (temp, "%.13g", TargetPosition[i]);
      strncat (ExecuteMethod, temp, SIZE_SMALL);
      if ((i + 1) < NbElements)
      {
         strncat (ExecuteMethod, ",", SIZE_SMALL);
      }
   }
   strcat (ExecuteMethod, ")");

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}


/***********************************************************************
 * GroupMoveRelative :  Do a relative move
 *
 *     - Parameters :
 *            int SocketIndex
 *            char *GroupName
 *            double TargetDisplacement
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::GroupMoveRelative (int SocketIndex, char * GroupName, int NbElements, double TargetDisplacement[])
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));
   char temp[SIZE_NOMINAL+1];

   /* Convert to string */
   sprintf (ExecuteMethod, "GroupMoveRelative (%s,", GroupName);
   for (int i = 0; i < NbElements; i++)
   {
      sprintf (temp, "%.13g", TargetDisplacement[i]);
      strncat (ExecuteMethod, temp, SIZE_SMALL);
      if ((i + 1) < NbElements)
      {
         strncat (ExecuteMethod, ",", SIZE_SMALL);
      }
   }
   strcat (ExecuteMethod, ")");

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}

/***********************************************************************
 * GroupPositionCurrentGet :  Return current positions
 *
 *     - Parameters :
 *            int SocketIndex
 *            char *GroupName
 *            int nbElement
 *            double *CurrentEncoderPosition
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::GroupPositionCurrentGet (int SocketIndex, char * GroupName, int NbElements, double CurrentEncoderPosition[])
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));
   char temp[SIZE_NOMINAL+1];

   /* Convert to string */
   sprintf (ExecuteMethod, "GroupPositionCurrentGet (%s,", GroupName);
   for (int i = 0; i < NbElements; i++)
   {
      sprintf (temp, "double *");
      strncat (ExecuteMethod, temp, SIZE_SMALL);
      if ((i + 1) < NbElements)
      {
         strncat (ExecuteMethod, ",", SIZE_SMALL);
      }
   }
   strcat (ExecuteMethod, ")");

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (ret == 0)
   {
      char * pt;
      char * ptNext;

      pt = ReturnedValue;
      ptNext = NULL;

      for (int i = 0; i < NbElements; i++)
      {
         if (pt != NULL) pt = strchr (pt, ',');
         if (pt != NULL) pt++;
         if (pt != NULL) sscanf (pt, "%lf", &CurrentEncoderPosition[i]);
      }
   }
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}

int NewportXpsMotorController::ConnectToServer(char *Ip_Address, int Ip_Port, double TimeOut)
{
   int flag = 1;
   int socketID = 0;
   DWORD sockPendingFlag = 1;

   if (!AfxSocketInit())
   {
      AfxMessageBox((LPCTSTR) "Fatal Error: MFC Socket initialization failed");
      return -1;
   }
   /* Select a socket number */
   if (NbSockets < MAX_NB_SOCKETS)
   {
      while ((UsedSocket[socketID] == TRUE) && (socketID < MAX_NB_SOCKETS))
         socketID++;

      if (socketID == MAX_NB_SOCKETS)
      {
         return -1;
      }
   }
   else
   {
      return -1;
   }
   UsedSocket[socketID] = TRUE;
   NbSockets++;

   /* Socket creation */
   if ((m_sConnectSocket[socketID].Create() == 0)
       || (m_sConnectSocket[socketID].SetSockOpt(TCP_NODELAY,(char *)&flag,(int)sizeof( flag ),IPPROTO_TCP) == 0))
   {
      UsedSocket[socketID] = FALSE;
      NbSockets--;
      return -1;
   }

   /* Connect */
   if (m_sConnectSocket[socketID].Connect(CA2W(Ip_Address), Ip_Port) == 0)
   {
      int SocketError = m_sConnectSocket[socketID].GetLastError();
      if (SocketError != WSAEWOULDBLOCK)
      {
         UsedSocket[socketID] = FALSE;
         NbSockets--;
         return -1;
      }
   }

   /* Set timeout */
   if (TimeOut > 0)
   {
      if (TimeOut < 1e-3)
         TimeoutSocket[socketID] = 1e-3;
      else
         TimeoutSocket[socketID] = TimeOut;
   }
   else
      TimeoutSocket[socketID]	= TIMEOUT;

   /* Socket array */
   struct fd_set *Sockets = new struct fd_set;
   FD_ZERO(Sockets);
   FD_SET(m_sConnectSocket[socketID], Sockets);

   /* Time structure */
   struct timeval *TimeOutStruct = new struct timeval;
   TimeOutStruct->tv_sec = (long) TimeoutSocket[socketID];
   TimeOutStruct->tv_usec = (long) ((TimeoutSocket[socketID] - (long)TimeoutSocket[socketID])* 1e9);

   /* Checking connection is ok */
   int SelectReturn = select(0, NULL, Sockets, NULL, TimeOutStruct);
   if (SelectReturn == SOCKET_ERROR
       || SelectReturn == 0)
   {
      UsedSocket[socketID] = FALSE;
      NbSockets--;
      return -1;
   }

   /* Force no pending */
   if (m_sConnectSocket[socketID].IOCtl(FIONBIO,&sockPendingFlag) == 0)
   {
      m_sConnectSocket[socketID].Close();
      UsedSocket[socketID] = FALSE;
      NbSockets--;
      return -1;
   }

   /* Delay unless -1 return for the first API */
   Sleep(10);

   /* Return socket ID */
   return socketID;
}

/***********************************************************************
 * FirmwareVersionGet :  Return firmware version
 *
 *     - Parameters :
 *            int SocketIndex
 *            char *Version
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::FirmwareVersionGet (int SocketIndex, char * Version)
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));

   /* Convert to string */
   sprintf (ExecuteMethod, "FirmwareVersionGet (char *)");

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (ret == 0)
   {
      char * pt;
      char * ptNext;

      pt = ReturnedValue;
      ptNext = NULL;
      if (pt != NULL) pt = strchr (pt, ',');
      if (pt != NULL) pt++;
      if (pt != NULL) strcpy (Version, pt);
      ptNext = strchr (Version, ',');
      if (ptNext != NULL) *ptNext = '\0';
   }
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}

void NewportXpsMotorController::SetTCPTimeout(int SocketIndex, double TimeOut)
{
   if ((SocketIndex >= 0) && (SocketIndex < MAX_NB_SOCKETS) && (UsedSocket[SocketIndex] == TRUE))
   {
      if (TimeOut > 0)
      {
         if (TimeOut < 1e-3)
            TimeoutSocket[SocketIndex] = 1e-3;
         else
            TimeoutSocket[SocketIndex] = TimeOut;
      }
   }
}

void NewportXpsMotorController::SendAndReceive(int socketID, char sSendString[], char sReturnString[], int iReturnStringSize)
{
   char    sSocketBuffer[SMALL_BUFFER_SIZE + 1] = {'\0'};
   int     iReceiveByteNumber = 0;
   int     iErrorNo = 0;
   fd_set  readFds;
   int     iSelectStatus;
   double  dTimeout;
   struct timeval cTimeout;
   clock_t start, stop;

   if ((socketID >= 0) && (socketID < MAX_NB_SOCKETS) && (UsedSocket[socketID] == TRUE))
   {
      /* Clear receive buffer */
      do
      {
         iReceiveByteNumber = m_sConnectSocket[socketID].Receive(sSocketBuffer,SMALL_BUFFER_SIZE);
      }
      while (iReceiveByteNumber != SOCKET_ERROR);
      sReturnString[0] = '\0';

      /* Send String to controller and wait for response */
      m_sConnectSocket[socketID].Send(sSendString,strlen(sSendString));

      /* Get reply with timeout */
      dTimeout = TimeoutSocket[socketID];
      do
      {
         /* Get time */
         start = clock();

         /* Check reply */
         iReceiveByteNumber = m_sConnectSocket[socketID].Receive(sSocketBuffer,SMALL_BUFFER_SIZE);
         iErrorNo = GetLastError() & 0xffff;

         /* Wait for reply */
         if ((iReceiveByteNumber == SOCKET_ERROR) && (iErrorNo == WSAEWOULDBLOCK))
         {
            FD_ZERO(&readFds);
            FD_SET(m_sConnectSocket[socketID].m_hSocket, &readFds);
            cTimeout.tv_sec = (long)dTimeout;
            cTimeout.tv_usec = (long)((dTimeout - (long)dTimeout) * 1e6);
            iSelectStatus = select(FD_SETSIZE, (fd_set *)&readFds, (fd_set *) NULL, (fd_set *) NULL, &cTimeout);
            if ((iSelectStatus > 0) && (FD_ISSET(m_sConnectSocket[socketID].m_hSocket, &readFds)))
               iReceiveByteNumber = m_sConnectSocket[socketID].Receive(sSocketBuffer,SMALL_BUFFER_SIZE);
            else
            {
               iErrorNo = GetLastError() & 0xffff;
               sprintf(sSocketBuffer,"-2,%s,EndOfAPI",sSendString);
               strncpyWithEOS(sReturnString, sSocketBuffer, strlen(sSocketBuffer), iReturnStringSize);
               iReceiveByteNumber = SOCKET_ERROR;
            }
         }

         /* Concatenation */
         if ((iReceiveByteNumber >= 0) && (iReceiveByteNumber <= SMALL_BUFFER_SIZE))
         {
            sSocketBuffer[iReceiveByteNumber] = '\0';
            strncat(sReturnString, sSocketBuffer, iReturnStringSize - strlen(sReturnString) - 1);
         }

         /* Calculate new timeout */
         stop = clock();
         dTimeout = dTimeout - (double)(stop - start) / CLOCKS_PER_SEC;
         if (dTimeout < 1e-3)
            dTimeout = 1e-3;
      }
      while ((iReceiveByteNumber != SOCKET_ERROR) && (strstr(sReturnString, "EndOfAPI") == NULL));
   }
   else
      sReturnString[0] = '\0';

   return;
}

void NewportXpsMotorController::CloseSocket(int socketID)
{
   if ((socketID >= 0) && (socketID < MAX_NB_SOCKETS))
   {
      if (UsedSocket[socketID] == TRUE)
      {
         m_sConnectSocket[socketID].Close();
         UsedSocket[socketID] = FALSE;
         TimeoutSocket[socketID] = TIMEOUT;
         NbSockets--;
      }
   }
}

const char *NewportXpsMotorController::GetError(int socketID)
{
   if ((socketID >= 0) && (socketID < MAX_NB_SOCKETS))
   {
      int error = m_sConnectSocket[socketID].GetLastError();
      switch (error)
      {
      case WSANOTINITIALISED:
         return ("A successful AfxSocketInit must occur before using this API.");
      case WSAENETDOWN:
         return("The Windows Sockets implementation detected that the network subsystem failed.");
      case WSAEADDRINUSE:
         return("The specified address is already in use.");
      case WSAEINPROGRESS:
         return("A blocking Windows Sockets call is in progress.");
      case WSAEADDRNOTAVAIL:
         return("The specified address is not available from the local machine.");
      case WSAEAFNOSUPPORT:
         return("Addresses in the specified family cannot be used with this socket.");
      case WSAECONNREFUSED:
         return("The attempt to connect was rejected.");
      case WSAEDESTADDRREQ:
         return("A destination address is required.");
      case WSAEFAULT:
         return("The nSockAddrLen argument is incorrect.");
      case WSAEINVAL:
         return("Invalid host address.");
      case WSAEISCONN:
         return("The socket is already connected.");
      case WSAEMFILE:
         return("No more file descriptors are available.");
      case WSAENETUNREACH:
         return("The network cannot be reached from this host at this time.");
      case WSAENOBUFS:
         return("No buffer space is available. The socket cannot be connected.");
      case WSAENOTSOCK:
         return("The descriptor is not a socket.");
      case WSAETIMEDOUT:
         return("Attempt to connect timed out without establishing a connection.");
      case WSAEWOULDBLOCK:
         return("The socket is marked as nonblocking and the connection cannot be completed immediately.");
      case WSAEPROTONOSUPPORT:
         return("The specified port is not supported.");
      case WSAEPROTOTYPE:
         return("The specified port is the wrong type for this socket.");
      case WSAESOCKTNOSUPPORT:
         return("The specified socket type is not supported in this address family.");
      }
      return("");
   }
   else
      return("");
}

void NewportXpsMotorController::strncpyWithEOS(char * szStringOut, const char * szStringIn, int nNumberOfCharToCopy, int nStringOutSize)
{
   if (nNumberOfCharToCopy < nStringOutSize)
   {
      strncpy (szStringOut, szStringIn, nNumberOfCharToCopy);
      szStringOut[nNumberOfCharToCopy] = '\0';
   }
   else
   {
      strncpy (szStringOut, szStringIn, nStringOutSize - 1);
      szStringOut[nStringOutSize - 1] = '\0';
   }
}

/***********************************************************************
 * ErrorStringGet :  Return the error string corresponding to the error code
 *
 *     - Parameters :
 *            int SocketIndex
 *            int ErrorCode
 *            char *ErrorString
 *     - Return :
 *            int errorCode
 ***********************************************************************/
int NewportXpsMotorController::ErrorStringGet (int SocketIndex, int ErrorCode, char * ErrorString)
{
   int ret = -1;
   char ExecuteMethod[SIZE_EXECUTE_METHOD+1];
   char *ReturnedValue = (char *) malloc (sizeof(char) * (SIZE_SMALL+1));

   /* Convert to string */
   sprintf (ExecuteMethod, "ErrorStringGet (%d,char *)", ErrorCode);

   /* Send this string and wait return function from controller */
   /* return function : ==0 -> OK ; < 0 -> NOK */
   SendAndReceive (SocketIndex, ExecuteMethod, ReturnedValue, SIZE_SMALL);
   if (strlen (ReturnedValue) > 0)
      sscanf (ReturnedValue, "%i", &ret);

   /* Get the returned values in the out parameters */
   if (ret == 0)
   {
      char * pt;
      char * ptNext;

      pt = ReturnedValue;
      ptNext = NULL;
      if (pt != NULL) pt = strchr (pt, ',');
      if (pt != NULL) pt++;
      if (pt != NULL) strcpy (ErrorString, pt);
      ptNext = strchr (ErrorString, ',');
      if (ptNext != NULL) *ptNext = '\0';
   }
   if (NULL != ReturnedValue)
      free (ReturnedValue);

   return (ret);
}
