class EnvironmentVariables
  def env_get(key)
    Ruby.primitive :env_get
  end
  
  private :env_get

  def env_set(key, value)
    Ruby.primitive :env_set
  end
  
  private :env_set
  
  def to_hash
    Ruby.primitive :env_as_hash
  end
  
  private :to_hash
  
end
