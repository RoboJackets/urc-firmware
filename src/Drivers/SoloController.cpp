#include "Drivers/SoloController.hpp"

namespace motors {

void SoloController::begin(long baudRate) {
  begin(SOLOMotorControllers::ControlMode::positionMode, baudRate);
}

void SoloController::begin(SOLOMotorControllers::ControlMode controlMode, long baudRate) {
  // _solouno1.begin(baudRate);
  // Set default configurations 
  _solouno.SetFeedbackControlMode(SOLOMotorControllers::FeedbackControlMode::encoders);
  _solouno.SetControlMode(controlMode);
  _solouno.SetPositionControllerKp(0.0195693);
  _solouno.SetPositionControllerKi(0);
  _solouno.SetMotorDirection(SOLOMotorControllers::Direction::counterclockwise);
  _solouno.ResetPositionToZero();
}

int32_t SoloController::getSpeed(uint8_t address, uint8_t channel, bool &valid) {
  bool success;
  int32_t speed = 0;

  speed = _solouno.GetSpeedFeedback();

  valid = success;

  return speed;
}

void SoloController::setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) {

  valid = _solouno.SetSpeedReference(speed);
}

int32_t SoloController::getRawPosition(uint8_t address, uint8_t channel, bool &valid){

  bool success;
  int32_t position = 0;

  position = _solouno.GetPositionCountsFeedback();

  valid = success;

  return position;
}

void SoloController::setRawPosition(int32_t position, uint8_t address, uint8_t channel, bool &valid) {
  valid = _solouno.SetPositionReference(position);
}

}  // namespace motors