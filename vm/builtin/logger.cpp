#include "builtin/class.hpp"
#include "builtin/logger.hpp"
#include "builtin/string.hpp"

#include "util/logger.hpp"

#include "ontology.hpp"

namespace rubinius {
  using namespace utilities;

  void Logger::init(STATE) {
    GO(logger).set(ontology::new_class_under(state, "Logger", G(rubinius)));
    G(logger)->set_object_type(state, LoggerType);
  }

  Logger* Logger::create(STATE) {
    Logger* logger = state->new_object<Logger>(G(logger));
    return logger;
  }

  Logger* Logger::allocate(STATE, Object* self) {
    Logger* logger = create(state);
    logger->klass(state, as<Class>(self));
    return logger;
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
