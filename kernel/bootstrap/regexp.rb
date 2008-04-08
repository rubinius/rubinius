class Regexp
  def self.__regexp_new__(str, opts)
    Ruby.primitive :regexp_new
    raise PrimitiveFailure, "primitive failed"
  end

  def search_region(str, start, finish, forward) # equiv to MRI's re_search
    Ruby.primitive :regexp_search_region
    raise PrimitiveFailure, "primitive failed"
  end

  def options
    Ruby.primitive :regexp_options
    raise PrimitiveFailure, "primitive failed"
  end
end
