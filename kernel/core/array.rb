# Ruby's builtin dynamic array implementation
class Array
  ivar_as_index :total => 0, :tuple => 1
  
  def total    ; @total ; end
  def tuple    ; @tuple ; end
  def __ivars__; nil    ; end
  
  include Enumerable

  # The flow control for many of these methods is
  # pretty evil due to how MRI works. There is
  # also a lot of duplication of code due to very
  # subtle processing differences and, in some
  # cases, to avoid mutual dependency. Apologies.
  

  # Returns a new Array populated with the given objects
  def self.[](*args)
    return args if self.class == Array
    new(args.size) { |i| args[i] }   
  end                                                 # self.[]


  # Creates a new Array. Without arguments, an empty
  # Array is returned. If the only argument is an object
  # that responds to +to_ary+, a copy of that Array is 
  # created. Otherwise the first argument is the size
  # of the new Array (default 0.) The second argument 
  # may be an object used to fill the Array up to the
  # size given (the same object, not a copy.) Alternatively,
  # a block that takes the numeric index can be given and
  # will be run size times to fill the Array with its
  # result. The block supercedes any object given. If
  # neither is provided, the Array is filled with nil.
  def initialize(*args)
    raise ArgumentError, "Wrong number of arguments, #{args.size} for 2" if args.size > 2

    if args.empty?
      @tuple = Tuple.new 10
      @total = 0

    elsif (args.first.kind_of? Array or args.first.respond_to? :to_ary) and args.size == 1
      ary = ary_from args.first

      @tuple = Tuple.new(ary.size + 10)
      @total = ary.size
      @tuple.copy_from ary.tuple, 0

    else
      count = int_from args.first
      obj   = args[1]
      raise ArgumentError, "Size must be positive" if count < 0

      @tuple = Tuple.new(count + 10)
      @total = count

      if block_given?   
        count.times { |i| @tuple.put i, yield(i) }

      else
        count.times { |i| @tuple.put i, obj }
      end
    end
  end                                                 # initialize

  # Element reference, returns the element at the given index or 
  # a subarray starting from the index continuing for length 
  # elements or returns a subarray from range elements. Negative 
  # indices count from the end. Returns nil if the index or subarray 
  # request cannot be completed. Array#slice is synonymous with #[].
  # Subclasses return instances of themselves.
  def [](one, two = nil)
    # Normalise the argument variants
    start, finish, count, simple = nil, nil, nil, false

    if one.kind_of? Range
      start, finish = one.begin, one.end
    elsif two
      start, count = one, int_from(two)
      return nil if count < 0       # No need to go further
    else  
      start, finish, simple = one, one, true
    end

    # Convert negative indices
    start = int_from start
    start += @total if start < 0

    return self.class.new if start == @total and !simple   # ONE past end only, MRI compat

    finish = int_from finish if finish
    finish = (start + count - 1) if count    # For non-ranges

    finish += @total if finish < 0

    finish -= 1 if one.kind_of? Range and one.exclude_end?

    # Going past the end is ignored (sort of)
    finish = (@total - 1) if finish >= @total 

    return nil if start < 0 || start >= @total

    if finish < 0
      return self.class.new if one.kind_of? Range
      return nil
    end

    return @tuple.at(start) if simple

    return self.class.new if finish < start

    return self.class.new if count == 0

    out = self.class.new
    start.upto(finish) { |i| out << at(i) }
    out
  end                                                 # []


  def []=(idx, ent, *args)
    cnt = nil
    if args.size != 0
      cnt = ent
      ent = args[0]             # 2nd arg (cnt) is the optional one!
    end

    # Normalise Ranges
    if idx.class == Range
      if cnt
        raise ArgumentError, "Second argument invalid with a range"
      end
      lst = idx.last
      if lst < 0
        lst += @total
      end
      lst += 1 unless idx.exclude_end?

      idx = idx.first
      if idx < 0
        idx += @total
        raise IndexError if idx < 0
      end

      # m..n, m > n allowed
      lst = idx if idx > lst

      cnt = lst - idx
    end

    if idx < 0
      idx += @total
      raise IndexError.new("Index #{idx -= @total} out of bounds") if idx < 0
    end

    if cnt
      # count < 0 not allowed
      raise IndexError.new("Negative length #{cnt}") if cnt < 0

      cnt = @total - idx if cnt > @total - idx # MRI seems to be forgiving here!
      replacement = ent
      if ent == nil
        replacement = []
      elsif ent.class != Array  # FIXME: right test?
        replacement = [ent]
      end

      if replacement.size > cnt
        newtotal = @total + replacement.size - cnt
        if newtotal > @tuple.fields
          nt = Tuple.new(newtotal + 10)
          nt.copy_from @tuple, 0 # FIXME: double copy of right part
          @tuple = nt
        end                     # this should be an else
        f = @total
        t = newtotal
        while f > idx + cnt
          t -= 1
          f -= 1
          @tuple.put(t, @tuple.at(f))
        end
        @total = newtotal
      end
      replacement.each_with_index { |el, i|
        @tuple.put(idx+i, el)
      }
      if replacement.size < cnt
        f = idx + cnt
        t = idx + replacement.size
        while f < @total
          @tuple.put(t, @tuple.at(f))
          t += 1
          f += 1
        end
        @total -= cnt - replacement.size
      end

      return ent
    end

    if idx >= @tuple.fields
      nt = Tuple.new(idx + 10)
      nt.copy_from @tuple, 0
      @tuple = nt
    end

    @tuple.put idx, ent
    if idx >= @total - 1
      @total = idx + 1
    end
    return ent
  end
  
  # Appends the object to the end of the Array.
  # Returns self so several appends can be chained.
  def <<(obj)
    raise TypeError, "Array is frozen" if frozen?

    reallocate :at_least => (@total + 1)
    @tuple.put @total, obj
    @total += 1
    self
  end                                                 # <<

  # Creates a new Array containing only elements common to
  # both Arrays, without duplicates. Also known as a 'set
  # intersection'
  def &(other)
    other = ary_from other

    out, set_include = [], {}

    other.each { |x| set_include[x] = [true, x] }
    each { |x| 
      if set_include[x] and set_include[x].last.eql?(x)
        out << x
        set_include[x] = false
      end
    }

    out
  end                                                 # &

  # Creates a new Array by combining the two Arrays' items,
  # without duplicates. Also known as a 'set union.'
  def |(other)
    other = ary_from other

    out, exclude = [], {}
    
    (self + other).each { |x|
      unless exclude[x]
        out << x
        exclude[x] = true
      end
    }

    out
  end                                                 # |
  
  # Repetition operator when supplied a #to_int argument:
  # returns a new Array as a concatenation of the given number
  # of the original Arrays. With an argument that responds to
  # #to_str, functions exactly like #join instead.
  def *(val)
    if val.respond_to? :to_str
      return join(val)

    else 
      # Aaargh stupid MRI's stupid specific stupid error stupid types stupid
      val = int_from val

      raise ArgumentError, "Count cannot be negative" if val < 0

      out = self.class.new
      val.times { out.push(*self) }
      out
    end
  end                                                 # *
  
  # Create a concatenation of the two Arrays.
  def +(other)
    other = ary_from other
    out = []

    each { |e| out << e }
    other.each { |e| out << e }

    out
  end                                                 # +
  
  # Creates a new Array that contains the items of the original 
  # Array that do not appear in the other Array, effectively
  # 'deducting' those items. The matching method is Hash-based.
  def -(other)
    other = ary_from other
    out, exclude = [], {}

    other.each { |x| exclude[x] = true }
    each { |x| out << x unless exclude[x] }

    out
  end                                                 # -
  
  # Compares the two Arrays and returns -1, 0 or 1 depending
  # on whether the first one is 'smaller', 'equal' or 'greater'
  # in relation to the second. Two Arrays are equal only if all
  # their elements are 0 using first_e <=> second_e and their
  # lengths are the same. The element comparison is the primary
  # and length is only checked if the former results in 0's.
  def <=>(other)
    other = ary_from other

    size.times { |i| 
      return 1 unless other.size > i

      diff = at(i) <=> other.at(i)
      return diff if diff != 0 
    }

    return 1 if size > other.size
    return -1 if size < other.size
    0
  end                                                 # <=>

  # The two Arrays are considered equal only if their
  # lengths are the same and each of their elements 
  # are equal according to first_e == second_e . Both
  # Array subclasses and to_ary objects are accepted.
  def ==(other)
    unless other.kind_of? Array
      return false unless other.respond_to? :to_ary
      other = other.to_ary
    end

    return false unless size == other.size

    size.times { |i| return false unless at(i) == other.at(i) }
    
    true
  end                                                 # ==

  # Assumes the Array contains other Arrays and searches through 
  # it comparing the given object with the first element of each
  # contained Array using obj.==. Returns the first contained 
  # Array that matches (the first 'associated' Array) or nil.
  def assoc(obj)
    # FIX: use break when it works again
    found, res = nil, nil

    each { |elem| 
      if found.nil? and elem.kind_of? Array and obj == elem.first 
        found, res = true, elem
      end
    }

    res
  end                                                 # assoc

  # Returns the element at the given index. If the
  # index is negative, counts from the end of the
  # Array. If the index is out of range, nil is
  # returned. Slightly faster than +Array#[]+
  def at(idx)
    idx = int_from idx
    idx += @total if idx < 0

    return nil if idx < 0 || idx >= @total

    @tuple.at idx
  end                                                 # at

  # Generates a string from converting all elements of 
  # the Array to strings, inserting a separator between
  # each. The separator defaults to $,. Detects recursive
  # Arrays.
  def join(separator = nil, method = :to_s)
    return '' if @total == 0
    return at(0).to_s if @total == 1

    separator = $, unless separator
    raise TypeError, "Cannot convert #{separator.inspect} to str" unless separator.respond_to? :to_str

    separator, out, stack = separator.to_str, "", []

    recursively_join self, separator, out, stack
    out[separator.size..-1] 
  end                                                 # join
  
  def push(*args)
    args.each do |ent|
      self[@total] = ent
    end
    self
  end
  

  def each_index
    i = 0
    while i < @total
      yield i
      i += 1
    end
    self
  end

  def each
    i = 0
    while i < @total
      yield @tuple.at(i)
      i += 1
    end
    self
  end
  
  def reverse_each
    i = @total
    while i > 0
      i -= 1
      yield @tuple.at(i)
    end
    self
  end
  
  def map!
    i = 0
    each do |a|
      self[i] = yield(a)
      i += 1
    end
    return self
  end

  def eql?(other)
    return true if equal? other
    return false unless other.kind_of?(Array)
    return false if @total != other.size
    each_with_index do |o, i|
      return false unless o.eql? other[i]
    end
    return true
  end

  #  def collect!                  # FIXME: should use alias
  #    i = 0
  #    each do |a|
  #      self[i] = yield(a)
  #      i += 1
  #    end
  #    return self
  #  end

  alias :collect! :map!

  def concat(arr)
    push(*arr)
  end
  # The following three are all slightly different...

  def compact!
    i = 0
    while i < @total
      if @tuple.at(i) == nil
        j = i
        i += 1
        while i < @total
          if @tuple.at(i) != nil
            @tuple.put(j, @tuple.at(i))
            j += 1
          end
          i += 1
        end
        @total = j              # OK to leave tuple larger?
        return self
      end
      i += 1
    end
    nil                         # i.e., no change
  end
  
  def compact
    dup.compact!
  end

  def delete(obj)
    i = 0
    while i < @total
      if @tuple.at(i) == obj
        j = i
        i += 1
        while i < @total
          if @tuple.at(i) != obj
            @tuple.put(j, @tuple.at(i))
            j += 1
          end
          i += 1
        end
        @total = j              # OK to leave tuple larger?
        return obj
      end
      i += 1
    end
    yield if block_given?
    nil
  end

  def delete_if
    i = 0
    while i < @total
      if yield @tuple.at(i)
        j = i
        i += 1
        while i < @total
          unless yield @tuple.at(i)
            @tuple.put(j, @tuple.at(i))
            j += 1
          end
          i += 1
        end
        @total = j              # OK to leave tuple larger?
        return self
      end
      i += 1
    end
    return self
  end

  def delete_at(idx)
    if idx < 0
      idx += @total
      return nil if idx < 0
    end

    if idx >= @total
      return nil
    end
    obj = @tuple.at(idx)
    idx += 1
    while idx < @total
      @tuple.put(idx-1, @tuple.at(idx))
      idx += 1
    end
    @total -= 1
    return obj
  end

  def rassoc(obj)
    find { |x|
      Array === x && x[1] == obj
    }
  end

  def to_ary
    self
  end

  def to_a
    if self.class == Array
      self
    else
      self[0..-1] or []
    end
  end

  def to_s
    self.join
  end

  def inspect
    "[#{join(", ", :inspect)}]"
  end

  alias slice []

  # crashes compiler:
  #  def slice! (*args)
  #    out = self[*args]
  #    self[*args] = nil
  #    out
  #  end
  def slice!(*args)
    a1 = args.shift
    out = self[a1, *args]       # FIXME: a1 is needed to avoid compiler crash
    args.unshift a1

    args << 1 if args.size == 1 && !(Range === args[0])
    args << []
    self.send(:"[]=", *args)
    #self[*args] = []       # FIXME: this has mysterious bugs
    out
  end

  def flatten!
    ret = nil
    i = 0
    while i < @total
      o = @tuple.at(i)
  
      if Array === o
        self[i, 1] = o          # FIXME: quadratic.
        ret = self
      end
      
      i += 1      
    end
    ret
  end

  def flatten
    ret = []
    each { |o|
      if Array === o
        ret.concat(o.flatten)
      else
        ret << o
      end
    }
    ret
  end

  def self.[](*args)
    new.push(*args)
  end

  def values_at(*args)
    out = []
    args.each { |x|
      if Range === x
        # doesn't work: x.each { |i| out << self[i] }: one nil too many
        # doesn't work: out.concat self[x]: [1, 2, 3, 4, 5].values_at(0..2, 1...3, 4..6).should == [1, 2, 3, 2, 3, 5, nil]
        lst = x.last
        if lst < 0
          lst += @total
        end
        if lst > size
          lst = size            # sick to do this BEFORE exclude_end?
        end
        lst -= 1 if x.exclude_end?
        idx = x.first  
        idx.upto(lst) { |i|
          out << self[i]
        }
      else
        out << self[x]
      end
    }
    out
  end

  def indexes(*args)
    out = []
    args.each { |x|
      out << self[x]
    }
    out
  end

  alias indices indexes

  def transpose()
    result = []
    elems = map { |a| 
      begin
        a.to_ary 
      rescue 
        raise TypeError, "Unable to convert #{a.class.name} to Array" 
      end
    }
    a = elems.shift

    a.each_with_index { |a, i|
      result << elems.inject([a]) { |memo, o| memo << o.fetch(i) } 
    }

    result
  end

  def zip(*others)
    result = []
    others = others.map { |a| a.to_ary }

    each_with_index { |a, i|
      result << others.inject([a]) { |memo, o| memo << o.fetch(i, nil) } 
      yield result.last if block_given?
    }

    result unless block_given?
  end

  def sort(&block)
    return self.dup if self.size <= 1
    array = self.dup
    
    pivot   = array.first
    left    = []
    middle  = []
    right   = []    
    
    each { |elem| 
      begin
        if block_given?
          result = yield(elem, pivot) || 0          
        else
          # Done the 'wrong' way around to keep comp with MRI        
          result = pivot <=> elem
        end
        
        # Done backward to keep comp with MRI when no block is given
        if result == -1
          (block_given? ? left : right) << elem
        elsif result == 0
          middle << elem
        elsif result == 1
          (block_given? ? right : left) << elem
        else
          raise ArgumentError, block_given ? "Unknown result #{result} from block" : 
            "Unable to <=> on candidate #{elem.inspect}"
        end

      rescue TypeError
        raise ArgumentError, "Unable to compare #{pivot.class} with #{elem.class}"
      rescue
        raise
      end
    }

    left = left.sort(&block)
    right = right.sort(&block)

    left + middle + right    
  end

  def sort!(&block)
    replace(sort(&block))
  end

  def rindex(obj)
    i = @total
    while i > 0
      i -= 1
      return i if @tuple.at(i) == obj
    end
    nil
  end

  def index(obj)
    i = 0
    while i < @total
      return i if @tuple.at(i) == obj
      i += 1
    end
    nil
  end  
  
  BASE_64_ALPHA = {}
  def self.after_loaded
    (0..25).each {|x| BASE_64_ALPHA[x] = ?A + x}
    (26..51).each {|x| BASE_64_ALPHA[x] = ?a + x - 26}
    (52..61).each {|x| BASE_64_ALPHA[x] = ?0 + x - 52}
    BASE_64_ALPHA[62] = ?+
    BASE_64_ALPHA[63] = ?/
  end

  # TODO fill out pack.
  def pack(schema)
    # The schema is an array of arrays like [["A", "6"], ["u", "*"], ["X", ""]]. It represents the parsed
    # form of "A6u*X".
    schema = schema.scan(/([a-zA-Z][\d\*]*)/).flatten.map {|x| x.match(/([a-zA-Z])([\d\*]*)/)[1..-1] }
    
    # create the buffer
    ret = ""
    # we're starting from the first element in the array
    arr_idx = 0
    schema.each do |scheme|
      # get the kind of pack
      kind = scheme[0]
      # get the array item being worked on
      item = self[arr_idx]
      # set t to nil if no number (or "*") was passed in
      t = scheme[1].empty? ? nil : scheme[1]
      # X deletes a number of characters from the buffer (defaults to one; * means 0)
      if kind == "X"
        # set the default number to 1; otherwise to_i will give us the correct value
        t = t.nil? ? 1 : t.to_i
        # don't allow backing up farther than the size of the buffer
        raise ArgumentError, "you're backing up too far" if t > ret.size
        ret = ret[0..-(t + 1)]
      # x returns just a group of null strings
      elsif kind == "x"
        t = t.nil? ? 1 : t.to_i
        ret = "\x0" * t
      # if there's no item, that means there's more schema items than array items,
      # so throw an error.
      elsif !item
        raise ArgumentError, "too few array elements"
      # TODO: Document this
      elsif kind == "N"
        obj = self[arr_idx]
        parts = []
        4.times do
          parts << (obj % 256)
          obj = obj / 256
        end
        3.downto(0) do |j|
          ret << parts[j].chr
        end
      # A and a both pad the text
      elsif kind =~ /[aAZ]/
        raise TypeError unless item.is_a?(String)
        # The total new string size will be:
        # * the number passed in
        # * the size of the array's string if "*" was passed in
        # * 1 if nothing was passed in
        size = !t ? 1 : (t == "*" ? item.size : t.to_i)              
        # Z has a twist: "*" adds a null to the end of the string
        size += 1 if kind == "Z" && t == "*"
        # Pad or truncate the string (with spaces) as appropriate        
        ret << ("%-#{size}s" % item.dup)[0...(size)]
        # The padding size is the calculated size minus the string size        
        padsize = size - item.size
        # Replace the space padding for null padding in "a" or "Z"
        ret = ret.gsub(/\ {#{padsize}}$/, ("\x0" * (padsize)) ) if kind =~ /[aZ]/ && padsize > 0
      # c returns a single character. If there's a size, it will gobble up more array elements
      elsif kind =~ /c/i
        raise TypeError if item.is_a?(String)
        # Size is the same as for A
        size = !t ? 1 : (t == "*" ? self.size : t.to_i)
        0.upto(size - 1) do |i| 
          raise ArgumentError, "too few array elements" if !self[arr_idx + i]
          # Add a character if there's an element at the array index; otherwise add a null
          ret << (self[arr_idx + i] ? "%c" % self[arr_idx + i] : "\x0")
        end
      # M returns a string encoded with Quoted printable
      elsif kind == "M"
         # normalize non-strings
         if !item.is_a?(String)
           item = item.to_s
         end
         encoded = ""
         # only 75 characters per line (each line ends with "=\n")
         e = item.scan(/.{0,73}/m).each do |result|
           # Loop through each byte
           # * if it's an encodable byte, encode it
           # * otherwise pass it through
           result.each_byte { |byte| encoded << (((32..60).to_a + (62..126).to_a + [9,10]).include?(byte) ? byte.chr : "=%02X" % byte) }         
           # add a line-ending to the end of the line
           encoded << "=\n"
         end
         ret << encoded
      # Base64 encoding
      elsif kind == "m"
        raise TypeError if !item.is_a?(String)
        # split the string into letters
        letters = item.split(//)
        # get a series of 0-padded 8-bit representations of the letters
        letters.map! {|letter| "%08d" % letter[0].to_s(2) }
        # merge the 8-bit representations into 24-bit representations (divisible by 6 and 8)
        even_bitstream = letters.join.scan(/.{0,24}/).reject {|stream| stream.empty? }
        # pad the 24-bit streams so we have an set of complete, 24-bit numbers (with a's for padding characters)
        even_bitstream.map! {|bitset| ("%-24s" % bitset).gsub(" ", "a") }
        # split the numbers up into 6-bit (base64) fragments
        base_64_stream = even_bitstream.join.scan(/.{0,6}/).reject {|stream| stream.empty? }
        # convert the 6-bit fragments into base64 characters. 'aaaaaa' means a padded base64 fragment
        encoded_letters = base_64_stream.map do |fragment|
          # if there's a mixture of digits and "a", the a's should be 0's
          fragment = fragment.gsub("a", "0") if fragment =~ /\da/
          # if the fragment is not a pad, get the letter from the alphabet; otherwise, pad with "="
          (fragment != "aaaaaa") ? BASE_64_ALPHA[("%08s" % fragment).to_i(2)].chr : "="
        end
        # almost done; join the encoded letters together
        unbroken_stream = encoded_letters.join
        # break the stream up into 60 character sets
        broken_stream = unbroken_stream.scan(/.{0,60}/).reject{|set| set.empty?}
        # add \n to the end of each line (including the last line)
        ret << broken_stream.map {|set| set + "\n"}.join
      # UUEncode
      elsif kind == "u"
        raise TypeError if !item.is_a?(String)
        # split the string into 45-character lines
        lines = item.scan(/.{0,45}/)
        # get a series of 0-padded 8-bit representations of the lines
        lines.map! do |line|
          # store line length
          line_length = line.size
          # get a list of 0-padded 8-bit representations of the line
          letters = line.split(//).map! {|letter| "%08d" % letter[0].to_s(2) }
          # merge the 8-bit representations into 24-bit representations (divisible by 6 and 8)
          even_bitstream = letters.join.scan(/.{0,24}/).reject {|stream| stream.empty? }
          # pad the 24-bit streams so we have an set of complete, 24-bit numbers (with a's for padding characters)
          even_bitstream.map! {|bitset| ("%-24s" % bitset).gsub(" ", "0") }
          # split the numbers up into 6-bit fragments
          base_64_stream = even_bitstream.join.scan(/.{0,6}/).reject {|stream| stream.empty? }
          # convert the 6-bit fragments into ASCII characters.
          encoded_letters = base_64_stream.map do |fragment|
            fragment == "000000" ? "`" : (("%08s" % fragment).to_i(2) + 32).chr
          end
          # almost done; join the encoded letters together
          unbroken_stream = encoded_letters.join
          # return properly set up line (size preamble and line feed ending) from the map
          "#{(line_length + 32).chr}#{unbroken_stream}\n"
        end
        # add \n to the end of each line (including the last line)
        ret << lines.join
      else
        raise ArgumentError, "Unknown kind #{kind.chr}"
      end
      arr_idx += 1
    end
    
    return ret
  end
  
  def reverse
    ary = []
    i = @total - 1
    while i >= 0
      ary << self[i]
      i -= 1
    end
    return ary
  end

  def uniq
    seen = {}
    ary = []
    i = 0
    while i < @total
      e = @tuple.at(i)
      ary << e unless seen[e]
      seen[e] = true
      i += 1
    end
    ary
  end

  def uniq!
    ary = self.uniq
    ary.size == self.size ? nil : replace(ary)
  end
  
  def fetch(pos, *rest)
    if rest.length > 1
      raise ArgumentError, "wrong number of arguments (#{1 + rest.length} for 2)"
    end
    if !rest.empty? && block_given?
      #warn about block superceding default arg
    end
    index = pos
    if index < 0
      index += self.length
    end
    if index < 0 || self.length <= index
      return yield(pos) if block_given?
      if rest.length == 0
        raise IndexError, "index #{index} out of array"
      end
      return rest[0]
    end
    self.at(index)
  end  

  def nitems
    self.inject(0) { |count, elt| elt ? count + 1 : count}
  end
  
  def insert(index, *items)
    return self if items.length == 0
    
    pos = index
    if pos == -1
      pos = self.length
    end
    if pos < 0
      pos += self.length + 1
      if pos < 0
        raise IndexError, "index #{index} out of array"
      end
    end

    #move up existing elements if necessary
    #this should probably some sort of fast native memmove operation
    if pos < self.length
      (self.length - 1).downto(pos) {|i| self[i + items.length] = self[i] }
    end
    items.each_with_index {|el, i| self[i + pos] = el}
    self
  end
  
  def reject!(&block)
    old_length = self.length
    self.delete_if(&block)
    old_length == self.length ? nil : self
  end
  
  def reverse!
    if self.length > 1
       len = self.length - 1
       0.upto(len / 2) do |i|
         tmp = self[i]
         self[i] = self[len - i]
         self[len - i] = tmp
       end
    end
    self
  end
  
  def fill(*args)
    start_index = 0
    end_index = self.length - 1
    len = item = nil
    arg_length = args.length
    arg_index_offset = 0
    max_args = 2
    if !block_given?
      item = args[0]
      arg_length -= 1
      arg_index_offset = 1
      max_args = 3
    end

    arg1 = args[arg_index_offset] || 0
    arg2 = args[1 + arg_index_offset]

    case arg_length
    when 0
      start_index = 0
      end_index = self.length - 1
    when 1
      if arg1.nil? || arg1.kind_of?(Integer)
        start_index = arg1 || 0
        end_index = self.length - 1
      elsif arg1.kind_of?(Range)
        start_index = arg1.first
        end_index = arg1.last
        end_index -= 1 if arg1.exclude_end?
        if end_index < 0
          end_index += self.length
        end
      else
        raise TypeError, "can't convert #{arg1.class} into Integer"
      end
    when 2
      start_index = arg1
      len = arg2
    else
      raise ArgumentError, "wrong number of arguments (#{args.length} for #{max_args})"
    end
    if start_index < 0
      start_index += self.length
      start_index = 0 if start_index < 0
    end
    if len
      end_index = start_index + len - 1
    end
    
    (start_index..end_index).each do |i|
      self[i] = block_given? ? yield(i) : item
    end
    self
  end
  
  def first(n=nil)
    return self[0] unless n
    raise ArgumentError, "negative array size (or size too big)" if n < 0
    self[0...n]
  end
  
  def last(n=nil)
    return self[-1] unless n
    raise ArgumentError, "negative array size (or size too big)" if n < 0
    return [] if n.zero?
    n = size if n > size
    self[-n..-1]
  end

  
  def size
    @total
  end

  def length
    @total
  end
  
  def empty?
    @total == 0
  end
  
  
  def unshift(val)
    @tuple = @tuple.shifted(1)
    
    @tuple.put 0, val
    @total += 1
    return self
  end
  
  def replace(other)
    @tuple = other.tuple.dup
    @total = other.total
    return self
  end
  
  # Synonymous to #replace
  alias initialize_copy replace
  
  def clear
    @tuple = Tuple.new(0)
    @total = 0
    return self
  end
  
  def dup
    ary = Array.new
    each { |e| ary << e }
    return ary
  end
  
  def shift
    return nil if empty?
    
    ele = self[0]
    
    @tuple = @tuple.shift
    @total -= 1
    return ele
  end
  
  def pop
    return nil if empty?
    
    # TODO if total is a lot less than size of the tuple, 
    # the tuple should be resized down.
    
    ele = last()
    @total -= 1
    return ele
  end

  def hash
    # IMPROVE: This is a really really poor implementation of hash for an array, but
    # it does work. It should be replaced with something much better, but I'm not sure
    # what level it belongs at.
    str = ""
    each do |item|
      str << item.hash.to_s
    end
    return str.hash
  end
  
  def pretty_inspect(indent=0)
    str = "["
    
    sub = false
    i = 0
    lst = size - 1
    while i < size
      element = self[i]
      if Array === element
        estr = element.pretty_inspect(indent + 2)
        if str.size > 30 or estr.size > 30
          if estr[0] != ?\ 
            estr = "#{' ' * (indent + 2)}#{estr}"
          end

          str << "\n#{estr}"
          sub = true
        else
          str << estr
        end
      else
        str << element.inspect
      end
      
      str << ", " unless i == lst
      i += 1
    end
    
    if sub
      str << "\n#{' ' * indent}]"
    else
      str << "]"
    end
    
    if sub
      return "#{' ' * indent}#{str}"
    end
    
    return str
  end


  # Internals
  private

    # Reallocates the internal Tuple to accommodate at least given size
    def reallocate(at_least)
      at_least = at_least[:at_least] if Hash === at_least
      return if at_least < @tuple.size

      factor = @total < 1_000_000 ? 2 : 1        # Does this make any sense?
      
      tuple = Tuple.new((at_least * factor) + 10)

      tuple.copy_from @tuple, 0     # Swap over old data

      @tuple = tuple
    end

    # Attempt to convert the given object to_int or fail 
    def int_from(obj)
      unless obj.kind_of? Integer
        raise TypeError, "Unable to convert #{obj.inspect} to Integer" unless obj.respond_to? :to_int
        return obj.to_int
      end

      obj
    end                                               # int_from

    # Attempt to convert the given object to_ary or fail 
    def ary_from(obj)
      unless obj.kind_of? Array
        raise TypeError, "Unable to convert #{obj.inspect} to Array" unless obj.respond_to? :to_ary
        return obj.to_ary
      end

      obj
    end                                               # ary_from

    # Helper to depth-first recurse through joining an Array
    # Detects recursive structures.
    def recursively_join(array, separator, out, stack)
      if stack.include?(array.object_id)
        out << separator << "[...]"
      end

      stack.push array.object_id

      array.each { |o|
        if o.kind_of? Array
          recursively_join(o, separator, out, stack)
        else
          out << separator << o.to_s
        end
      }

      stack.pop
      out
    end                                                 # recursively_inspect
end
