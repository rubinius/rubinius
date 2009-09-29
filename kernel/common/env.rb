##
# Interface to process environment variables.

module Rubinius
  class EnvironmentVariables

    include Enumerable

    def [](key)
      value = EnvironmentVariables.getenv StringValue(key)
      value.taint unless value.nil?
      value
    end

    def []=(key, value)
      key = StringValue(key)
      if value.nil?
        EnvironmentVariables.unsetenv(key)
      else
        EnvironmentVariables.setenv key, StringValue(value), 1
      end
      value
    end
    alias_method :store, :[]=

    def each_key
      return to_enum :each_key unless block_given?

      each { |k, v| yield k }
    end

    def each_value
      return to_enum :each_value unless block_given?

      each { |k, v| yield v }
    end

    def each
      return to_enum :each unless block_given?

      to_hash.each { |k, v| yield k, v }
      self
    end
    alias_method :each_pair, :each

    def delete(key)
      existing_value = self[key]
      self[key] = nil if existing_value
      existing_value
    end

    def delete_if(&block)
      return to_enum :delete_it unless block_given?
      reject!(&block)
      self
    end

    def fetch(*params)
      if params.size < 1
        raise ArgumentError, "wrong number of arguments (0 for 1)"
      elsif params.size > 2
        raise ArgumentError, "wrong number of arguments (#{params.size} for 2)"
      end
      if block_given? and params.size == 2
        warn "block supersedes default value argument"
      end
      value = self[params[0]]
      return value if value
      return yield(params[0]) if block_given?
      raise IndexError, "key not found" if params.size == 1
      return params[1]
    end

    def include?(key)
      !self[key].nil?
    end
    alias_method :has_key?, :include?
    alias_method :key?, :include?
    # More efficient than using the one from Enumerable
    alias_method :member?, :include?

    def to_s
      "ENV"
    end

    def inspect
      to_hash.inspect
    end

    def reject(&block)
      to_hash.reject(&block)
    end

    def reject!
      return to_enum :reject! unless block_given?
      rejected = false
      each do |k, v|
        if yield k, v
          delete k
          rejected = true
        end
      end
      rejected ? self : nil
    end

    def clear
      reject! { true }
    end

    def has_value?(value)
      to_hash.has_value? value
    end
    alias_method :value?, :has_value?

    def values_at(*params)
      to_hash.values_at(*params)
    end

    def invert
      to_hash.invert
    end

    def keys
      to_hash.keys
    end

    def values
      to_hash.values
    end

    def empty?
      to_hash.empty?
    end

    def length
      to_hash.length
    end
    alias_method :size, :length

    def rehash
      # No need to do anything, our keys are always strings
    end

    def replace(other)
      clear
      other.each { |k, v| self[k] = v }
    end

    def shift
      hash = to_hash
      return nil if hash.empty?
      result = hash.shift
      delete result.first
      result
    end

    def to_a
      to_hash.to_a
    end

    def to_hash
      environ = EnvironmentVariables.environ
      environ.type_size = FFI.type_size(FFI.find_type :pointer)

      i = 0

      hash = {}

      until environ[i].read_pointer.null? do
        entry = environ[i].read_pointer.read_string
        key, value = entry.split '=', 2
        value.taint unless value.nil?
        key.taint unless key.nil?
        hash[key] = value
        i += 1
      end

      hash
    end

    def update(other, &block)
      if block_given?
        other.each { |k, v| self[k] = yield k, self[k], v }
      else
        other.each { |k, v| self[k] = v }
      end
    end

    # Missing and deprecated: index, indexes, indices
  end
end
