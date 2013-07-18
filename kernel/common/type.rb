# -*- encoding: us-ascii -*-

##
# Namespace for coercion functions between various ruby objects.

module Rubinius
  module Type

    ##
    # Returns an object of given class. If given object already is one, it is
    # returned. Otherwise tries obj.meth and returns the result if it is of the
    # right kind. TypeErrors are raised if the conversion method fails or the
    # conversion result is wrong.
    #
    # Uses Rubinius::Type.object_kind_of to bypass type check overrides.
    #
    # Equivalent to MRI's rb_convert_type().

    def self.coerce_to(obj, cls, meth)
      return obj if object_kind_of?(obj, cls)
      execute_coerce_to(obj, cls, meth)
    end

    def self.execute_coerce_to(obj, cls, meth)
      begin
        ret = obj.__send__(meth)
      rescue Exception => orig
        raise TypeError,
              "Coercion error: #{obj.inspect}.#{meth} => #{cls} failed",
              orig
      end

      return ret if object_kind_of?(ret, cls)

      msg = "Coercion error: obj.#{meth} did NOT return a #{cls} (was #{object_class(ret)})"
      raise TypeError, msg
    end

    ##
    # Same as coerce_to but returns nil if conversion fails.
    # Corresponds to MRI's rb_check_convert_type()
    #
    def self.check_convert_type(obj, cls, meth)
      return obj if object_kind_of?(obj, cls)
      return nil unless obj.respond_to?(meth)
      execute_check_convert_type(obj, cls, meth)
    end

    def self.execute_check_convert_type(obj, cls, meth)
      begin
        ret = obj.__send__(meth)
      rescue Exception
        return nil
      end

      return ret if ret.nil? || object_kind_of?(ret, cls)

      msg = "Coercion error: obj.#{meth} did NOT return a #{cls} (was #{object_class(ret)})"
      raise TypeError, msg
    end

    ##
    # Uses the logic of [Array, Hash, String].try_convert.
    #
    def self.try_convert(obj, cls, meth)
      return obj if object_kind_of?(obj, cls)
      return nil unless obj.respond_to?(meth)
      execute_try_convert(obj, cls, meth)
    end

    def self.execute_try_convert(obj, cls, meth)
      ret = obj.__send__(meth)

      return ret if ret.nil? || object_kind_of?(ret, cls)

      msg = "Coercion error: obj.#{meth} did NOT return a #{cls} (was #{object_class(ret)})"
      raise TypeError, msg
    end

    def self.coerce_to_comparison(a, b)
      unless cmp = (a <=> b)
        raise ArgumentError, "comparison of #{a.inspect} with #{b.inspect} failed"
      end
      cmp
    end

    def self.each_ancestor(mod)
      unless singleton_class_object(mod)
        yield mod
      end

      sup = mod.direct_superclass()
      while sup
        if object_kind_of?(sup, IncludedModule)
          yield sup.module
        elsif object_kind_of?(sup, Class)
          yield sup unless singleton_class_object(sup)
        else
          yield sup
        end
        sup = sup.direct_superclass()
      end
    end

    def self.coerce_to_constant_name(name)
      name = Rubinius::Type.coerce_to_symbol(name)

      unless name.is_constant?
        raise NameError, "wrong constant name #{name}"
      end

      name
    end

    def self.coerce_to_regexp(pattern, quote=false)
      case pattern
      when Regexp
        return pattern
      when String
        # nothing
      else
        pattern = StringValue(pattern)
      end

      pattern = Regexp.quote(pattern) if quote
      Regexp.new(pattern)
    end

    # Taint host if source is tainted.
    def self.infect(host, source)
      Rubinius.primitive :object_infect
      raise PrimitiveFailure, "Object.infect primitive failed"
    end

    def self.check_null_safe(string)
      Rubinius.invoke_primitive(:string_check_null_safe, string)
    end

    def self.const_get(mod, name, inherit = true)
      name = coerce_to_constant_name name

      current, constant = mod, undefined

      while current
        if bucket = current.constant_table.lookup(name)
          constant = bucket.constant
          constant = constant.call(current) if constant.kind_of?(Autoload)
          return constant
        end

        unless inherit
          return mod.const_missing(name)
        end

        current = current.direct_superclass
      end

      if instance_of?(Module)
        if bucket = Object.constant_table.lookup(name)
          constant = bucket.constant
          constant = constant.call(current) if constant.kind_of?(Autoload)
          return constant
        end
      end

      mod.const_missing(name)
    end

    def self.const_exists?(mod, name, inherit = true)
      name = coerce_to_constant_name name

      current = mod

      while current
        if bucket = current.constant_table.lookup(name)
          return !!bucket.constant
        end

        return false unless inherit

        current = current.direct_superclass
      end

      if instance_of?(Module)
        if bucket = Object.constant_table.lookup(name)
          return !!bucket.constant
        end
      end

      false
    end
  end
end
