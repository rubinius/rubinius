# -*- encoding: us-ascii -*-

##
# Interface to process environment variables.

module Rubinius
  class EnvironmentVariables
    include Enumerable
    include Rubinius::EnvironmentAccess

    def [](key)
      value = getenv(StringValue(key))
      if value
        value = set_encoding value
        value.freeze
      end
      value
    end

    def each_key
      return to_enum(:each_key) unless block_given?

      each { |k, v| yield k }
    end

    def each_value
      return to_enum(:each_value) unless block_given?

      each { |k, v| yield v }
    end

    def each
      return to_enum(:each) unless block_given?

      env = environ()
      ptr_size = FFI.type_size FFI.find_type(:pointer)

      offset = 0
      cur = env + offset

      until cur.read_pointer.null?
        entry = cur.read_pointer.read_string
        key, value = entry.split '=', 2
        value.taint if value
        key.taint if key

        key = set_encoding key
        value = set_encoding value

        yield key, value

        offset += ptr_size
        cur = env + offset
      end

      self
    end

    alias_method :each_pair, :each

    def delete(key)
      existing_value = self[key]
      if existing_value
        self[key] = nil
      elsif block_given?
        yield key
      end
      existing_value
    end

    def delete_if(&block)
      return to_enum(:delete_if) unless block_given?
      reject!(&block)
      self
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
      return to_enum(:reject!) unless block_given?

      # Avoid deleting from environ while iterating because the
      # OS can handle that in a million different bad ways.

      keys = []
      each { |k, v| keys << k if yield(k, v) }
      keys.each { |k| delete k }

      keys.empty? ? nil : self
    end

    def clear
      # Avoid deleting from environ while iterating because the
      # OS can handle that in a million different bad ways.

      keys = []
      each { |k, v| keys << k }
      keys.each { |k| delete k }

      self
    end

    def has_value?(value)
      each { |k, v| return true if v == value }
      return false
    end

    alias_method :value?, :has_value?

    def values_at(*params)
      params.map{ |k| self[k] }
    end

    def index(value)
      each do |k, v|
        return k if v == value
      end
      nil
    end

    def invert
      to_hash.invert
    end

    def key(value)
      index(value)
    end

    def keys
      keys = []
      each { |k, v| keys << k }
      keys
    end

    def values
      vals = []
      each { |k, v| vals << v }
      vals
    end

    def empty?
      each { return false }
      return true
    end

    def length
      sz = 0
      each { |k, v| sz += 1 }
      sz
    end

    alias_method :size, :length

    def rehash
      # No need to do anything, our keys are always strings
    end

    def replace(other)
      clear
      other.each { |k, v| self[k] = v }
    end

    def select(&blk)
      return to_enum unless block_given?
      to_hash.select(&blk)
    end

    def shift
      env = environ()

      offset = 0
      cur = env + offset

      ptr = cur.read_pointer
      return nil unless ptr

      key, value = ptr.read_string.split "=", 2

      return nil unless key

      key.taint if key
      value.taint if value

      delete key

      key = set_encoding key
      value = set_encoding value

      return [key, value]
    end

    def to_a
      ary = []
      each { |k, v| ary << [k, v] }
      ary
    end

    def to_hash
      hsh = {}
      each { |k, v| hsh[k] = v }
      hsh
    end

    def update(other)
      if block_given?
        other.each { |k, v| self[k] = yield(k, self[k], v) }
      else
        other.each { |k, v| self[k] = v }
      end
    end

    # Missing and deprecated: indexes, indices
  end
end
