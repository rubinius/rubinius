ENV = Object.new

class << ENV
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

    env_delete(key.to_str)
  end

  def to_hash
    Ruby.primitive :env_as_hash
  end

  def to_s
    "ENV"
  end

  private
  def env_get(key)
    Ruby.primitive :env_get
  end

  def env_set(key, value)
    Ruby.primitive :env_set
  end

  def env_delete(key)
    Ruby.primitive :env_delete
  end
end

