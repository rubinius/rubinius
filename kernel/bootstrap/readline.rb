module Readline

  # MTP
  def self._terminal_raw
    Ruby.primitive :terminal_raw
  end

  # MTP
  def self._terminal_normal
    Ruby.primitive :terminal_normal
  end

end