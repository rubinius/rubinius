# -*- encoding: us-ascii -*-

class Regexp
  OPTION_MASK = IGNORECASE | EXTENDED | MULTILINE | DONT_CAPTURE_GROUP | CAPTURE_GROUP

  ValidKcode = [?n, ?e, ?s, ?u]
  KcodeValue = [KCODE_NONE, KCODE_EUC, KCODE_SJIS, KCODE_UTF8]

  def initialize(pattern, opts=nil, lang=nil)
    if pattern.kind_of?(Regexp)
      opts = pattern.options
      pattern = pattern.source
    elsif opts.kind_of?(Fixnum)
      opts = opts & (OPTION_MASK | KCODE_MASK) if opts > 0
    elsif opts
      opts = IGNORECASE
    else
      opts = 0
    end

    if lang.kind_of?(String)
      opts &= OPTION_MASK
      idx   = ValidKcode.index(lang.downcase[0])
      opts |= KcodeValue[idx] if idx
    end

    compile pattern, opts
  end

  def initialize_copy(other)
    initialize other.source, other.options, other.kcode
  end

  def match(str)
    unless str
      Regexp.last_match = nil
      return nil
    end

    str = StringValue(str)

    Regexp.last_match = search_region(str, 0, str.bytesize, true)
  end

  # Returns the index of the first character in the region that
  # matched or nil if there was no match. See #match for returning
  # the MatchData instead.
  def =~(str)
    # unless str.nil? because it's nil and only nil, not false.
    str = StringValue(str) unless str.nil?

    match = match_from(str, 0)
    if match
      Regexp.last_match = match
      return match.begin(0)
    else
      Regexp.last_match = nil
      return nil
    end
  end

  def ===(other)
    unless other.kind_of?(String)
      other = Rubinius::Type.check_convert_type other, String, :to_str
      unless other
        Regexp.last_match = nil
        return false
      end
    end

    if match = match_from(other, 0)
      Regexp.last_match = match
      true
    else
      Regexp.last_match = nil
      false
    end
  end

  def eql?(other)
    return false unless other.kind_of?(Regexp)
    return false unless source == other.source

    # Ruby 1.8 doesn't destinguish between KCODE_NONE (16) & not specified (0) for eql?
    o1 = options
    if o1 & KCODE_MASK == 0
      o1 += KCODE_NONE
    end

    o2 = other.options
    if o2 & KCODE_MASK == 0
      o2 += KCODE_NONE
    end

    o1 == o2
  end

  alias_method :==, :eql?

  def hash
    str = '/' << source << '/' << option_to_string(options)
    if options & KCODE_MASK == 0
      str << 'n'
    else
      str << kcode[0, 1]
    end
    str.hash
  end

  def inspect
    # the regexp matches any / that is after anything except for a \
    escape = source.gsub(%r!(\\.)|/!) { $1 || '\/' }
    str = "/#{escape}/#{option_to_string(options)}"
    k = kcode
    str << k[0, 1] if k
    str
  end

  def kcode
    lang = options & KCODE_MASK
    return "none" if lang == KCODE_NONE
    return "euc"  if lang == KCODE_EUC
    return 'sjis' if lang == KCODE_SJIS
    return 'utf8' if lang == KCODE_UTF8
    return nil
  end

  def self.union(*patterns)
    case patterns.size
    when 0
      return %r/(?!)/
    when 1
      pat = patterns.first
      case pat
      when Array
        return union(*pat)
      when Regexp
        return pat
      else
        return Regexp.new(Regexp.quote(StringValue(pat)))
      end
    end

    kcode = nil
    str = ""
    patterns.each_with_index do |pat, idx|
      str << "|" if idx != 0

      if pat.kind_of? Regexp
        if pat_kcode = pat.kcode
          if kcode
            if kcode != pat_kcode
              raise ArgumentError, "Conflict kcodes: #{kcode} != #{pat_kcode}"
            end
          else
            kcode = pat_kcode
          end
        end

        str << pat.to_s
      else
        str << Regexp.quote(StringValue(pat))
      end
    end

    Regexp.new(str, nil, kcode)
  end

  def self.escape(str)
    StringValue(str).transform(ESCAPE_TABLE, true)
  end

  class << self
    alias_method :quote, :escape
  end
end

class MatchData

  def begin(idx)
    if idx == 0
      @full.at(0)
    else
      start = @region.at(idx - 1).at(0)
      return nil if start == -1
      start
    end
  end

  def end(idx)
    if idx == 0
      @full.at(1)
    else
      fin = @region.at(idx - 1).at(1)
      return nil if fin == -1
      fin
    end
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
    when Symbol
      num = @regexp.name_table[idx]
      raise ArgumentError, "Unknown named group '#{idx}'" unless num
      return self[num + 1]
    when String
      num = @regexp.name_table[idx.to_sym]
      raise ArgumentError, "Unknown named group '#{idx}'" unless num
      return self[num + 1]
    end

    return to_a[idx]
  end
end
