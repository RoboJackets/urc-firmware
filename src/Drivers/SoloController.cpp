#include "Drivers/SoloController.hpp"

namespace motors {

void SoloController::begin(long speed) {
//   _solouno.begin(speed);
  // Set default configurations 
  _solouno1.SetFeedbackControlMode(SOLOMotorControllers::FeedbackControlMode::encoders);
  _solouno1.SetControlMode(SOLOMotorControllers::ControlMode::positionMode);
  _solouno1.SetPositionControllerKp(0.0195693);
  _solouno1.SetPositionControllerKi(0);
  _solouno1.ResetPositionToZero();

  _solouno2.SetFeedbackControlMode(SOLOMotorControllers::FeedbackControlMode::encoders);
  _solouno2.SetControlMode(SOLOMotorControllers::ControlMode::positionMode);
  _solouno2.SetPositionControllerKp(0.0195693);
  _solouno2.SetPositionControllerKi(0);
  _solouno2.ResetPositionToZero();
}

int32_t SoloController::getSpeed(uint8_t address, uint8_t channel, bool &valid) {
  bool success;
  int32_t speed = 0;

  if (channel == 0) {
    speed = _solouno1.GetSpeedFeedback();
    valid = success;
  } else if (channel == 1) {
    speed = _solouno2.GetSpeedFeedback();
    valid = success;
  } else {
    valid = false;
  }
  
//   if (channel == 0) {
//     speed = _solouno.ReadSpeedM1(address, nullptr, &success);
//     valid = success;
//   } else if (channel == 1) {
//     speed = _solouno.ReadSpeedM2(address, nullptr, &success);
//     valid = success;
//   } else {
//     valid = false;
//   }

  return speed;
}

void SoloController::setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) {

  if (channel == 0) {
    valid = _solouno1.SetSpeedReference(speed);
  } else if (channel == 1) {
    valid = _solouno2.SetSpeedReference(speed);
  } else {
      valid = false;
  }

//   if (channel == 0) {
//     valid = _solouno.SpeedM1(address, speed);
//   } else if (channel == 1) {
//     valid = _solouno.SpeedM2(address, speed);
//   } else {
//     valid = false;
//   }
}

}  // namespace motors