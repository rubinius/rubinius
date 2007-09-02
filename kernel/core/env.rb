class EnvironmentVariables
  def [](key)
    unless key.respond_to?(:to_str)
      raise TypeError
    end

    env_get(key.to_str)
  end

  def []=(key, value)
    unless key.respond_to?(:to_str)
      raise TypeError
    end

    if value.nil?
      delete(key.to_str)
    elsif value.respond_to?(:to_str)
      env_set(key.to_str, value.to_str)
    else
      raise TypeError
    end
  end

  def delete(key)
    unless key.respond_to?(:to_str)
      raise TypeError
    end

    env_set(key.to_str, nil)
  end

  def to_s
    "ENV"
  end
  
end

