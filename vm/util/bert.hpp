#ifndef RBX_UTIL_BERT
#define RBX_UTIL_BERT

#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef RBX_WINDOWS
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <string.h>

#include <stdio.h>
#include <ostream>
#include <iostream>
#include <vector>

namespace bert {

  enum T2BCodes {
    Small_Integer_Ext = 97,
    Integer_Ext = 98,
    Float_Ext = 99,
    Atom_Ext = 100,
    Reference_Ext = 101,
    Port_Ext = 102,
    Pid_Ext = 103,
    Small_Tuple_Ext = 104,
    Large_Tuple_Ext = 105,
    Nil_Ext = 106,
    String_Ext = 107,
    List_Ext = 108,
    Binary_Ext = 109,
    Small_Big_Ext = 110,
    Large_Big_Ext = 111,
    New_Fun_Ext = 112,
    Export_Ext = 113,
    New_Reference_Ext = 114,
    Small_Atom_Ext = 115,
    Fun_Ext = 117,
    Bit_Binary_Ext = 77,
    New_Float_Ext = 70
  };

  class Term {
  public:
    enum TermType {
      Nil,
      Integer,
      Float,
      Atom,
      String,
      Binary,
      Tuple,
      List,
      Unsupported
    };

  private:

    TermType type_;
    union {
      int integer_;
      double float_;
    };
    char* string_;

  public:
    Term(TermType type)
      : type_(type)
      , integer_(0)
      , string_(0)
    {}

    Term(TermType type, int num)
      : type_(type)
      , integer_(num)
      , string_(0)
    {}

    Term(TermType type, char* str)
      : type_(type)
      , integer_(0)
      , string_(str)
    {}

    ~Term() {
      if(string_) delete[] string_;
    }

    TermType type() const {
      return type_;
    }

    int integer() const {
      return integer_;
    }

    double float_number() const {
      return float_;
    }

    void set_float(double val) {
      float_ = val;
    }

    char* string() {
      return string_;
    }

    void set_string(char* str) {
      string_ = str;
    }

    void print() {
      switch(type_) {
      case Integer:
        std::cout << "<integer: " << integer_ << ">\n";
        break;
      case Float:
        std::cout << "<float: " << float_ << ">\n";
        break;
      case Atom:
        std::cout << "<atom: " << string_ << ">\n";
        break;
      case String:
      case Binary:
        std::cout << "<string: \"" << string_ << "\">\n";
        break;
      case Tuple:
        std::cout << "<tuple: " << integer_ << ">\n";
        break;
      case List:
        std::cout << "<list: " << integer_ << ">\n";
        break;
      case Nil:
        std::cout << "<nil>\n";
        break;
      case Unsupported:
      default:
        std::cout << "<unsupported>\n";
      }
    }

    bool contains_string_p() {
      switch(type_) {
      case Atom:
      case String:
      case Binary:
        return true;
      default:
        return false;
      }
    }

    bool contains_elements_p() {
      switch(type_) {
      case Tuple:
      case List:
        return true;
      default:
        return false;
      }
    }
  };

  template <class Reader>
  class Termizer {
    Reader& reader_;

  public:
    Termizer(Reader& reader)
      : reader_(reader)
    {}

    int from_32(char* buffer) {
      return ntohl(*(uint32_t*)buffer);
    }

    int from_16(char* buffer) {
      return ntohs(*(uint16_t*)buffer);
    }

    bool read_i16(int *out) {
      char buf[2];
      if(!reader_.read(2, buf)) return false;
      *out = from_16(buf);
      return true;
    }

    bool read_i32(int *out) {
      char buf[4];
      if(!reader_.read(4, buf)) return false;
      *out = from_32(buf);
      return true;
    }

    bool read_i8(int *out) {
      char buf;
      if(!reader_.read(1, &buf)) return false;
      *out = (int)buf;
      return true;
    }

    char* read_string8() {
      int len = reader_.read1();
      char* buf = new char[len + 1];
      reader_.read(len, buf);
      buf[len] = 0;
      return buf;
    }

    char* read_string16() {
      int len = read_i16();
      char* buf = new char[len + 1];
      reader_.read(len, buf);
      buf[len] = 0;
      return buf;
    }

    char* read_string32() {
      int len = read_i32();
      char* buf = new char[len + 1];
      reader_.read(len, buf);
      buf[len] = 0;
      return buf;
    }

    Term* next_term() {
      int num = 0;
      char code = 0;

      if(!reader_.read(1, &code)) return NULL;

      switch(code) {
      case Small_Integer_Ext:
        if(!read_i8(&num)) return NULL;
        return new Term(Term::Integer, num);
      case Integer_Ext:
        if(!read_i32(&num)) return NULL;
        return new Term(Term::Integer, num);
      case Float_Ext:
        return new Term(Term::Float);
      case Atom_Ext:
        if(!read_i16(&num)) return NULL;
        return new Term(Term::Atom, num);
      case Small_Tuple_Ext:
        if(!read_i8(&num)) return NULL;
        return new Term(Term::Tuple, num);
      case Large_Tuple_Ext:
        if(!read_i16(&num)) return NULL;
        return new Term(Term::Tuple, num);
      case Nil_Ext:
        return new Term(Term::Nil);
      case String_Ext:
        if(!read_i16(&num)) return NULL;
        return new Term(Term::String, num);
      case List_Ext:
        if(!read_i32(&num)) return NULL;
        return new Term(Term::List, num);
      case Binary_Ext:
        if(!read_i32(&num)) return NULL;
        return new Term(Term::Binary, num);
      case Small_Atom_Ext:
        if(!read_i8(&num)) return NULL;
        return new Term(Term::Atom, num);
      default:
        return new Term(Term::Unsupported);
      }
    }

    bool fill_body(Term* term) {
      if(term->contains_string_p() && term->string() == 0) {
        char* buf = new char[term->integer() + 1];
        if(!reader_.read(term->integer(), buf)) {
          delete[] buf;
          return false;
        }

        buf[term->integer()] = 0;
        term->set_string(buf);
      } else if(term->type() == Term::Float) {
        char buf[32];
        if(!reader_.read(31, buf)) return false;
        buf[31] = 0;

        double val;
        if(sscanf(buf, "%lf", &val) < 0) return false;
        term->set_float(val);
      }

      return true;
    }
  };

  template <class Writer>
  class Binaryizer {
    Writer& writer_;

    void write_i8(int val) {
      char buf = (char)val;
      writer_.write(1, &buf);
    }

    void write_i16(int val) {
      uint16_t out = htons((uint16_t)val);
      writer_.write(2, (char*)&out);
    }

    void write_i32(int val) {
      uint32_t out = htonl((uint32_t)val);
      writer_.write(4, (char*)&out);
    }

    void write_string(Term* term) {
      if(char* str = term->string()) {
        writer_.write(term->integer(), str);
      }
    }

  public:

    Binaryizer(Writer& writer)
      : writer_(writer)
    {}

    void write_tuple(int count) {
      if(count < 256) {
        write_i8(Small_Tuple_Ext);
        write_i8(count);
      } else {
        write_i8(Large_Tuple_Ext);
        write_i16(count);
      }
    }

    void write_atom(const char* str, int size) {
      write_i8(Atom_Ext);
      write_i16(size);
      if(str) writer_.write(size, str);
    }

    void write_string(const char* str, int size) {
      write_i8(String_Ext);
      write_i16(size);
      if(str) writer_.write(size, str);
    }

    void write_binary(const char* str, int size) {
      write_i8(Binary_Ext);
      write_i32(size);
      if(str) writer_.write(size, str);
    }

    void write_integer(int val) {
      if(val < 256 && val >= 0) {
        write_i8(Small_Integer_Ext);
        write_i8(val);
      } else {
        write_i8(Integer_Ext);
        write_i32(val);
      }
    }

    void write_float(double val) {
      char buf[32];
      memset(buf, 0, 32);

      snprintf(buf, 31, "%.20e", val);

      write_i8(Float_Ext);
      writer_.write(31, buf);
    }

    void write_term(Term* term) {
      switch(term->type()) {
      case Term::Integer:
        write_integer(term->integer());
        break;
      case Term::Float:
        write_float(term->float_number());
        break;
      case Term::Atom:
        write_atom(term->string(), term->integer());
        break;
      case Term::String:
        write_string(term->string(), term->integer());
        break;
      case Term::Binary:
        write_binary(term->string(), term->integer());
        break;
      case Term::Tuple:
        write_tuple(term->integer());
        break;
      case Term::List:
        write_i8(List_Ext);
        write_i32(term->integer());
        break;
      default:
      case Term::Nil:
        write_i8(Nil_Ext);
        break;
      }
    }
  };

  class StringReader {
    const char* data_;
    int size_;
    int cursor_;

  public:
    StringReader(const char* data, int size)
      : data_(data)
      , size_(size)
      , cursor_(0)
    {}

    bool read(int count, char* into) {
      if(cursor_ + count > size_) return false;

      memcpy(into, data_ + cursor_, count);
      cursor_ += count;

      return true;
    }
  };

  class StringWriter {
    char* buffer_;
    size_t size_;
    int cursor_;

  public:
    StringWriter(char* buf, size_t size)
      : buffer_(buf)
      , size_(size)
      , cursor_(0)
    {}

    void write(size_t count, const char* data) {
      if(cursor_ + count > size_) return;

      memcpy(buffer_ + cursor_, data, count);
      cursor_ += count;
    }
  };

  class IOWriter {
    int fd_;

  public:
    IOWriter(int fd)
      : fd_(fd)
    {}

    void write(size_t count, const char* data) {
      size_t need = count;
      const char* buf = data;

      while(need > 0) {
        ssize_t bytes = ::write(fd_, (const void*)buf, need);

        if(bytes == -1) {
          if(errno != EAGAIN && errno != EINTR) return;
          bytes = 0;
        }

        need -= bytes;
        buf += bytes;
      }
    }
  };

  class IOReader {
    int fd_;
    bool eof_;

  public:
    IOReader(int fd)
      : fd_(fd)
      , eof_(false)
    {}

    bool eof_p() const {
      return eof_;
    }

    bool read(int count, char* into) {
      size_t need = count;
      char* buf = into;

      while(need > 0) {
        ssize_t bytes = ::read(fd_, (void*)buf, need);

        if(bytes == -1) {
          if(errno == EAGAIN || errno == EINTR) {
            bytes = 0;
          } else {
            return false;
          }
        } if(bytes == 0) {
          eof_ = true;
          return false;
        }

        need -= bytes;
        buf += bytes;
      }

      return true;
    }
  };

  enum Type {
    Integer,
    Float,
    Atom,
    String,
    Binary,
    Tuple,
    List,
    Nil,
    Boolean,
    Dictionary,
    Time,
    Regex,
    Invalid
  };

  class Value;
  typedef std::vector<Value*> ValueList;

  class Value {
    Type type_;
    union {
      Term* term_;
      bool boolean_;
      ValueList* elements_;
      uint64_t time_;
    };

    Value* sub_value_;

  public:
    Value(Type type)
      : type_(type)
      , term_(0)
      , sub_value_(0)
    {}

    Value(Type type, Term* term)
      : type_(type)
      , term_(term)
      , sub_value_(0)
    {}

    Value(Type type, bool boolean)
      : type_(type)
      , boolean_(boolean)
      , sub_value_(0)
    {}

    Value(Type type, ValueList* elements)
      : type_(type)
      , elements_(elements)
      , sub_value_(0)
    {}

    Value(Type type, Value* sub)
      : type_(type)
      , term_(0)
      , sub_value_(sub)
    {}

    Value(Type type, Term* term, Value* sub)
      : type_(type)
      , term_(term)
      , sub_value_(sub)
    {}

    ~Value() {
      if(type_ == Tuple || type_ == List) {
        if(elements_) {
          for(ValueList::iterator i = elements_->begin();
              i != elements_->end();
              ++i) {
            delete *i;
          }
          delete elements_;
        }
      } else {
        if(term_) delete term_;
      }
      if(sub_value_) delete sub_value_;
    }

    Type type() const {
      return type_;
    }

    Term* term() {
      return term_;
    }

    bool integer_p() const {
      return type_ == Integer;
    }

    int integer() const {
      return term_->integer();
    }

    bool float_p() const {
      return type_ == Float;
    }

    double float_number() const {
      return term_->float_number();
    }

    bool string_p() const {
      return type_ == Binary;
    }

    char* string() {
      return term_->string();
    }

    bool boolean() const {
      return boolean_;
    }

    ValueList* elements() {
      return elements_;
    }

    Value* sub_value() {
      return sub_value_;
    }

    void set_time(uint64_t time) {
      time_ = time;
    }

    uint64_t time() const {
      return time_;
    }

    // Access
    bool equal_atom(const char* str) {
      if(type_ != Atom) return false;

      int len = strlen(str);
      if(len != integer()) return false;
      if(memcmp(str, string(), len) != 0) return false;

      return true;
    }

    int total_elements() {
      if(!(type_ == Tuple || type_ == List)) return -1;
      return elements_->size();
    }

    Value* get_element(size_t which) {
      if(!(type_ == Tuple || type_ == List)) return NULL;
      if(!elements_) return NULL;
      return elements_->at(which);
    }

    // Printing
    void print_element_list(std::ostream& stream) {
      int last = elements_->size() - 1;
      int j = 0;

      for(ValueList::iterator i = elements_->begin();
          i != elements_->end();
          ++i, ++j) {
        (*i)->print(stream);
        if(j != last) stream << ", ";
      }

    }

    void print(std::ostream& stream) {
      switch(type_) {
      case Nil:
        stream << "nil";
        break;
      case Integer:
        stream << integer();
      case Float:
        stream << float_number();
        break;
      case Atom:
        stream << string();
        break;
      case String:
        stream << "\"" << string() << "\"";
        break;
      case Binary:
        stream << "<<\"" << string() << "\">>";
        break;
      case Tuple:
        stream << "{";
        print_element_list(stream);
        stream << "}";
        break;
      case List:
        stream << "[";
        print_element_list(stream);
        stream << "]";
        break;
      case Boolean:
        if(boolean()) {
          stream << "{bert, true}";
        } else {
          stream << "{bert, false}";
        }
        break;
      case Dictionary:
        stream << "{bert, dict, ";
        sub_value_->print(stream);
        stream << "}";
        break;
      case Time:
        {
          uint64_t val = time();
          int allsecs = val / 1000000;
          int megasecs = allsecs / 1000000;
          int secs = allsecs % 1000000;
          int millisecs = val % 1000000;

          stream  << "{bert, time, ";
          stream << megasecs << ", " << secs << ", " << millisecs;
          stream << "}";
        }
        break;
      default:
        stream << "{bert, unknown}";
      }
    }
  };

  template <class Reader>
  class Decoder {
    Reader& reader_;
    Termizer<Reader> tzr_;

    bool is_bert(Term* term) {
      if(term->integer() != 4) {
        tzr_.fill_body(term);
        return false;
      }

      char buf[5] = "0000";
      if(!reader_.read(4, buf)) return false;
      if(memcmp(buf, "bert", 4) != 0) {
        char* new_buf = new char[5];
        memcpy(new_buf, buf, 4);
        new_buf[4] = 0;

        term->set_string(new_buf);
        return false;
      }
      return true;
    }

    enum ComplexIdent {
      InvalidIdent,
      NilIdent,
      TrueIdent,
      FalseIdent,
      DictIdent,
      TimeIdent,
      RegexIdent
    };

    ComplexIdent detect_ident(Term* term) {
      if(term->type() != Term::Atom) return InvalidIdent;

      int size = term->integer();
      if(size < 3) return InvalidIdent;

      char str[6];
      if(!reader_.read(size, str)) return InvalidIdent;

      switch(str[0]) {
      case 'n':
        if(size != 3) return InvalidIdent;
        if(str[1] == 'i' && str[2] == 'l') return NilIdent;
        return InvalidIdent;
      case 't':
        if(size != 4) return InvalidIdent;
        switch(str[1]) {
        case 'r':
          if(str[2] == 'u' && str[3] == 'e') return TrueIdent;
        case 'i':
          if(str[2] == 'm' && str[3] == 'e') return TimeIdent;
        }
        return InvalidIdent;
      case 'f':
        if(size != 5) return InvalidIdent;
        if(memcmp(str+1, "alse", 4) == 0) return FalseIdent;
        return InvalidIdent;
      case 'd':
        if(size != 4) return InvalidIdent;
        if(memcmp(str+1, "ict", 3) == 0) return DictIdent;
        return InvalidIdent;
      case 'r':
        if(size != 5) return InvalidIdent;
        if(memcpy(str+1, "egex", 4) == 0) return RegexIdent;
        return InvalidIdent;
      }

      return InvalidIdent;
    }

    Value* new_invalid() {
      return new Value(Invalid);
    }

    Value* new_wrapped_term(Term* term) {
      Type type;
      switch(term->type()) {
      case Term::Integer:
        type = Integer;
        break;
      case Term::Float:
        type = Float;
        break;
      case Term::Atom:
        type = Atom;
        break;
      case Term::String:
        type = String;
        break;
      case Term::Binary:
        type = Binary;
        break;
      default:
        abort();
      }

      return new Value(type, term);
    }

    Value* new_nil() {
      return new Value(Nil);
    }

    Value* new_boolean(bool val) {
      return new Value(Boolean, val);
    }

    Value* new_list(Term* term) {
      ValueList* vect = new ValueList(term->integer());
      for(int i = 0; i < term->integer(); i++) {
        vect->at(i) = next_value();
      }

      // This is always nil if it's a good list, so make sure.
      Term* tail = tzr_.next_term();
      if(tail->type() != Term::Nil) {
        delete vect;
        return new_invalid();
      }

      return new Value(List, vect);
    }

    Value* new_tuple(Term* term) {
      ValueList* vect = new ValueList(term->integer());
      for(int i = 0; i < term->integer(); i++) {
        vect->at(i) = next_value();
      }

      return new Value(Tuple, vect);
    }

    Value* new_tuple(Term* term, Value* first) {
      ValueList* vect = new ValueList(term->integer());
      vect->at(0) = first;

      for(int i = 1; i < term->integer(); i++) {
        vect->at(i) = next_value();
      }

      return new Value(Tuple, vect);
    }

    Value* new_dict() {
      Term* list = tzr_.next_term();
      if(!list->type() == Term::List) {
        delete list;
        return new_invalid();
      }

      return new Value(Dictionary, new_list(list));
    }

    Value* new_time() {
      uint64_t sec = 0;
      Term* num = tzr_.next_term();
      if(!num) {
        return new_invalid();
      } else if(num->type() != Term::Integer) {
        delete num;
        return new_invalid();
      }

      sec = num->integer() * 1000000;

      delete num;

      num = tzr_.next_term();
      if(!num) {
        return new_invalid();
      } else if(num->type() != Term::Integer) {
        delete num;
        return new_invalid();
      }

      sec += num->integer();

      delete num;

      num = tzr_.next_term();
      if(!num) {
        return new_invalid();
      } else if(num->type() != Term::Integer) {
        delete num;
        return new_invalid();
      }

      Value* val = new Value(Time);
      val->set_time((sec * 1000000) + num->integer());

      return val;
    }

    Value* new_regex() {
      Term* pattern = tzr_.next_term();
      if(!pattern) {
        return new_invalid();
      } else if(pattern->type() != Term::Binary) {
        delete pattern;
        return new_invalid();
      }

      Term* options = tzr_.next_term();
      if(!options) {
        delete pattern;
        return new_invalid();
      } else if(options->type() != Term::List) {
        delete pattern;
        delete options;
        return new_invalid();
      }

      return new Value(Regex, pattern, new_list(options));
    }

    Value* new_complex(Term* term) {
      Value* val = 0;
      Term* ident = 0;

      ident = tzr_.next_term();

      if(!ident) return new_invalid();

      switch(detect_ident(ident)) {
      case NilIdent:
        val = new_nil();
        break;
      case TrueIdent:
        val = new_boolean(true);
        break;
      case FalseIdent:
        val = new_boolean(false);
        break;
      case DictIdent:
        if(term->integer() != 3) {
          val = new_invalid();
        } else {
          val = new_dict();
        }
        break;
      case TimeIdent:
        if(term->integer() != 5) {
          val = new_invalid();
        } else {
          val = new_time();
        }
        break;
      case RegexIdent:
        if(term->integer() != 4) {
          val = new_invalid();
        } else {
          val = new_regex();
        }
        break;
      case InvalidIdent:
        val = new_invalid();
        break;
      }

      delete ident;

      return val;
    }

  public:

    Decoder(Reader& reader)
      : reader_(reader)
      , tzr_(reader)
    {}

    int read_version() {
      unsigned char buf;
      if(!reader_.read(1, (char*)&buf)) return 0;
      return (int)buf;
    }

    Value* convert_term(Term* term) {
      Value* val = 0;

      switch(term->type()) {
      case Term::Tuple:
        if(term->integer() >= 1) {
          Term* first = tzr_.next_term();

          if(!first) {
            val = new_invalid();
          } else if(first->type() == Term::Atom) {
            if(is_bert(first)) {
              if(term->integer() == 1) {
                val = new_invalid();
              } else {
                val = new_complex(term);
              }
              delete first;
            } else {
              val = new_tuple(term, convert_term(first));
            }
          } else {
            val = new_tuple(term, convert_term(first));
          }
        } else {
          val = new_tuple(term);
        }

        delete term;
        break;
      case Term::Nil:
      case Term::List:
        val = new_list(term);
        delete term;
        break;

      case Term::Float:
      case Term::Atom:
      case Term::String:
      case Term::Binary:
        if(!tzr_.fill_body(term)) {
          delete term;
          val = new_invalid();
          break;
        }
      default:
        val = new_wrapped_term(term);
        break;
      }

      return val;
    }

    Value* next_value() {
      Term* term = tzr_.next_term();
      if(!term) return new_invalid();

      return convert_term(term);
    }
  };

  template <class Writer>
  class Encoder {
    Writer& writer_;
    Binaryizer<Writer> bzr_;

    void write_with_elements(Value* val) {
      bzr_.write_term(val->term());

      for(ValueList::iterator i = val->elements()->begin();
          i != val->elements()->end();
          ++i) {
        write_value(*i);
      }
    }

    void write_nil() {
      bzr_.write_tuple(2);
      bzr_.write_atom("bert", 4);
      bzr_.write_atom("nil", 3);
    }

    void write_boolean(bool val) {
      bzr_.write_tuple(2);
      bzr_.write_atom("bert", 4);
      if(val) {
        bzr_.write_atom("true", 4);
      } else {
        bzr_.write_atom("false", 5);
      }
    }

    void write_dict(Value* val) {
      bzr_.write_tuple(3);
      bzr_.write_atom("bert", 4);
      bzr_.write_atom("dict", 4);
      write_value(val->sub_value());
    }

    void write_time(Value* val) {
      uint64_t time = val->time();
      int allsecs = time / 1000000;
      int megasecs = allsecs / 1000000;
      int secs = allsecs % 1000000;
      int millisecs = time % 1000000;

      bzr_.write_tuple(5);
      bzr_.write_atom("bert", 4);
      bzr_.write_atom("time", 4);
      bzr_.write_integer(megasecs);
      bzr_.write_integer(secs);
      bzr_.write_integer(millisecs);
    }

    void write_regex(Value* val) {
      bzr_.write_tuple(4);
      bzr_.write_atom("bert", 4);
      bzr_.write_atom("regex", 5);
      bzr_.write_term(val->term());
      write_value(val->sub_value());
    }

  public:
    Encoder(Writer& writer)
      : writer_(writer)
      , bzr_(writer)
    {}

    void write_version() {
      char buf = 131;
      writer_.write(1, &buf);
    }

    void write_atom(const char* data, int size = -1) {
      if(size == -1) size = strlen(data);
      bzr_.write_atom(data, size);
    }

    void write_binary(const char* data, int size = -1) {
      if(size == -1) size = strlen(data);
      bzr_.write_binary(data, size);
    }

    void raw_write(const char* data, int size = -1) {
      if(size == -1) size = strlen(data);
      writer_.write(size, data);
    }

    void write_tuple(int size) {
      bzr_.write_tuple(size);
    }

    void write_integer(int val) {
      bzr_.write_integer(val);
    }

    void write_float(double val) {
      bzr_.write_float(val);
    }

    void write_value(Value* val) {
      switch(val->type()) {
      case Integer:
      case Float:
      case Atom:
      case String:
      case Binary:
        bzr_.write_term(val->term());
        break;
      case List:
      case Tuple:
        write_with_elements(val);
        break;
      case Nil:
        write_nil();
        break;
      case Boolean:
        write_boolean(val->boolean());
        break;
      case Dictionary:
        write_dict(val);
        break;
      case Time:
        write_time(val);
        break;
      case Regex:
        write_regex(val);
        break;
      default:
        write_nil();
        break;
      }
    }
  };
}

#endif
