module Rubinius
  class Splitter
    def self.split(string, pattern, limit)
      # Odd edge case
      return [] if string.empty?

      tail_empty = false

      if limit.equal?(undefined)
        limited = false
      else
        limit = Rubinius::Type.coerce_to limit, Fixnum, :to_int

        if limit > 0
          return [string.dup] if limit == 1
          limited = true
        else
          tail_empty = true
          limited = false
        end
      end

      pattern ||= ($; || " ")

      if pattern == ' '
        if limited
          lim = limit
        elsif tail_empty
          lim = -1
        else
          lim = 0
        end

        return Rubinius.invoke_primitive :string_awk_split, string, lim
      elsif pattern.nil?
        pattern = /\s+/
      elsif pattern.kind_of?(Regexp)
        # Pass
      else
        pattern = StringValue(pattern) unless pattern.kind_of?(String)

        if !limited and limit.equal?(undefined)
          if pattern.empty?
            ret = []
            pos = 0

            while pos < string.num_bytes
              ret << string.byteslice(pos, 1)
              pos += 1
            end

            return ret
          else
            return split_on_string(string, pattern)
          end
        end

        pattern = Regexp.new(Regexp.quote(pattern))
      end

      # Handle // as a special case.
      if pattern.source.empty?
        return split_characters(string, pattern, limited && limit, tail_empty)
      end

      start = 0
      ret = []

      last_match = nil
      last_match_end = 0

      while match = pattern.match_from(string, start)
        break if limited && limit - ret.size <= 1

        collapsed = match.collapsing?

        unless collapsed && (match.full.at(0) == last_match_end)
          ret << match.pre_match_from(last_match_end)
          ret.push(*match.captures.compact)
        end

        if collapsed
          start += 1
        elsif last_match && last_match.collapsing?
          start = match.full.at(1) + 1
        else
          start = match.full.at(1)
        end

        last_match = match
        last_match_end = last_match.full.at(1)
      end

      if last_match
        ret << last_match.post_match
      elsif ret.empty?
        ret << string.dup
      end

      # Trim from end
      if !ret.empty? and (limit.equal?(undefined) || limit == 0)
        while s = ret.last and s.empty?
          ret.pop
        end
      end

      ret
    end

    def self.split_on_string(string, pattern)
      pos = 0

      ret = []

      pat_size = pattern.size

      while pos < string.num_bytes
        nxt = string.find_string(pattern, pos)
        break unless nxt

        match_size = nxt - pos
        ret << string.byteslice(pos, match_size)

        pos = nxt + pat_size
      end

      # No more separators, but we need to grab the last part still.
      ret << string.byteslice(pos, string.num_bytes - pos)

      ret.pop while !ret.empty? and ret.last.empty?

      ret
    end
  end
end
