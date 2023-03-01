#ifndef MANAGERS_HPP
#define MANAGERS_HPP

#include <Arduino.h>
#include "Context.hpp"
#include "Messages.hpp"
#include "Drivers/MotorController.hpp"

namespace manager {

class MotorManager {
public:
  MotorManager(Context &context);
  void update(Context &context);
  void update(Context &context, RequestMessage message);

private:
  class Motor {
  public:
    Motor(){};
    void init(const char *name, motors::MotorController *motorController, uint32_t address, uint32_t channel);
    int32_t getSpeed(bool &valid);
    void setSpeed(int32_t speed, bool &valid);
    int32_t getPosition(bool &valid);
    void setPosition(int32_t speed, bool &valid);

  private:
    const static size_t MAX_NAME_LEN = 30;
    motors::MotorController *_motorController;
    uint32_t _address;
    uint32_t _channel;
    char _name[MAX_NAME_LEN];
  };

  const static size_t NUM_MOTORS = 6;
  Motor _motors[NUM_MOTORS];
};

}  // namespace manager

#endif