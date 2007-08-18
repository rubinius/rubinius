class EnvironmentVariables
  private
  def env_get(key)
    Ruby.primitive :env_get
  end

  def env_set(key, value)
    Ruby.primitive :env_set
  end
  
  def to_hash
    Ruby.primitive :env_as_hash
  end
  
end