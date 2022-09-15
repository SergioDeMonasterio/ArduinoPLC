#ifndef AIR_CYLINDER_CONTROL_H
#define AIR_CYLINDER_CONTROL_H

#include <Arduino.h>

const int processActivationDelay = 10;

enum CrimperStates
{
  waitOnDetection = 0,
  confirmDetection = 1,
  crimpingProcess = 2,
  removingTube = 3,
  inactive = 4,
};

enum ValveStates
{
  valveOnDelay = 0,
  valveOn = 1,
  valveOff = 2
};

class CrimperCtrlFSM
{
private:
  uint8_t _inPinInductCrimpSensor;
  uint8_t _outPinCrimperValve;
  uint8_t _outPinInsertValve;
  
  unsigned int _detectionConfirmTime;
  unsigned int _insertValveOnDelay;
  unsigned int _insertValveOnTime;
  unsigned int _crimperValveOnDelay;
  unsigned int _crimperValveOnTime;
  unsigned int _offValvesTime;

  unsigned long _lastCrimperStateChangeTime;
  unsigned long _lastInsertValveStateChangeTime;
  unsigned long _lastCrimperValveStateChangeTime;

  unsigned int _crimpingProcessTime;

  CrimperStates _crimperState;
  ValveStates   _insertValveState;
  ValveStates   _crimperValveState;

  void crimperFSM(unsigned long currentTime);
  void insertValveFSM(unsigned long currentTime);
  void crimperValveFSM(unsigned long currentTime);

  unsigned long getTimeInterval(unsigned long lastChangeTime, unsigned long currentTime);
  void changeCrimperState(CrimperStates nextCrimperState, unsigned long currentTime);
  void changeInsertValveState(ValveStates nextInsertValveState, unsigned long currentTime);
  void changeCrimperValveState(ValveStates nextCrimperValveState, unsigned long currentTime);

public:
  CrimperCtrlFSM(    uint8_t inPinInductCrimpSensor,
                     uint8_t outPinCrimperValve,
                     uint8_t outPinInsertValve,
                     unsigned int detectionConfirmTime,
                     unsigned int insertValveOnDelay,
                     unsigned int insertValveOnTime,
                     unsigned int crimperValveOnDelay,
                     unsigned int crimperValveOnTime,
                     unsigned int offValvesTime
                  );
                     
  void start(unsigned long currentTime);
  void stop(unsigned long currentTime);
  void run(unsigned long currentTime);
};

#endif