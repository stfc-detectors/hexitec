#ifndef MOTOR_H
#define MOTOR_H

#include <QApplication>
#include <QObject>
#include <QVariant>
#include <QThread>
#include "motorlimits.h"
#include "motorstatus.h"
#include "reservable.h"
#include "applicationoutput.h"

using namespace std;

enum MoveType {ABSOLUTE_MODE, RELATIVE_MODE};

class Motor : public QThread, public Reservable
{
   Q_OBJECT
public:
   explicit Motor(QString name, QObject *parent = 0);
   Q_INVOKABLE virtual void initialise();
   Q_INVOKABLE virtual void stop() = 0;
   Q_INVOKABLE void begin();
   Q_INVOKABLE virtual void reset();
   Q_INVOKABLE void setDesiredMove(int move, int mode);
   Q_INVOKABLE int getPosition();
   Q_INVOKABLE QString getStatusString();
   Q_INVOKABLE bool isMoving();
   virtual void run() = 0;
   virtual void configure() = 0;
   void setMoveInput(int moveInput, bool doEmit);
   int getMoveInput();
   void setMode(int mode, bool doEmit);
   int getMode();
   void setSelected(bool selected);
   bool isSelected();
   bool willGo(bool doLock);
   bool willInitialise(bool doLock);
   bool release(QString releaser);

protected:
   QMutex mutex;
   int position;
   int desiredPosition;
   virtual void begin(int desiredPosition) = 0;
   void setPosition(int newPosition);
   void setMotorStatus(MotorStatus motorStatus);
   virtual void _initialise();
   MotorStatus getMotorStatus();
   bool drivingIntoLimit();
   bool initialiseInProgress;
   int gearing;
   QString name;

private:
   MotorLimits limits;
   int moveInput;
   int selected;
   int mode;
   MotorStatus motorStatus;
   void calculateDesiredPosition();
   QString motorReleaser;
   bool moveInProgress;

signals:
   void positionChanged(Motor *motor, const QVariant & value);
   void statusChanged(Motor *motor, MotorStatus motorStatus);
   void moveInputChanged(Motor *motor, const QVariant & value);
   void modeChanged(Motor *motor, const QVariant & value);
   void writeError(QString message);
   void writeMessage(QString message);

private slots:
   void trySelfRelease();
};

#endif // MOTOR_H
