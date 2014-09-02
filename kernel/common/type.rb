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
        if object_respond_to?(obj, :inspect)
          raise TypeError,
              "Coercion error: #{obj.inspect}.#{meth} => #{cls} failed",
              orig
        else
          raise TypeError,
              "Coercion error: #{meth} => #{cls} failed",
              orig
        end
      end

      return ret if object_kind_of?(ret, cls)

      coerce_to_type_error obj, ret, meth, cls
    end

    def self.coerce_to_type_error(original, converted, method, klass)
      oc = object_class original
      cc = object_class converted
      msg = "failed to convert #{oc} to #{klass}: #{oc}\##{method} returned #{cc}"
      raise TypeError, msg
    end

    ##
    # BasicObject responds to only a select few of methods such as `==` and
    # `__send__`. It however does not respond to other methods such as `class`,
    # `respond_to?` and other common methods.
    #
    # This method provides a guard for code that can not handle BasicObject
    # instances. The guard itself is quite simple: it simply raises for
    # BasicObject instances.
    #
    # The first argument is the object to check, the second argument the class
    # name that the object would normally be converted into.
    #
    # Basic usage example:
    #
    #     object = BasicObject.new
    #
    #     # TypeError: "BasicObject can't be coerced into String".
    #     Rubinius::Type.basic_object_guard(object, String)
    #
    def self.coerce_basic_object_guard(other, klass)
      if object_class(other) == BasicObject
        raise TypeError, "can't convert BasicObject into #{klass}"
      end
    end

    ##
    # Same as coerce_to but returns nil if conversion fails.
    # Corresponds to MRI's rb_check_convert_type()
    #
    def self.check_convert_type(obj, cls, meth)
      return obj if object_kind_of?(obj, cls)
      return nil unless object_respond_to?(obj, meth, true)
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
      sup = mod
      while sup
        if object_kind_of?(sup, IncludedModule)
          yield sup.module
        else
          yield sup if sup == sup.origin
        end
        sup = sup.direct_superclass
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

    def self.const_lookup(mod, name, inherit, resolve)
      parts = name.split '::'

      if name.start_with? '::'
        mod = Object
        parts.shift
      end

      parts.each do |part|
        mod = const_get mod, part, inherit, resolve
      end

      mod
    end

    def self.const_get(mod, name, inherit=true, resolve=true)
      unless object_kind_of? name, Symbol
        name = StringValue(name)
        if name.index '::' and name.size > 2
          return const_lookup mod, name, inherit, resolve
        end
      end

      name = coerce_to_constant_name name
      current, constant = mod, undefined

      while current and object_kind_of? current, Module
        if bucket = current.constant_table.lookup(name)
          constant = bucket.constant
          if resolve and object_kind_of? constant, Autoload
            constant = constant.call(current)
          end

          return constant
        end

        unless inherit
          return resolve ? mod.const_missing(name) : undefined
        end

        current = current.direct_superclass
      end

      if object_instance_of? mod, Module
        if bucket = Object.constant_table.lookup(name)
          constant = bucket.constant
          if resolve and object_kind_of? constant, Autoload
            constant = constant.call(current)
          end

          return constant
        end
      end

      resolve ? mod.const_missing(name) : undefined
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

    def self.include_modules_from(included_module, klass)
      insert_at = klass
      changed = false
      constants_changed = false

      mod = included_module

      while mod

        # Check for a cyclic include
        if mod == klass
          raise ArgumentError, "cyclic include detected"
        end

        if mod == mod.origin
          # Try and detect check_mod in klass's heirarchy, and where.
          #
          # I (emp) tried to use Module#< here, but we need to also know
          # where in the heirarchy the module is to change the insertion point.
          # Since Module#< doesn't report that, we're going to just search directly.
          #
          superclass_seen = false
          add = true

          k = klass.direct_superclass
          while k
            if k.kind_of? Rubinius::IncludedModule
              # Oh, we found it.
              if k == mod
                # ok, if we're still within the directly included modules
                # of klass, then put future things after mod, not at the
                # beginning.
                insert_at = k unless superclass_seen
                add = false
                break
              end
            else
              superclass_seen = true
            end

            k = k.direct_superclass
          end

          if add
            if mod.kind_of? Rubinius::IncludedModule
              original_mod = mod.module
            else
              original_mod = mod
            end

            im = Rubinius::IncludedModule.new(original_mod).attach_to insert_at
            insert_at = im

            changed = true
          end

          constants_changed ||= mod.constants.any?
        end

        mod = mod.direct_superclass
      end

      if changed
        included_module.method_table.each do |meth, obj, vis|
          Rubinius::VM.reset_method_cache klass, meth
        end
      end

      if constants_changed
        Rubinius.inc_global_serial
      end
    end

    def self.object_respond_to__dump?(obj)
      object_respond_to? obj, :_dump
    end

    def self.object_respond_to_marshal_dump?(obj)
      object_respond_to? obj, :marshal_dump
    end

    def self.object_respond_to_marshal_load?(obj)
      object_respond_to? obj, :marshal_load
    end

    def self.coerce_to_encoding(obj)
      case obj
      when Encoding
        return obj
      when String
        return Encoding.find obj
      else
        return Encoding.find StringValue(obj)
      end
    end

    def self.try_convert_to_encoding(obj)
      case obj
      when Encoding
        return obj
      when String
        str = obj
      else
        str = StringValue obj
      end

      key = str.upcase.to_sym

      pair = Encoding::EncodingMap[key]
      if pair
        index = pair.last
        return index && Encoding::EncodingList[index]
      end

      return undefined
    end

    def self.coerce_to_path(obj)
      if object_kind_of?(obj, String)
        obj
      else
        if object_respond_to? obj, :to_path
          obj = obj.to_path
        end

        StringValue(obj)
      end
    end

    def self.coerce_to_symbol(obj)
      return obj if object_kind_of? obj, Symbol

      obj = obj.to_str if obj.respond_to?(:to_str)
      coerce_to(obj, Symbol, :to_sym)
    end

    def self.coerce_to_reflection_name(obj)
      return obj if object_kind_of? obj, Symbol
      return obj if object_kind_of? obj, String
      coerce_to obj, String, :to_str
    end

    def self.ivar_validate(name)
      case name
      when Symbol
        # do nothing
      when String
        name = name.to_sym
      else
        name = Rubinius::Type.coerce_to(name, String, :to_str)
        name = name.to_sym
      end

      unless name.is_ivar?
        raise NameError, "`#{name}' is not allowed as an instance variable name"
      end

      name
    end

    def self.coerce_to_binding(obj)
      if obj.kind_of? Binding
        binding = obj
      elsif obj.kind_of? Proc
        raise TypeError, 'wrong argument type Proc (expected Binding)'
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

    # Equivalent of num_exact in MRI's time.c; used by Time methods.
    def self.coerce_to_exact_num(obj)
      if obj.kind_of?(Integer)
        obj
      elsif obj.kind_of?(String)
        raise TypeError, "can't convert #{obj} into an exact number"
      elsif obj.nil?
        raise TypeError, "can't convert nil into an exact number"
      else
        check_convert_type(obj, Rational, :to_r) || coerce_to(obj, Integer, :to_int)
      end
    end

    def self.coerce_to_utc_offset(offset)
      offset = String.try_convert(offset) || offset

      if offset.kind_of?(String)
        unless offset.encoding.ascii_compatible? && offset.match(/\A(\+|-)(\d\d):(\d\d)\z/)
          raise ArgumentError, '"+HH:MM" or "-HH:MM" expected for utc_offset'
        end

        offset = $2.to_i*60*60 + $3.to_i*60
        offset = -offset if $1.ord == 45
      else
        offset = Rubinius::Type.coerce_to_exact_num(offset)
      end

      if offset <= -86400 || offset >= 86400
        raise ArgumentError, "utc_offset out of range"
      end

      offset
    end

    def self.coerce_to_pid(obj)
      Rubinius::Type.coerce_to obj, Integer, :to_int
    end

    def self.coerce_to_bitwise_operand(obj)
      if object_kind_of? obj, Float
        raise TypeError, "can't convert Float into Integer for bitwise arithmetic"
      end
      coerce_to obj, Integer, :to_int
    end

    def self.object_initialize_dup(obj, copy)
      Rubinius.privately do
        copy.initialize_dup obj
      end
    end

    def self.object_initialize_clone(obj, copy)
      Rubinius.privately do
        copy.initialize_clone obj
      end
    end

    def self.object_respond_to_ary?(obj)
      object_respond_to?(obj, :to_ary, true)
    end

    def self.binary_string(string)
      string.force_encoding Encoding::BINARY
    end

    def self.external_string(string)
      string.force_encoding Encoding.default_external
    end

    def self.encode_string(string, enc)
      string.force_encoding enc
    end

    def self.ascii_compatible_encoding(string)
      compatible_encoding string, Encoding::US_ASCII
    end

    def self.compatible_encoding(a, b)
      enc = Encoding.compatible? a, b

      unless enc
        enc_a = object_encoding a
        enc_b = object_encoding b
        message = "undefined conversion "
        message << "for '#{a.inspect}' " if object_kind_of?(a, String)
        message << "from #{enc_a} to #{enc_b}"

        raise Encoding::CompatibilityError, message
      end

      enc
    end

    def self.encoding_order(a, b)
      index_a = Encoding::EncodingMap[a.name.upcase][1]
      index_b = Encoding::EncodingMap[b.name.upcase][1]
      index_a <=> index_b
    end

    def self.object_respond_to__dump?(obj)
      object_respond_to? obj, :_dump, true
    end

    def self.object_respond_to_marshal_dump?(obj)
      object_respond_to? obj, :marshal_dump, true
    end

    def self.object_respond_to_marshal_load?(obj)
      object_respond_to? obj, :marshal_load, true
    end

    def self.bindable_method?(source, destination)
      unless object_kind_of? source, Module or
             object_kind_of? source, destination
        if singleton_class_object source
          raise TypeError, "illegal attempt to rebind a singleton method to another object"
        end

        raise TypeError, "Must be bound to an object of kind #{source}"
      end
    end
  end
end
