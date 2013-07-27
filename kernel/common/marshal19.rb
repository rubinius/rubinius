# -*- encoding: us-ascii -*-

class BasicObject
  def __marshal__(ms, strip_ivars = false)
    out = ms.serialize_extended_object self
    out << "o"
    cls = Rubinius::Type.object_class self
    name = Rubinius::Type.module_inspect cls
    out << ms.serialize(name.to_sym)
    out << ms.serialize_instance_variables_suffix(self, true, strip_ivars)
  end
end

class Class
  def __marshal__(ms)
    if Rubinius::Type.singleton_class_object(self)
      raise TypeError, "singleton class can't be dumped"
    elsif name.nil? || name.empty?
      raise TypeError, "can't dump anonymous module #{self}"
    end

    "c#{ms.serialize_integer(name.length)}#{name}"
  end
end

class Module
  def __marshal__(ms)
    raise TypeError, "can't dump anonymous module #{self}" if name.nil? || name.empty?
    "m#{ms.serialize_integer(name.length)}#{name}"
  end
end

class Float
  def __marshal__(ms)
    if nan?
      str = "nan"
    elsif zero?
      str = (1.0 / self) < 0 ? '-0' : '0'
    elsif infinite?
      str = self < 0 ? "-inf" : "inf"
    else
      s, decimal, sign, digits = dtoa

      if decimal < -3 or decimal > digits
        str = s.insert(1, ".") << "e#{decimal - 1}"
      elsif decimal > 0
        str = s[0, decimal]
        digits -= decimal
        str << ".#{s[decimal, digits]}" if digits > 0
      else
        str = "0."
        str << "0" * -decimal if decimal != 0
        str << s[0, digits]
      end
    end

    Rubinius::Type.binary_string("f#{ms.serialize_integer(str.length)}#{str}")
  end
end

class Exception
  def __marshal__(ms)
    out = ms.serialize_extended_object self
    out << "o"
    cls = Rubinius::Type.object_class self
    name = Rubinius::Type.module_inspect cls
    out << ms.serialize(name.to_sym)
    out << ms.serialize_fixnum(2)

    out << ms.serialize(:mesg)
    out << ms.serialize(@reason_message)
    out << ms.serialize(:bt)
    out << ms.serialize(backtrace)

    out
  end
end

module Marshal
  class State
    def serialize_encoding?(obj)
      enc = Rubinius::Type.object_encoding(obj)
      enc && enc != Encoding::BINARY
    end

    def serialize_encoding(obj)
      case enc = Rubinius::Type.object_encoding(obj)
        when Encoding::US_ASCII
          :E.__marshal__(self) + false.__marshal__(self)
        when Encoding::UTF_8
          :E.__marshal__(self) + true.__marshal__(self)
        else
          :encoding.__marshal__(self) + serialize_string(enc.name)
      end
    end

    def set_object_encoding(obj, enc)
      case obj
      when String
        obj.force_encoding enc
      when Regexp
        obj.source.force_encoding enc
      when Symbol
        # TODO
      end
    end

    def set_instance_variables(obj)
      construct_integer.times do
        ivar = get_symbol
        value = construct

        case ivar
        when :E
          if value
            set_object_encoding obj, Encoding::UTF_8
          else
            set_object_encoding obj, Encoding::US_ASCII
          end
          next
        when :encoding
          if enc = Encoding.find(value)
            set_object_encoding obj, enc
            next
          end
        end

        obj.__instance_variable_set__ prepare_ivar(ivar), value
      end
    end

    def construct_string
      obj = get_byte_sequence
      obj = get_user_class.new obj if @user_class

      set_object_encoding(obj, Encoding::ASCII_8BIT)

      store_unique_object obj
    end
  end
end
