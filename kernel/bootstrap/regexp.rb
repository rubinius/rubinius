class Regexp
  def self.__regexp_new__(str, opts)
    Ruby.primitive :regexp_new
  end
  
  def match_region(str, start, finish, forward)
    Ruby.primitive :regexp_match_region
  end
  
  def options
    Ruby.primitive :regexp_options
  end
  
end
