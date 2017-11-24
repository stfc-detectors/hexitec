#ifndef NEWPORTXPSMOTORCONTROLLER_H
#define NEWPORTXPSMOTORCONTROLLER_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#define TIMEOUT 300
#define SMALL_BUFFER_SIZE 256
#define MAX_NB_SOCKETS  100
#define SIZE_SMALL 1023
#define SIZE_NOMINAL 1023
#define SIZE_BIG 2047
#define SIZE_HUGE 65535
#define SIZE_EXECUTE_METHOD 1023
#define SIZE_NAME 100

#include <QObject>
#include <QMutex>

#if defined(XPS_SHARED_LIBRARY)
#  define XPS_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPS_SHARED_EXPORT Q_DECL_IMPORT
#endif

class XPS_SHARED_EXPORT NewportXpsMotorController : public QObject
{
   Q_OBJECT
public:
   ~NewportXpsMotorController();
   NewportXpsMotorController(char *pIPAddress, int port, double timeout);
   // TO DO : Motor identifier required. Int?
   void configure(int speed);
   int startMotor(char *axis, double desiredPosition);
   void stopMotor(char *axis);
   double getPosition(char *axis);
   int getStatus(char *axis);
   int initialiseMotor(char *axis);
   bool isActive();
   bool isNotInit(int code);
   char *getErrorString(int errorCode);
private:
   int TCP_ConnectToServer(char *Ip_Address, int Ip_Port, double TimeOut);
   void TCP_CloseSocket(int SocketIndex);
   const char *TCP_GetError(int SocketIndex);
   void TCP_SetTimeout(int SocketIndex, double Timeout);
   int FirmwareVersionGet (int SocketIndex, char *Version);
   int GroupInitialize (int SocketIndex, char *GroupName);
   int GroupHomeSearch (int SocketIndex, char *GroupName);
   int GroupStatusGet (int SocketIndex, char *GroupName, int *Status);
   int GroupStatusStringGet (int SocketIndex, int GroupStatusCode, char *GroupStatusString);
   int GroupMoveAbort (int SocketIndex, char *GroupName);
   int GroupMoveAbortFast (int SocketIndex, char *GroupName, int AccelerationMultiplier);
   int GroupMoveAbsolute (int SocketIndex, char *GroupName, int NbElements, double TargetPosition[]);
   int GroupMoveRelative (int SocketIndex, char *GroupName, int NbElements, double TargetDisplacement[]);
   int GroupPositionCurrentGet (int SocketIndex, char *GroupName, int NbElements, double CurrentEncoderPosition[]);
   int ConnectToServer (char *Ip_Address, int Ip_Port, double TimeOut);
   void SetTCPTimeout (int SocketID, double Timeout);
   void SendAndReceive(int socketID, char sSendString[], char sReturnString[], int iReturnStringSize);
   void CloseSocket (int SocketID);
   const char *GetError (int SocketID);
   void strncpyWithEOS(char *szStringOut, const char *szStringIn, int nNumberOfCharToCopy, int nStringOutSize);
   int ErrorStringGet(int SocketIndex, int ErrorCode, char *ErrorString);

   bool active;
   int port;
   double timeout;
   int moveSocketId;
   int statusSocketId;
   int positionSocketId;
   int stopSocketId;
   int messageSocketId;
   double TimeoutSocket[MAX_NB_SOCKETS];
   char buffer[SIZE_SMALL];
   QMutex mutex;
   QList<int> notInitCodes;

signals:
};

#endif // NEWPORTXPSMOTORCONTROLLER_H
