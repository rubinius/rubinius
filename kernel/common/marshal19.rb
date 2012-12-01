# -*- encoding: us-ascii -*-

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
  end
end
