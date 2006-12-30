module Readline
  def self.readline(prompt)
    ""
  end
  
  def self._terminal_raw
    Ruby.primitive :terminal_raw
  end
  
  def self._terminal_normal
    Ruby.primitive :terminal_normal
  end
end
