module Readline

  # MTP
  def self._terminal_raw
    Ruby.primitive :terminal_raw
    raise PrimitiveFailure, "primitive failed"
  end

  # MTP
  def self._terminal_normal
    Ruby.primitive :terminal_normal
    raise PrimitiveFailure, "primitive failed"
  end

end
