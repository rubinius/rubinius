class Regexp
  def self.__regexp_new__(str, opts)
    Ruby.primitive :regexp_new
    raise PrimitiveFailure, "primitive failed"
  end
  
  def match_region(str, start, finish, forward)
    Ruby.primitive :regexp_match_region
    raise PrimitiveFailure, "primitive failed"
  end
  
  def options
    Ruby.primitive :regexp_options
    raise PrimitiveFailure, "primitive failed"
  end
  
end
