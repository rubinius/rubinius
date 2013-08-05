# -*- encoding: us-ascii -*-

module Rubinius
  module Type
    def self.coerce_to_path(obj)
      StringValue(obj)
    end

    def self.coerce_to_symbol(obj)
      if object_kind_of?(obj, Fixnum)
        raise ArgumentError, "Fixnums (#{obj}) cannot be used as symbols"
      end
      obj = obj.to_str if obj.respond_to?(:to_str)

      coerce_to(obj, Symbol, :to_sym)
    end

    def self.ivar_validate(name)
      # adapted from rb_to_id
      case name
      when String
        return name.to_sym if name[0] == ?@
      when Symbol
        return name if name.is_ivar?
      when Fixnum
        raise ArgumentError, "#{name.inspect} is not a symbol"
      else
        name = Rubinius::Type.coerce_to(name, String, :to_str)
        return name.to_sym if name[0] == ?@
      end

      raise NameError, "`#{name}' is not allowed as an instance variable name"
    end

    def self.coerce_to_binding(obj)
      if obj.kind_of? Binding
        binding = obj
      elsif obj.kind_of? Proc
        binding = obj.binding
      elsif obj.respond_to? :to_binding
        binding = obj.to_binding
      else
        binding = obj
      end

      unless binding.kind_of? Binding
        raise ArgumentError, "unknown type of binding"
      end

      binding
    end

    def self.coerce_to_pid(obj)
      unless obj.kind_of? Fixnum
        raise TypeError, "wrong argument type #{obj.class} (expected Fixnum)"
      end

      obj
    end

    def self.object_initialize_dup(obj, copy)
      Rubinius.privately do
        copy.initialize_copy obj
      end
    end

    def self.object_initialize_clone(obj, copy)
      Rubinius.privately do
        copy.initialize_copy obj
      end
    end

    def self.object_respond_to_ary?(obj)
      object_respond_to?(obj, :to_ary)
    end

    def self.binary_string(string)
      string
    end

    def self.external_string(string)
      string
    end

    def self.encode_string(string, enc)
      string
    end

    def self.ascii_compatible_encoding(string)
    end

    def self.compatible_encoding(a, b)
    end
  end
end
