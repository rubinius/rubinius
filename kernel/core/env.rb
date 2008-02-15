# depends on: class.rb

##
# Interface to process environment variables.

class EnvironmentVariables
  def [](key)
    env_get(StringValue(key))
  end

  def []=(key, value)
    env_set(StringValue(key), value.nil? ? nil : StringValue(value))
  end

  def delete(key)
    env_set(StringValue(key), nil)
  end
  
  def include?(key)
    !self[key].nil?
  end
  alias_method :has_key?, :include?
  alias_method :key?, :include?

  def to_s
    "ENV"
  end
end
