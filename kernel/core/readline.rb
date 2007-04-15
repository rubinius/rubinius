module Readline

  module ASCII
    Return = 10
    ControlC = 3
    ControlD = 4
    Erase = 8
    Tab = 9
    Backspace = 127
    Escape = 27
  end

  @history = []
  @history_idx = 0
  @raw_mode = false

  # Reset the terminal when we exit.
  at_exit do
    _terminal_normal if @raw_mode
  end

  def self.readline(prompt)
    str = []
    idx = 0

    begin
      print prompt
      unless @raw_mode
        _terminal_raw
        @raw_mode = true
      end
      while true
        cur = STDIN.read(1)

        code = cur[0]
        code = ASCII::Erase if code == ASCII::Backspace

        cur = code.chr

        #puts "Code: #{code}"
        if code == ASCII::Return or code == ASCII::ControlD
          print "\n"
          break
        elsif code == ASCII::ControlC
          puts "", "<Control-C>"
          raise Interrupt, "User requested termination with Control-C"
        elsif code == ASCII::Erase
          if str.size > 0
            print "\b" * idx
            idx -= 1
            str.slice!(idx)
            print str.join(""), " ", "\b" * (str.size + 1 - idx)
          end
        elsif code == ASCII::Escape
          STDIN.read(1) # eat the [
          command = STDIN.read(1)
          input = "\e[" + command

          if command == "A"
            # history up
            if @history_idx > 0
              if @history_idx == @history.size && str.size > 0
                @history << str
              end

              @history_idx -= 1

              print "\b" * idx, " " * str.size, "\b" * str.size
              str = @history[@history_idx].dup
              idx = str.size
              print str.join("")
            end
          elsif command == "B"
            # history down
            if @history_idx < @history.size
              @history_idx += 1

              print "\b" * idx, " " * str.size, "\b" * str.size
              str = (@history[@history_idx] || []).dup
              idx = str.size
              print str.join("")
            end
          elsif command == "C"
            # cursor right
            if idx < str.size
              idx += 1
              print input
            end
          elsif command == "D"
            # cursor left
            if idx > 0
              idx -= 1
              print input
            end
          else
            print input
          end
        else
          str.insert(idx, cur)
          print "\b" * idx, str.join(""), "\b" * (str.size - idx - 1)
          idx += 1
        end
      end
    end

    @history << str
    @history_idx = @history.size

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
