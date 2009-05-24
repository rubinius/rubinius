class String
  alias_method :bytesize, :length

  def each_char(&block)
    return to_enum :each_char unless block_given?
    return scan(/./u, &block) if $KCODE == "UTF-8"
    i = 0
    while i < @num_bytes do
      yield @data.get_byte(i).chr
      i += 1
    end

    self
  end

  alias_method :chars, :each_char

  # Passes each byte in <i>self</i> to the given block.
  #
  #   "hello".each_byte {|c| print c, ' ' }
  #
  # <em>produces:</em>
  #
  #   104 101 108 108 111
  def each_byte
    return to_enum :each_byte unless block_given?
    i = 0
    while i < @num_bytes do
      yield @data.get_byte(i)
      i += 1
    end
    self
  end

  # Splits <i>self</i> using the supplied parameter as the record separator
  # (<code>$/</code> by default), passing each substring in turn to the supplied
  # block. If a zero-length record separator is supplied, the string is split on
  # <code>\n</code> characters, except that multiple successive newlines are
  # appended together.
  #
  #   print "Example one\n"
  #   "hello\nworld".each {|s| p s}
  #   print "Example two\n"
  #   "hello\nworld".each('l') {|s| p s}
  #   print "Example three\n"
  #   "hello\n\n\nworld".each('') {|s| p s}
  #
  # <em>produces:</em>
  #
  #   Example one
  #   "hello\n"
  #   "world"
  #   Example two
  #   "hel"
  #   "l"
  #   "o\nworl"
  #   "d"
  #   Example three
  #   "hello\n\n\n"
  #   "world"
  def each(sep = $/)
    return to_enum :each, sep unless block_given?
    if sep.nil?
      yield self
      return self
    end

    sep = StringValue sep

    id = @data.object_id
    size = @num_bytes
    ssize = sep.size
    newline = ssize == 0 ? ?\n : sep[ssize-1]

    last, i = 0, ssize
    while i < size
      if ssize == 0 && @data[i] == ?\n
        if @data[i+=1] != ?\n
          i += 1
          next
        end
        i += 1 while i < size && @data[i] == ?\n
      end

      if i > 0 && @data[i-1] == newline &&
          (ssize < 2 || sep.compare_substring(self, i-ssize, ssize) == 0)
        line = substring last, i-last
        line.taint if tainted?
        yield line
        modified? id, size
        last = i
      end

      i += 1
    end

    unless last == size
      line = substring last, size-last+1
      line.taint if tainted?
      yield line
    end

    self
  end
  alias_method :each_line, :each

  def end_with?(*suffixes)
    suffixes.each do |suffix|
      next unless suffix.respond_to? :to_str
      suffix = suffix.to_str
      return true if self[-suffix.length, suffix.length] == suffix
    end
    false
  end

  # Returns a copy of <i>self</i> with <em>all</em> occurrences of <i>pattern</i>
  # replaced with either <i>replacement</i> or the value of the block. The
  # <i>pattern</i> will typically be a <code>Regexp</code>; if it is a
  # <code>String</code> then no regular expression metacharacters will be
  # interpreted (that is <code>/\d/</code> will match a digit, but
  # <code>'\d'</code> will match a backslash followed by a 'd').
  #
  # If a string is used as the replacement, special variables from the match
  # (such as <code>$&</code> and <code>$1</code>) cannot be substituted into it,
  # as substitution into the string occurs before the pattern match
  # starts. However, the sequences <code>\1</code>, <code>\2</code>, and so on
  # may be used to interpolate successive groups in the match.
  #
  # In the block form, the current match string is passed in as a parameter, and
  # variables such as <code>$1</code>, <code>$2</code>, <code>$`</code>,
  # <code>$&</code>, and <code>$'</code> will be set appropriately. The value
  # returned by the block will be substituted for the match on each call.
  #
  # The result inherits any tainting in the original string or any supplied
  # replacement string.
  #
  #   "hello".gsub(/[aeiou]/, '*')              #=> "h*ll*"
  #   "hello".gsub(/([aeiou])/, '<\1>')         #=> "h<e>ll<o>"
  #   "hello".gsub(/./) {|s| s[0].to_s + ' '}   #=> "104 101 108 108 111 "
  def gsub(pattern, replacement = Undefined, &prc)
    return to_enum :gsub, pattern, replacement unless block_given? || replacement != Undefined

    tainted = false

    unless replacement == Undefined
      tainted = replacement.tainted?
      replacement = StringValue(replacement)
      tainted ||= replacement.tainted?
    end

    pattern = get_pattern(pattern, true)
    copy = self.dup

    last_end = 0
    offset = nil
    ret = self.class.pattern(0,0) # Empty string, or string subclass

    last_match = nil
    match = pattern.match_from self, last_end

    offset = match.begin 0 if match

    while match do
      ret << (match.pre_match_from(last_end) || "")

      unless replacement == Undefined
        ret << replacement.to_sub_replacement(match)
      else
        # We do this so that we always manipulate $~ in the context
        # of the passed block.
        prc.block.top_scope.last_match = match

        val = yield(match[0].dup)
        tainted ||= val.tainted?
        ret << val.to_s

        raise RuntimeError, "string modified" if self != copy
      end

      tainted ||= val.tainted?

      last_end = match.end(0)
      offset = match.collapsing? ? offset + 1 : match.end(0)

      last_match = match

      match = pattern.match_from self, offset
      break unless match

      offset = match.begin 0
    end

    Rubinius::VariableScope.of_sender.last_match = last_match

    str = substring(last_end, @num_bytes-last_end+1)
    ret << str if str

    ret.taint if tainted || self.tainted?
    return ret
  end

  def start_with?(*prefixes)
    prefixes.each do |prefix|
      next unless prefix.respond_to? :to_str
      prefix = prefix.to_str
      return true if self[0, prefix.length] == prefix
    end
    false
  end
end
