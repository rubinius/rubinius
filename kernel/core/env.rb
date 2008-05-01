# depends on: class.rb

##
# Interface to process environment variables.

class EnvironmentVariables

  include Enumerable

  def [](key)
    env_get(StringValue(key))
  end

  def []=(key, value)
    env_set(StringValue(key), value.nil? ? nil : StringValue(value))
  end
  alias_method :store, :[]=

  def delete(key)
    env_set(StringValue(key), nil)
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

  def each
    to_hash.each { |k, v| yield k, v }
    self
  end
  alias_method :each_pair, :each

  def reject!
    rejected = false
    each do |k, v|
      if yield k, v
        delete k
        rejected = true
      end
    end
    rejected ? self : nil
  end

  def reject(&block)
    to_hash.reject(&block)
  end

  def clear
    reject! { true }
  end

  def delete_if(&block)
    reject!(&block)
    self
  end

  def each_key
    each { |k, v| yield k }
  end

  def each_value
    each { |k, v| yield v }
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

  public :to_hash

  def update(other, &block)
    if block_given?
      other.each { |k, v| self[k] = yield k, self[k], v }
    else
      other.each { |k, v| self[k] = v }
    end
  end

  # Missing and deprecated: index, indexes, indices
end
