#include "memory.hpp"

#include "class/class.hpp"
#include "class/logger.hpp"
#include "class/string.hpp"

#include "logger.hpp"

namespace rubinius {
  using namespace utilities;

  void Logger::bootstrap(STATE) {
    GO(logger).set(state->memory()->new_class<Class, Logger>(
          state, G(rubinius), "Logger"));
  }

  Logger* Logger::create(STATE) {
    return Logger::allocate(state, G(logger));
  }

  Logger* Logger::allocate(STATE, Object* self) {
    return state->memory()->new_object<Logger>(state, as<Class>(self));
  }

  Object* Logger::write(STATE, String* message) {
    logger::write(format()->c_str(state), message->c_str(state));
    return cNil;
  }

  Object* Logger::fatal(STATE, String* message) {
    logger::fatal(format()->c_str(state), message->c_str(state));
    return cNil;
  }

  Object* Logger::error(STATE, String* message) {
    logger::error(format()->c_str(state), message->c_str(state));
    return cNil;
  }

  Object* Logger::warn(STATE, String* message) {
    logger::warn(format()->c_str(state), message->c_str(state));
    return cNil;
  }

  Object* Logger::info(STATE, String* message) {
    logger::info(format()->c_str(state), message->c_str(state));
    return cNil;
  }

  Object* Logger::debug(STATE, String* message) {
    logger::debug(format()->c_str(state), message->c_str(state));
    return cNil;
  }
}
