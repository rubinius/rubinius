#ifndef RBX_DIAGNOSTICS_MEASUREMENT_HPP
#define RBX_DIAGNOSTICS_MEASUREMENT_HPP

#include "diagnostics/diagnostic.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <mutex>
#include <string>

namespace rubinius {
  class CompiledCode;

  namespace diagnostics {
    class Measurement : public Diagnostic {
    public:
      std::string label;

      Measurement(STATE, CompiledCode* code, int ip);

      virtual ~Measurement() { }
    };

    class Bytes : public Measurement {
    public:
      std::atomic<uintptr_t> bytes;

      Bytes(STATE, CompiledCode* code, int ip);

      virtual ~Bytes() { }

      void set(STATE, uintptr_t value) {
        bytes.fetch_add(value);
      }

      virtual void update() {
        document_["value"] = rapidjson::Value().SetInt(bytes.load());
      }
    };

    class Counter : public Measurement {
    public:
      std::atomic<uintptr_t> counter;

      Counter(STATE, CompiledCode* code, int ip);

      virtual ~Counter() { }

      void set(STATE, uintptr_t value) {
        counter.fetch_add(value);
      }

      virtual void update() {
        document_["value"] = rapidjson::Value().SetInt(counter.load());
      }
    };

    class Sum : public Measurement {
    public:
      std::atomic<intptr_t> sum;

      Sum(STATE, CompiledCode* code, int ip);

      virtual ~Sum() { }

      void set(STATE, intptr_t value) {
        sum.fetch_add(value);
      }

      virtual void update() {
        document_["value"] = rapidjson::Value().SetInt(sum.load());
      }
    };

    class TimeStamp : public Measurement {
    public:
      std::recursive_mutex lock;
      std::chrono::time_point<std::chrono::system_clock> time_stamp;

      TimeStamp(STATE, CompiledCode* code, int ip);

      virtual ~TimeStamp() { }

      void set(STATE) {
        std::lock_guard<std::recursive_mutex> guard(lock);

        time_stamp = std::chrono::system_clock::now();
      }

      virtual void update() {
        std::time_t now = std::chrono::system_clock::to_time_t(time_stamp);
        std::string value = std::ctime(&now);

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_["value"] = rapidjson::Value(value.c_str(), value.size(), alloc).Move();
      }
    };

    class Timer : public Measurement {
    public:
      std::recursive_mutex lock;
      std::chrono::time_point<std::chrono::high_resolution_clock> total;
      std::chrono::time_point<std::chrono::high_resolution_clock> begin;

      Timer(STATE, CompiledCode* code, int ip);

      virtual ~Timer() { }

      void start(STATE) {
        lock.lock();

        begin = std::chrono::high_resolution_clock::now();
      }

      void stop(STATE) {
        std::chrono::time_point<std::chrono::high_resolution_clock> end;
        total += end - begin;

        lock.unlock();
      }

      virtual void update() {
        std::string value = std::to_string(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
              total.time_since_epoch()).count());

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_["value"] = rapidjson::Value(value.c_str(), value.size(), alloc).Move();
      }
    };

    class Value : public Measurement {
    public:
      std::mutex lock;
      std::string value;

      Value(STATE, CompiledCode* code, int ip);

      virtual ~Value() { }

      void set(STATE, std::string& value) {
        this->value.assign(value);
      }

      virtual void update() {
        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();

        document_["value"] = rapidjson::Value(value.c_str(), value.size(), alloc).Move();
      }
    };
  }
}

#endif
