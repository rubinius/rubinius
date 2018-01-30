#ifndef RBX_DIAGNOSTICS_MEASUREMENT_HPP
#define RBX_DIAGNOSTICS_MEASUREMENT_HPP

#include <atomic>

namespace rubinius {
  namespace diagnostics {
    class Measurement {
    public:
      typedef void (Update)(STATE, Measurement*, intptr_t value);
      typedef void (Scan)(STATE, Measurement*);
      typedef std::string (Report)(STATE, Measurement*);

      std::atomic<intptr_t> _value_;
      std::string _label_;

      attr_field(update, Update*);
      attr_field(scan, Scan*);
      attr_field(report, Report*);

      Measurement()
        : _value_(0)
        , _label_()
        , _update_(nullptr)
        , _scan_(nullptr)
        , _report_(nullptr)
      {
      }

      static Measurement* create_counter(STATE);

      static void update_counter(STATE, Measurement* m, intptr_t value) {
        m->_value_.fetch_add(value);
      }

      static std::string report_counter(STATE, Measurement* m) {
        return std::to_string(m->value());
      }

      intptr_t value() {
        return _value_;
      }

      void label(const std::string& file, const std::string& name, int ip) {
        _label_.assign(file).append(":").append(name).append(":").append(std::to_string(ip));
      }

      void update(STATE, intptr_t value) {
        if(_update_) _update_(state, this, value);
      }

      void scan(STATE) {
        if(_scan_) _scan_(state, this);
      }

      std::string report(STATE) {
        if(_report_) {
          return _report_(state, this);
        } else {
          return std::string("");
        }
      }
    };
  }
}

#endif
