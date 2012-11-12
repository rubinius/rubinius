# -*- encoding: us-ascii -*-

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
  end
end
