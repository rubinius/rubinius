module Comparable
  
  # Compares two objects based on the receiver's <code><=></code>
  # method, returning true if it returns 0. Also returns true if
  # _obj_ and _other_ are the same object.
  def ==(other)
    return true if equal?(other)

    begin
      unless comp = (self <=> other)
        return false
      end

      return Comparable.compare_int(comp) == 0
    rescue StandardError
      return false
    end
  end

end