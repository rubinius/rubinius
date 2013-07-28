# -*- encoding: us-ascii -*-

class Object
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
    elsif name.empty?
      raise TypeError, "can't dump anonymous module #{self}"
    end

    "c#{ms.serialize_integer(name.length)}#{name}"
  end
end

class Module
  def __marshal__(ms)
    raise TypeError, "can't dump anonymous module #{self}" if name.empty?
    "m#{ms.serialize_integer(name.length)}#{name}"
  end
end

class Float
  def __marshal__(ms)
    str = if nan?
            "nan"
          elsif zero?
            (1.0 / self) < 0 ? '-0' : '0'
          elsif infinite?
            self < 0 ? "-inf" : "inf"
          else
            ("%.*g" % [17, self]) + ms.serialize_mantissa(self)
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

    out << ms.serialize(:bt)
    out << ms.serialize(backtrace)
    out << ms.serialize(:mesg)
    out << ms.serialize(@reason_message)

    out
  end
end

module Marshal
  class State
    def serialize_encoding?(obj)
      false
    end

    def set_instance_variables(obj)
      construct_integer.times do
        ivar = get_symbol
        value = construct
        obj.__instance_variable_set__ prepare_ivar(ivar), value
      end
    end

    def serialize_mantissa(flt)
      str = ""
      flt = Math.modf(Math.ldexp(Math.frexp(flt.abs)[0], 37))[0]
      if flt > 0
        str = "\0" * 32
        i = 0
        while flt > 0
          flt, n = Math.modf(Math.ldexp(flt, 32))
          n = n.to_i
          str.setbyte i += 1, (n >> 24) & 0xff
          str.setbyte i += 1, (n >> 16) & 0xff
          str.setbyte i += 1, (n >> 8) & 0xff
          str.setbyte i += 1, (n & 0xff)
        end
        str.gsub!(/(\000)*\Z/, '')
      end
      Rubinius::Type.binary_string(str)
    end

    def construct_string
      obj = get_byte_sequence
      obj = get_user_class.new obj if @user_class

      store_unique_object obj
    end
  end
end
