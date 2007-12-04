module Readline

  module ASCII
    Home = 1
    ControlC = 3
    ControlD = 4
    End = 5
    Erase = 8
    Tab = 9
    Return = 10
    Escape = 27
    Backspace = 127
  end

  @history = []
  @history_idx = 0

  def self.readline(prompt)
    str = []
    idx = 0

    begin
      @c_erase, @c_kill, @c_quit, @c_intr = _terminal_raw

      print prompt

      while true
        cur = STDIN.read(1)

        if cur.nil? then
          print "\n"
          return nil
        end
        
        code = cur[0]
        code = ASCII::Erase if code == @c_erase

        cur = code.chr
        
        if code == ASCII::Return
          print "\n"
          break
        elsif code == ASCII::ControlD
          print "\n"
          return nil
        elsif code == @c_intr
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
        elsif code == ASCII::Home
          print "\b" * idx
          idx = 0
        elsif code == ASCII::End
          print "\b" * idx, str.join("")
          idx = str.size
        else
          str.insert(idx, cur)
          print "\b" * idx, str.join(""), "\b" * (str.size - idx - 1)
          idx += 1
        end
      end
    ensure
      _terminal_normal
    end

    @history << str
    @history_idx = @history.size

    out = str.join("")
    return out
  end
end
