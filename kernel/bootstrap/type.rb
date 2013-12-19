# The Type module provides facilities for accessing various "type" related
# data about an object, as well as providing type coercion methods. These
# facilities are independent of the object and thus are more robust in the
# face of ad hoc monkeypatching.
module Rubinius
  module Type
    # Performs a direct kind_of? check on the object bypassing any method
    # overrides.
    def self.object_kind_of?(obj, cls)
      Rubinius.primitive :vm_object_kind_of
      raise PrimitiveFailure, "Rubinius::Type.object_kind_of? primitive failed"
    end

    def self.object_class(obj)
      Rubinius.primitive :vm_object_class
      raise PrimitiveFailure, "Rubinius::Type.object_class primitive failed"
    end

    def self.object_singleton_class(obj)
      Rubinius.primitive :vm_object_singleton_class
      raise TypeError, "no singleton class available for a #{Type.object_class(obj)}"
    end

    def self.singleton_class_object(mod)
      Rubinius.primitive :vm_singleton_class_object
      raise PrimitiveFailure, "Rubinius::Type.singleton_class_object primitive failed"
    end

    def self.object_respond_to?(obj, name, include_private = false)
      Rubinius.invoke_primitive :vm_object_respond_to, obj, name, include_private
    end

    def self.object_equal(a, b)
      Rubinius.primitive :vm_object_equal
      raise PrimitiveFailure, "Rubinius::Type.object_equal primitive failed"
    end

    def self.module_name(mod)
      Rubinius.primitive :vm_get_module_name
      raise PrimitiveFailure, "Rubinius::Type.module_name primitive failed"
    end

    def self.module_inspect(mod)
      sc = singleton_class_object mod

      if sc
        case sc
        when Class, Module
          name = "#<Class:#{module_inspect(sc)}>"
        else
          cls = object_class sc
          name = "#<Class:#<#{module_name(cls)}:0x#{sc.object_id.to_s(16)}>>"
        end
      else
        name = module_name mod
        if !name or name == ""
          name = "#<#{object_class(mod)}:0x#{mod.object_id.to_s(16)}>"
        end
      end

      name
    end

    def self.set_module_name(mod, name, under)
      Rubinius.primitive :vm_set_module_name
      raise PrimitiveFailure, "Rubinius::Type.set_module_name primitive failed"
    end

    def self.coerce_string_to_float(string, strict)
      value = Rubinius.invoke_primitive :string_to_f, string, strict
      raise ArgumentError, "invalid string for Float" if value.nil?
      value
    end

    def self.coerce_to_array(obj)
      return [obj] unless obj

      return obj.to_ary if obj.respond_to?(:to_ary)

      # Just call #to_a, which wraps the reciever in an
      # array if it's not one.
      return Rubinius.privately { obj.to_a }
    end

    def self.coerce_to_float(obj, strict=true, must_be_numeric=true)
      case obj
      when String
        coerce_string_to_float obj, strict
      else
        coerce_object_to_float obj
      end
    end

    def self.coerce_object_to_float(obj)
      case obj
      when Float
        obj
      when nil
        raise TypeError, "can't convert nil into Float"
      else
        coerced_value = coerce_to obj, Float, :to_f
        if coerced_value.nan?
          raise ArgumentError, "invalid value for Float(): #{coerced_value.inspect}"
        end
        coerced_value
      end
    end

    def self.convert_to_names(list)
      list.map { |x| x.to_s }
    end

    def self.convert_to_name(sym)
      sym.to_s
    end
  end
end
