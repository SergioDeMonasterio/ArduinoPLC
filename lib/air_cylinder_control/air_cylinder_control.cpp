#include "air_cylinder_control.h"
#include <Arduino.h>

CrimperCtrlFSM::CrimperCtrlFSM(
    uint8_t inPinInductCrimpSensor,
    uint8_t outPinCrimperValve,
    uint8_t outPinInsertValve,
    unsigned int detectionConfirmTime,
    unsigned int insertValveOnDelay,
    unsigned int insertValveOnTime,
    unsigned int crimperValveOnDelay,
    unsigned int crimperValveOnTime,
    unsigned int offValvesTime )
{
  _inPinInductCrimpSensor = inPinInductCrimpSensor;
  _outPinCrimperValve     = outPinCrimperValve;
  _outPinInsertValve      = outPinInsertValve;

  _detectionConfirmTime   = detectionConfirmTime;
  _insertValveOnDelay     = insertValveOnDelay;
  _insertValveOnTime      = insertValveOnTime;
  _crimperValveOnDelay    = crimperValveOnDelay;
  _crimperValveOnTime     = crimperValveOnTime;
  _offValvesTime          = offValvesTime;

  _crimpingProcessTime = max(insertValveOnDelay+insertValveOnTime, crimperValveOnDelay+crimperValveOnTime) + offValvesTime;

  _crimperState = inactive;
  _insertValveState = valveOff;
  _crimperValveState = valveOff;

  // Console output
  Serial.print("Inductive Sensor Input Pin: "); Serial.println(inPinInductCrimpSensor);
  Serial.print("Insert Valve Output Pin: "); Serial.println(outPinInsertValve);
  Serial.print("Crimper Valve Output Pin: "); Serial.println(outPinCrimperValve);
  Serial.println("Confirm time interval: " + _detectionConfirmTime);
}

// Public Methods

void CrimperCtrlFSM::start(unsigned long currentTime)
{
  Serial.println("Air cylinder state machine STARTED!");
  changeCrimperState(waitOnDetection, currentTime);
}

void CrimperCtrlFSM::stop(unsigned long currentTime)
{
  Serial.println("Air cylinder state machine STOPPED!");
  changeCrimperState(inactive, currentTime);
}

void CrimperCtrlFSM::run(unsigned long currentTime)
{
  crimperFSM(currentTime);
}

// --------------> Private Methods <-----------------

unsigned long CrimperCtrlFSM::getTimeInterval(unsigned long currentTime, unsigned long lastChangeTime)
{
  unsigned long timeInterval = currentTime - lastChangeTime;
  return timeInterval;
}

 void CrimperCtrlFSM::changeCrimperState(CrimperStates nextCrimperState, unsigned long currentTime){
  const char* crimperStateStr[] = {"waitOnDetection", "confirmDetection", "crimpingProcess","removingTube", "inactive"};
    _crimperState = nextCrimperState;
    _lastCrimperStateChangeTime = currentTime;
    Serial.print("Crimper State: "); Serial.println("Test 1");
 }

 void CrimperCtrlFSM::changeInsertValveState(ValveStates nextInsertValveState, unsigned long currentTime){
  const char* valveStateStr[] = {"valveOnDelay", "valveOn", "valveOff"};
   _insertValveState = nextInsertValveState;
   _lastInsertValveStateChangeTime = currentTime;
   Serial.print("Insert Valve State: "); Serial.println("Test 2");
 }

 void CrimperCtrlFSM::changeCrimperValveState(ValveStates nextCrimperValveState, unsigned long currentTime){
  
   _crimperValveState = nextCrimperValveState;
   _lastCrimperValveStateChangeTime = currentTime;
   const char* valveStateStrs[] = {"valveOnDelay", "valveOn", "valveOff"};
   Serial.print("Crimper Valve State: "); Serial.println("Test 3");
 }

void CrimperCtrlFSM::insertValveFSM(unsigned long currentTime){

  switch (_insertValveState)
  {
    case valveOnDelay:
      if (_insertValveOnDelay < getTimeInterval(currentTime, _lastInsertValveStateChangeTime)) 
         changeInsertValveState(valveOn,currentTime);
      break;
    case valveOn:
      digitalWrite(_outPinInsertValve, HIGH);
      if (_insertValveOnTime < getTimeInterval(currentTime, _lastInsertValveStateChangeTime))
          changeInsertValveState(valveOff,currentTime);
      break;
    case valveOff:
      digitalWrite(_outPinInsertValve, LOW);
      if (_crimperState == crimpingProcess) changeInsertValveState(valveOnDelay,currentTime);
      break;
    default:
      if (_crimperState == crimpingProcess) changeInsertValveState(valveOnDelay,currentTime);
      break;
  }

}
void CrimperCtrlFSM::crimperValveFSM(unsigned long currentTime){
  switch (_crimperValveState)
  {
    case valveOnDelay:
      if (_crimperValveOnDelay < getTimeInterval(currentTime, _lastCrimperValveStateChangeTime)) 
         changeCrimperValveState(valveOn,currentTime);
      break;
    case valveOn:
      digitalWrite(_outPinCrimperValve, HIGH);
      if (_crimperValveOnTime < getTimeInterval(currentTime, _lastCrimperValveStateChangeTime))
          changeCrimperValveState(valveOff,currentTime);
      break;
    case valveOff:
      digitalWrite(_outPinCrimperValve, LOW);
      if (_crimperState == crimpingProcess) changeCrimperValveState(valveOnDelay,currentTime);
      break;
    default:
      if (_crimperState == crimpingProcess) changeCrimperValveState(valveOnDelay,currentTime);
      break;
  }
}

void CrimperCtrlFSM::crimperFSM(unsigned long currentTime)
{
  switch (_crimperState)
  {
  case waitOnDetection:
    if (digitalRead(_inPinInductCrimpSensor) == LOW) changeCrimperState(confirmDetection, currentTime);
    break;
  case confirmDetection:
    if (digitalRead(_inPinInductCrimpSensor) == LOW)
    {
      if (_detectionConfirmTime < getTimeInterval(currentTime, _lastCrimperStateChangeTime)) 
        changeCrimperState(crimpingProcess, currentTime);
    }
    else changeCrimperState(waitOnDetection, currentTime);
    break;
  case crimpingProcess:
    if ( processActivationDelay < getTimeInterval(currentTime, _lastCrimperStateChangeTime))
      changeCrimperState(removingTube, currentTime);
    break;
  case removingTube:
    if (_crimpingProcessTime < getTimeInterval(currentTime, _lastCrimperStateChangeTime))
      changeCrimperState(waitOnDetection, currentTime);
    break;
  case inactive:
    changeCrimperState(inactive, currentTime);
    break; // optional

  // you can have any number of case statements.
  default: // Optional
    changeCrimperState(waitOnDetection, currentTime);
  }
}


