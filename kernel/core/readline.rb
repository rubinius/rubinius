module Readline
    
  module ASCII
    Return = 10
    ControlC = 3
    ControlD = 4
    Erase = 8
    Tab = 9
  end

  @raw_mode = false

  # Reset the terminal when we exit.
  at_exit do
    _terminal_normal if @raw_mode
  end
  
  def self.readline(prompt)
    str = []
    begin
      print prompt
      unless @raw_mode
        _terminal_raw
        @raw_mode = true
      end
      while true
        cur = STDIN.read(1)
        code = cur[0]
        if code == ASCII::Return or code == ASCII::ControlD
          print "\n"
          break
        elsif code == ASCII::ControlC
          puts "<Control-C>"
          raise Interrupt, "User requested termination with Control-C"
        elsif code == ASCII::Erase
          if str.size > 0
            print "#{cur} #{cur}"
            str.pop
          end
        else
          print cur
          str << cur
        end
      end
    end
    
    out = str.join("")
    
    return out
  end
  
  def self._terminal_raw
    Ruby.primitive :terminal_raw
  end
  
  def self._terminal_normal
    Ruby.primitive :terminal_normal
  end
end
