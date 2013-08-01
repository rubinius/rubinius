module Rubinius
  class Splitter
    def self.split_characters(string, pattern, limit, tail_empty)
      if limit
        string.chars.take(limit - 1) << string[(limit - 1)..-1]
      else
        ret = string.chars.to_a
        # Use #byteslice because it returns the right class and taints
        # automatically. This is just appending a "", which is this
        # strange protocol if a negative limit is passed in
        ret << string.byteslice(0,0) if tail_empty
        ret
      end
    end

    def self.valid_encoding?(string)
      raise ArgumentError, "invalid byte sequence in #{string.encoding.name}" unless string.valid_encoding?
    end

  end
end
