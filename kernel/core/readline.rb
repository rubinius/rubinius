module Readline
  def self.readline(prompt)
    print prompt
    STDIN.read(1024)
  end
  
  def self._terminal_raw
    Ruby.primitive :terminal_raw
  end
  
  def self._terminal_normal
    Ruby.primitive :terminal_normal
  end
end
