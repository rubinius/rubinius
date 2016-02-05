class MatchData
  def begin(idx)
    if idx == 0
      start = @full.at(0)
    else
      start = @region.at(idx - 1).at(0)
      return nil if start == -1
    end
    m = Rubinius::Mirror.reflect @source
    m.byte_to_character_index start
  end

  def end(idx)
    if idx == 0
      fin = @full.at(1)
    else
      fin = @region.at(idx - 1).at(1)
      return nil if fin == -1
    end
    m = Rubinius::Mirror.reflect @source
    m.byte_to_character_index fin
  end

  def offset(idx)
    out = []
    out << self.begin(idx)
    out << self.end(idx)
    return out
  end

  def [](idx, len = nil)
    return to_a[idx, len] if len

    case idx
    when Fixnum
      if idx <= 0
        return matched_area() if idx == 0
        return to_a[idx]
      elsif idx <= @region.size
        tup = @region[idx - 1]

        x = tup.at(0)
        return nil if x == -1

        y = tup.at(1)
        return @source.byteslice(x, y-x)
      end
    when String
      if @regexp.name_table
        return self[idx.to_sym]
      end
      raise IndexError, "Unknown named group '#{idx}'"
    when Symbol
      if @regexp.name_table
        if nums = @regexp.name_table[idx]
          nums.reverse_each do |num|
            val = self[num]
            return val if val
          end
          return nil
        end
      end
      raise IndexError, "Unknown named group '#{idx}'"
    end

    return to_a[idx]
  end

  attr_reader :regexp

  def ==(other)
    other.kind_of?(MatchData) &&
      string == other.string  &&
      regexp == other.regexp  &&
      captures == other.captures
  end
  alias_method :eql?, :==

  def string
    @source.dup.freeze
  end

  def source
    @source
  end

  def full
    @full
  end

  def length
    @region.fields + 1
  end

  def captures
    out = Array.new(@region.fields)

    idx = 0
    @region.each do |tup|
      x = tup.at(0)

      if x == -1
        val = nil
      else
        y = tup.at(1)
        val = @source.byteslice(x, y-x)
      end

      out[idx] = val
      idx += 1
    end

    return out
  end

  def names
    @regexp.names
  end

  def pre_match
    return @source.byteslice(0, 0) if @full.at(0) == 0
    nd = @full.at(0) - 1
    @source.byteslice(0, nd+1)
  end

  def pre_match_from(idx)
    return @source.byteslice(0, 0) if @full.at(0) == 0
    nd = @full.at(0) - 1
    @source.byteslice(idx, nd-idx+1)
  end

  def collapsing?
    @full[0] == @full[1]
  end

  def post_match
    nd = @source.bytesize - 1
    st = @full.at(1)
    @source.byteslice(st, nd-st+1)
  end

  def inspect
    capts = captures
    if capts.empty?
      "#<MatchData \"#{matched_area}\">"
    else
      idx = 0
      capts.map! { |capture| "#{idx += 1}:#{capture.inspect}" }
      "#<MatchData \"#{matched_area}\" #{capts.join(" ")}>"
    end
  end

  def select
    unless block_given?
      raise LocalJumpError, "no block given"
    end

    out = []
    ma = matched_area()
    out << ma if yield ma

    each_capture do |str|
      if yield(str)
        out << str
      end
    end
    return out
  end

  alias_method :size, :length

  def to_a
    ary = captures()
    ary.unshift matched_area()
    return ary
  end

  def values_at(*indexes)
    indexes.map { |i| self[i] }.flatten(1)
  end

  def matched_area
    x = @full.at(0)
    y = @full.at(1)
    @source.byteslice(x, y-x)
  end

  alias_method :to_s, :matched_area
  private :matched_area

  def get_capture(num)
    x, y = @region[num]
    return nil if !y or x == -1

    return @source.byteslice(x, y-x)
  end

  private :get_capture

  def each_capture
    @region.each do |tup|
      x, y = *tup
      yield @source.byteslice(x, y-x)
    end
  end

  private :each_capture
end
