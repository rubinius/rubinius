module Rubinius
  class Splitter
    def self.split_characters(string, pattern, limit, tail_empty)
      start = 0
      ret = []

      kcode = $KCODE

      begin
        if pattern.options and kc = pattern.kcode
          $KCODE = kc
        end

        if limit
          iterations = limit - 1
          while c = string.find_character(start)
            ret << c
            start += c.size
            iterations -= 1

            break if iterations == 0
          end

          ret << string[start..-1]
        else
          while c = string.find_character(start)
            ret << c
            start += c.size
          end

          # Use #byteslice because it returns the right class and taints
          # automatically. This is just appending a "", which is this
          # strange protocol if a negative limit is passed in
          ret << string.byteslice(0,0) if tail_empty
        end
      ensure
        $KCODE = kcode
      end

      return ret
    end

    def self.valid_encoding?(string)
    end

  end
end
