class Exception
  def fill_locations
    Ruby.primitive :exception_fill_locations
    raise PrimitiveFailure, "Unable to fill locations"
  end
end
