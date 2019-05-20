# -*- encoding: us-ascii -*-

module Rubinius
  module Runtime
    def self.matches_when(array, receiver)
      array.each { |x| return true if x === receiver }
      false
    end

    def self.unwrap_block_arg(arg)
      if arg.size == 1
        elem = arg.at(0)
        return elem if elem.kind_of?(Array)
      end

      arg
    end

    def self.find_constant_for_op_asign_or(name, scope)
      name = Rubinius::Type.coerce_to_constant_name name

      current, constant = scope.module, undefined

      while current
        if entry = current.constant_table.lookup(name)
          constant = entry.constant
          constant = constant.call if constant.kind_of?(Autoload)
          return constant
        end

        current = current.direct_superclass
      end

      if instance_of?(Module)
        if entry = Object.constant_table.lookup(name)
          constant = entry.constant
          constant = constant.call if constant.kind_of?(Autoload)
          return constant
        end
      end

      nil
    end

    def self.get_flip_flop(scope, index)
      scope.flip_flops ||= {}
      scope.flip_flops[index]
    end

    def self.set_flip_flop(scope, index, value)
      scope.flip_flops ||= {}
      scope.flip_flops[index] = value
    end

    def self.rbx_marshal_constant
      name
    end

    def self.rbx_marshal_rational(obj)
      Rubinius::Tuple[obj.numerator, obj.denominator]
    end

    def self.rbx_marshal_complex(obj)
      Rubinius::Tuple[obj.real, obj.imaginary]
    end

    def self.get_encoding(name)
      if defined?(Encoding)
        Encoding.find name
      else
        name
      end
    end

    def self.pre_exe
      yield
    end

    def self.splat_hash_value(hash, value)
      value = Rubinius::Type.coerce_to value, Hash, :to_hash
      value.merge hash
    end

    def self.splat_hash_entry(hash, key, value)
      hash[key] = value unless hash.key? key
      hash
    end

    def self.keyword_object?(obj, optional)
      hash = Rubinius::Type.try_convert obj, Hash, :to_hash
      return unless hash

      hs = hash.size
      return [nil, hash] if hs == 0

      kw = {}
      t = Tuple.new hs
      i = 0

      hash.each do |key, value|
        if key.instance_of? Symbol
          kw[key] = value
        else
          return unless optional

          t[i] = key
          i += 1
        end
      end

      if i == 0
        [nil, kw]
      elsif i == hs
        [obj, kw]
      else
        nk = {}
        j = 0

        while j < i
          k = t[j]
          nk[k] = hash[k]
          j += 1
        end

        [nk, kw]
      end
    end

    def self.keywords_missing(hash)
      keywords = CompiledCode.of_sender.keywords

      missing = []
      total = keywords.size
      i = 0

      while i < total
        break unless keywords[i+1]

        key = keywords[i]
        missing << key unless hash.key? key

        i += 2
      end

      msg = "missing keyword#{"s"if missing.size > 1}: #{missing.join(", ")}"
      raise ArgumentError, msg
    end

    def self.keywords_extra(hash, kwrest)
      keywords = CompiledCode.of_sender.keywords

      total = keywords.size
      i = 0

      while i < total
        hash.delete keywords[i]

        i += 2
      end

      return hash if kwrest or hash.size == 0

      msg = "unknown keyword#{"s" if hash.size > 1}: #{hash.keys.join(", ")}"
      raise ArgumentError, msg
    end

    def self.unmarshal_rational(a, b)
      Rational(a, b)
    end

    def self.unmarshal_complex(r, i)
      Complex(r, i)
    end
  end
end
