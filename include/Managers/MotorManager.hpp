#ifndef MANAGERS_HPP
#define MANAGERS_HPP


#include "Context.hpp"
#include "Messages.hpp"
#include "Drivers/MotorController.hpp"
#include <Arduino.h>
#include <vector>
#include <string>
#include <unordered_map>

namespace manager {

class MotorManager {
public:
  MotorManager(Context &context);
  void update(Context &context);
  // void update(Context &context, RequestMessage message);

private:
  class Motor {
  public:
    Motor();
    Motor(std::string name, motors::MotorController *motorController, uint32_t address, uint32_t channel);
    int32_t getSpeed(bool &valid);
    void setSpeed(int32_t speed, bool &valid);
    int32_t getPosition(bool &valid);
    void setPosition(int32_t speed, bool &valid);

  private:
    std::string _name;
    motors::MotorController *_motorController;
    uint32_t _address;
    uint32_t _channel;
  };

  std::unordered_map<std::string, Motor> _motor;
};

}  // namespace manager

#endif