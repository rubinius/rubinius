class Regexp
  def self.__regexp_new__(str, opts)
    Ruby.primitive :regexp_new
  end
  
  def match(str)
    Ruby.primitive :regexp_match
  end
  
  def options
    Ruby.primitive :regexp_options
  end
  
end
