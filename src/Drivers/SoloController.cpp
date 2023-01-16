#include "Drivers/SoloController.hpp"

namespace motors {

void SoloController::begin(long speed) {
//   _solouno.begin(speed);
  _solouno.SetFeedbackControlMode(SOLOMotorControllers::FeedbackControlMode::encoders);
  _solouno.SetControlMode(SOLOMotorControllers::ControlMode::positionMode);
  _solouno.SetPositionControllerKp(0.0195693);
  _solouno.SetPositionControllerKi(0);
  _solouno.ResetPositionToZero();
}

int32_t SoloController::getSpeed(uint8_t address, uint8_t channel, bool &valid) {
  bool success;
  int32_t speed = 0;

  if (channel == 0) {
    speed = _solouno.GetSpeedFeedback();
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
        valid = _solouno.SetSpeedReference(speed);
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