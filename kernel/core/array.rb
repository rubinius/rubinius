# depends on: enumerable.rb

# Ruby's builtin dynamic array implementation
class Array
  ivar_as_index :total => 0, :tuple => 1, :start => 2, :shared => 3
  
  def total    ; @total ; end
  def tuple    ; @tuple ; end
  def start    ; @start ; end
  def __ivars__; nil    ; end
  
  include Enumerable

  # The flow control for many of these methods is
  # pretty evil due to how MRI works. There is
  # also a lot of duplication of code due to very
  # subtle processing differences and, in some
  # cases, to avoid mutual dependency. Apologies.
  

  # Returns a new Array populated with the given objects
  def self.[](*args)
    new.push(*args)
  end


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
    
    @start = 0
    
    if args.empty?
      @tuple = Tuple.new 2
      @total = 0
    else
      raise TypeError, "can't modify frozen array" if frozen?
      
      if (args.first.kind_of? Array or args.first.respond_to? :to_ary) and args.size == 1
        ary = ary_from args.first
        return self if self == ary
      
        @tuple = Tuple.new(ary.size + 10)
        @total = ary.size
        @tuple.copy_from ary.tuple, ary.start
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
    end
  
    self
  end              

  # Element reference, returns the element at the given index or 
  # a subarray starting from the index continuing for length 
  # elements or returns a subarray from range elements. Negative 
  # indices count from the end. Returns nil if the index or subarray 
  # request cannot be completed. Array#slice is synonymous with #[].
  # Subclasses return instances of themselves.
  def [](one, two = nil)
    # Normalise the argument variants
    start, finish, count, simple, is_range = nil, nil, nil, false, false

    if one.kind_of? Range
      is_range = true
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
    
    if simple
      return nil if start < 0 or start >= @total
      return @tuple.at(@start + start)
      
    # ONE past end only, MRI compat
    elsif start == @total
      return self.class.new
      
    elsif start < 0 or start >= @total
      return nil
    end

    finish = int_from finish if finish
    finish = (start + count - 1) if count    # For non-ranges

    finish += @total if finish < 0

    finish -= 1 if is_range and one.exclude_end?

    # Going past the end is ignored (sort of)
    finish = (@total - 1) if finish >= @total 

    if finish < 0
      return self.class.new if is_range
      return nil
    end

    return self.class.new if finish < start

    return self.class.new if count == 0

    out = self.class.new
    start.upto(finish) { |i| out << at(i) }
    out
  end             

  alias_method :slice, :[]

  def []=(idx, ent, *args)
    raise TypeError, "can't modify frozen array" if frozen?
    
    cnt = nil
    if args.size != 0
      cnt = ent.to_int
      ent = args[0]             # 2nd arg (cnt) is the optional one!
    end

    # Normalise Ranges
    if idx.is_a?(Range)
      if cnt
        raise ArgumentError, "Second argument invalid with a range"
      end
      
      unless idx.first.respond_to?(:to_int)
        raise TypeError, "can't convert #{idx.first.class} into Integer"
      end
      
      unless idx.last.respond_to?(:to_int)
        raise TypeError, "can't convert #{idx.last.class} into Integer"
      end
      
      lst = idx.last.to_int
      if lst < 0
        lst += @total
      end
      lst += 1 unless idx.exclude_end?

      idx = idx.first.to_int
      if idx < 0
        idx += @total
        raise RangeError if idx < 0
      end

      # m..n, m > n allowed
      lst = idx if idx > lst

      cnt = lst - idx
    end
    
    idx = idx.to_int
    
    if idx < 0
      idx += @total
      raise IndexError.new("Index #{idx -= @total} out of bounds") if idx < 0
    end

    if cnt
      # count < 0 not allowed
      raise IndexError.new("Negative length #{cnt}") if cnt < 0

      cnt = @total - idx if cnt > @total - idx # MRI seems to be forgiving here!
      
      if ent == nil
        replacement = []
      elsif ent.is_a?(Array)
        replacement = ent
      elsif ent.respond_to?(:to_ary)
        replacement = ent.to_ary
      else
        replacement = [ent]
      end

      if replacement.size > cnt
        newtotal = @total + replacement.size - cnt
        if newtotal > @tuple.fields
          nt = Tuple.new(newtotal + 10)
          nt.copy_from @tuple, @start # FIXME: double copy of right part
          @start = 0
          @tuple = nt
        end                     # this should be an else
        f = @total
        t = newtotal
        while f > idx + cnt
          t -= 1
          f -= 1
          @tuple.put(@start+t, @tuple.at(@start+f))
        end
        @total = newtotal
      end
      replacement.each_with_index { |el, i|
        @tuple.put(@start+idx+i, el)
      }
      
      if replacement.size < cnt
        f = @start + idx + cnt
        t = @start + idx + replacement.size
        
        # shift fields to the left
        while f < @total
          @tuple.put(t, @tuple.at(f))
          t += 1
          f += 1
        end
        
        # unset any extraneous fields
        while t < @tuple.fields
          @tuple.put(t, nil)
          t += 1
        end
        
        @total -= (cnt - replacement.size)
      end

      return ent
    end

    if idx >= @tuple.fields
      nt = Tuple.new(idx + 10)
      nt.copy_from @tuple, @start
      @tuple = nt
      @start = 0
    end

    @tuple.put @start + idx, ent
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
    @tuple.put @start + @total, obj
    @total += 1
    self
  end                                               

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
  end     

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
  end    
  
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
  end   
  
  # Create a concatenation of the two Arrays.
  def +(other)
    other = ary_from other
    out = []

    each { |e| out << e }
    other.each { |e| out << e }

    out
  end  
  
  # Creates a new Array that contains the items of the original 
  # Array that do not appear in the other Array, effectively
  # 'deducting' those items. The matching method is Hash-based.
  def -(other)
    other = ary_from other
    out, exclude = [], {}

    other.each { |x| exclude[x] = true }
    each { |x| out << x unless exclude[x] }

    out
  end 
  
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
  end

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

    size.times { |i| return false unless @tuple.at(@start + i) == other.at(i) }
    
    true
  end 

  # Assumes the Array contains other Arrays and searches through 
  # it comparing the given object with the first element of each
  # contained Array using elem == obj. Returns the first contained 
  # Array that matches (the first 'associated' Array) or nil.
  def assoc(obj)
    # FIX: use break when it works again
    found, res = nil, nil

    each { |elem| 
      if found.nil? and elem.kind_of? Array and elem.first == obj
        found, res = true, elem
      end
    }

    res
  end 

  # Returns the element at the given index. If the
  # index is negative, counts from the end of the
  # Array. If the index is out of range, nil is
  # returned. Slightly faster than +Array#[]+
  def at(idx)
    idx = int_from idx
    idx += @total if idx < 0

    return nil if idx < 0 or idx >= @total

    @tuple.at @start + idx
  end 
  
  # Removes all elements in the Array and leaves it empty
  def clear()
    raise TypeError, "Array is frozen" if frozen? 

    @tuple = Tuple.new(1)
    @total = 0
    @start = 0
    self
  end 

  # Returns a copy of self with all nil elements removed
  def compact()
    dup.compact! || self
  end 

  # Removes all nil elements from self, returns nil if no changes
  # TODO: Needs improvement
  def compact!()
    raise TypeError, "Array is frozen" if frozen?

    i = @start
    tot = @start + @total

    # Low-level because pretty much anything else breaks everything
    while i < tot
      if @tuple.at(i) == nil
        j = i
        i += 1

        while i < tot
          if @tuple.at(i) != nil
            @tuple.put j, @tuple.at(i)
            j += 1
          end

          i += 1
        end

        # OK to leave tuple size larger?
        @total = j - @start
        return self
      end

      i += 1
    end

    nil                 
  end

  # Appends the elements in the other Array to self
  def concat(other)
    raise TypeError, "Array is frozen" if frozen?

    push(*ary_from(other))
  end 

  # Stupid subtle differences prevent proper reuse in these three
  
  # Removes all elements from self that are #== to the given object.
  # If the object does not appear at all, nil is returned unless a
  # block is provided in which case the value of running it is
  # returned instead.
  def delete(obj)
    raise TypeError, "Array is frozen" if frozen?

    i = @start
    tot = @start + @total

    # Leaves the tuple to the original size still
    while i < tot
      if @tuple.at(i) == obj
        j = i
        i += 1

        while i < tot
          if @tuple.at(i) != obj
            @tuple.put(j, @tuple.at(i))
            j += 1
          end

          i += 1
        end

        @total = j - @start            
        return obj
      end

      i += 1
    end
  end  

  # Deletes the element at the given index and returns
  # the deleted element or nil if the index is out of
  # range. Negative indices count backwards from end.
  def delete_at(idx)
    raise TypeError, "Array is frozen" if frozen?

    idx = int_from idx

    # Flip to positive and weed out out of bounds
    idx += @total if idx < 0
    return nil if idx < 0 or idx >= @total

    # Grab the object and adjust the indices for the rest
    obj = @tuple.at(@start + idx)

    idx.upto(@total - 2) { |i| @tuple.put(@start + i, @tuple.at(@start + i + 1)) }
    @tuple.put(@start + @total - 1, nil)

    @total -= 1
    obj
  end 

  # Deletes every element from self for which block evaluates to true
  def delete_if()
    raise TypeError, "Array is frozen" if frozen?
 
    i = @start
    tot = @total + @start

    # Leaves the tuple to the original size still
    while i < tot
      if yield @tuple.at(i)
        j = i
        i += 1

        while i < tot
          unless yield @tuple.at(i)
            @tuple.put(j, @tuple.at(i))
            j += 1
          end

          i += 1
        end

        @total = j - @start          
        return self
      end

      i += 1
    end
    
    return self
  end

  # Passes each element in the Array to the given block
  # and returns self.
  def each()
    @total.times { |i| yield at(i) }
    self
  end 

  # Passes each index of the Array to the given block
  # and returns self.
  def each_index()
    @total.times {|i| yield i}
    self
  end 

  # Returns true if both are the same object or if both
  # have the same elements (#eql? used for testing.)
  def eql?(other)
    return true if equal? other
    return false unless other.kind_of?(Array)
    return false if @total != other.size

    each_with_index { |o, i| return false unless o.eql?(other[i]) }

    true
  end 

  # True if Array has no elements.
  def empty?()
    @total == 0
  end

  # Attempts to return the element at the given index. By default
  # an IndexError is raised if the element is out of bounds. The
  # user may supply either a default value or a block that takes
  # the index object instead.
  def fetch(idx, *rest)
    raise ArgumentError, "Expected 1-2, got #{1 + rest.length}" if rest.length > 1
    warn 'Block supercedes default object' if !rest.empty? && block_given?

    idx, orig = int_from(idx), idx
    idx += @total if idx < 0

    if idx < 0 || idx >= @total
      return yield(orig) if block_given?
      return rest.at(0) unless rest.empty?

      raise IndexError, "Index #{idx} out of array" if rest.empty?
    end

    at(idx)
  end

  # Fill some portion of the Array with a given element. The
  # element to be used can be either given as the first argument
  # or as a block that takes the index as its argument. The
  # section that is to be filled can be defined by the following
  # arguments. The first following argument is either a starting
  # index or a Range. If the first argument is a starting index,
  # the second argument can be the length. No length given
  # defaults to rest of Array, no starting defaults to 0. Negative
  # indices are treated as counting backwards from the end. Negative
  # counts leave the Array unchanged. Returns self.
  #
  # array.fill(obj)                                -> array
  # array.fill(obj, start [, length])              -> array
  # array.fill(obj, range)                         -> array
  # array.fill {|index| block }                    -> array
  # array.fill(start [, length]) {|index| block }  -> array
  # array.fill(range) {|index| block }             -> array
  #
  def fill(*args)
    raise TypeError, "Array is frozen" if frozen?

    raise ArgumentError, "Wrong number of arguments" if block_given? and args.size > 2
    raise ArgumentError, "Wrong number of arguments" if args.size > 3

    # Normalise arguments
    start, finish, obj = 0, (@total - 1), nil

    obj = args.shift unless block_given?
    one, two = args.at(0), args.at(1)

    if one.kind_of? Range
      raise TypeError, "Length invalid with range" if args.size > 1   # WTF, MRI, TypeError?

      start, finish = int_from(one.begin), int_from(one.end)

      start += @total if start < 0
      finish += @total if finish < 0

      if one.exclude_end?
        return self if start == finish
        finish -= 1
      end

      raise RangeError, "#{one.inspect} out of range" if start < 0 
      return self if finish < 0           # Nothing to modify

    else
      if one
        start = int_from one

        start += @total if start < 0
        start = 0 if start < 0            # MRI comp adjusts to 0

        if two
          finish = int_from two
          return self if finish < 1       # Nothing to modify

          finish = start + finish - 1
        end
      end
    end                                   # Argument normalisation

    # Adjust the size progressively
    unless finish < @total
      reallocate(finish)
      @total = finish + 1
    end

    if block_given?
      start.upto(finish) { |i|  @tuple.put @start + i, yield(i) }
    else
      start.upto(finish) { |i|  @tuple.put @start + i, obj }
    end


    self
  end 
  
  # Returns the first or first n elements of the Array.
  # If no argument is given, returns nil if the item
  # is not found. If there is an argument, an empty
  # Array is returned instead.
  def first(n = nil)
    return at(0) unless n

    n = int_from n
    raise ArgumentError, "Size must be positive" if n < 0

    Array.new(self[0...n])
  end    

  # Recursively flatten any contained Arrays into an one-dimensional result.  
  def flatten()
    dup.flatten! || self
  end 

  # Flattens self in place as #flatten. If no changes are
  # made, returns nil, otherwise self.
  def flatten!
    raise TypeError, "Array is frozen" if frozen?

    ret, out, stack = nil, [], []

    ret = recursively_flatten self, out, stack
    replace(out) if ret
    ret
    
#    recursor  = lambda { |ary|
#                  ary.each { |o|
#                    if o.kind_of? Array
#                      recursor.call o 
#                      ret = self
#                    else
#                      out << o
#                    end
#                  }
#                }
#
#    recursor.call self
#
#    replace(out) if ret
#    ret
  end 

  # Returns true if the Array is frozen with #freeze or
  # temporarily sorted while being sorted.
  def frozen?()
    @sort_frozen || super 
  end 

  # Computes a Fixnum hash code for this Array. Any two
  # Arrays with the same content will have the same hash
  # code (similar to #eql?)
  def hash()
    # IMPROVE: This is a really really poor implementation of hash for an array, but
    # it does work. It should be replaced with something much better, but I'm not sure
    # what level it belongs at.
    str = ""
    each { |item| str << item.hash.to_s }
    str.hash
  end

  # Returns true if the given obj is present in the Array.
  # Presence is determined by calling elem == obj until found.
  def include?(obj)
    @total.times { |i| return true if @tuple.at(@start + i) == obj }
    false
  end

  # Returns the index of the first element in the Array
  # for which elem == obj is true or nil.
  def index(obj)
    @total.times { |i| return i if @tuple.at(@start + i) == obj }
    nil
  end 

  # Returns an Array populated with the objects at the given indices of the original.
  # Range arguments are given as nested Arrays as from #[].
  def indexes(*args)
    warn 'Array#indexes is deprecated, use Array#values_at instead'

    out = []

    args.each { |a| 
      if a.kind_of? Range
        out << self[a]
      else
        out << at(int_from(a))
      end
    }

    out
  end

  alias_method :indices, :indexes

  # For a positive index, inserts the given values before
  # the element at the given index. Negative indices count 
  # backwards from the end and the values are inserted 
  # after them.
  def insert(idx, *items)
    raise TypeError, "Array is frozen" if frozen?

    return self if items.length == 0
    
    # Adjust the index for correct insertion
    idx = int_from idx
    idx += (@total + 1) if idx < 0    # Negatives add AFTER the element
    raise IndexError, "#{idx} out of bounds" if idx < 0

    self[idx, 0] = items   # Cheat
    self
  end 

  # Produces a printable string of the Array. The string 
  # is constructed by calling #inspect on all elements.
  # Descends through contained Arrays, recursive ones
  # are indicated as [...].
  def inspect()
    stack = []
    recursively_inspect self, stack
  end

  # Generates a string from converting all elements of 
  # the Array to strings, inserting a separator between
  # each. The separator defaults to $,. Detects recursive
  # Arrays.
  def join(sep = nil, method = :to_s)
    return '' if @total == 0
    return at(0).__send__(method) if @total == 1
    sep ||= $,

    raise TypeError, "Cannot convert #{sep.inspect} to str" unless sep.respond_to? :to_str
    sep = sep.to_str
    out = ""

    recursively_join self, sep, out, [], method
    out[sep.size..-1] 
  end 
  
  # Returns the last element or n elements of self. If
  # the Array is empty, without a count nil is returned,
  # otherwise an empty Array. Always returns an Array.
  def last(n = nil)
    return at(-1) unless n
    
    n = int_from n
    return [] if n.zero?
    raise ArgumentError, "Number must be positive" if n < 0

    n = size if n > size
    Array.new self[-n..-1] 
  end 
  
  # Creates a new Array from the return values of passing
  # each element in self to the supplied block.
  def map()
    out = []
    each { |elem| out << yield(elem) }
    out
  end 

  alias_method :collect, :map

  # Replaces each element in self with the return value
  # of passing that element to the supplied block.
  def map!(&block)
    raise TypeError, "Array is frozen" if frozen? 

    replace(map &block)
  end
  
  alias_method :collect!, :map!

  # Returns number of non-nil elements in self, may be zero
  def nitems
    sum = 0
    each { |elem| sum += 1 unless elem.nil? }
    sum
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

  # Removes and returns the last element from the Array.
  def pop()
    raise TypeError, "Array is frozen" if frozen?
    return nil if empty?
    
    # TODO Reduce tuple if there are a lot of free slots
    
    elem = at(-1)
    @total -= 1
    elem
  end
  
  # Rubinius-only, better inspect representation of the Array
  def pretty_inspect(indent = 0)
    # Here there be dragons. In fact, there is one jusAAAAAAAARGH
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

  # Appends the given object(s) to the Array and returns
  # the modified self.
  def push(*args)
    raise TypeError, "Array is frozen" if frozen?
    return self if args.empty?

    args.each { |ent| self[@total] = ent }
    self
  end 

  # Searches through contained Arrays within the Array,
  # comparing obj with the second element of each using
  # elem == obj. Returns the first matching Array, nil
  # on failure. See also Array#assoc.
  def rassoc(obj)
    # FIX: Use break when it works
    found, res = nil, nil

    each { |elem| 
      if found.nil? and elem.kind_of? Array and elem.at(1) == obj
        res, found = elem, true
      end
    }

    res
  end  
 
  # Returns a new Array by removing items from self for
  # which block is true. An Array is also returned when
  # invoked on subclasses. See #reject!
  def reject(&block)
    Array.new(self).reject!(&block) || self   
  end

  # Equivalent to #delete_if except that returns nil if
  # no changes were made.
  def reject!(&block)
    raise TypeError, "Array is frozen" if frozen?

    was = length
    self.delete_if(&block)

    self if was != length     # Too clever?
  end 

  # Replaces contents of self with contents of other,
  # adjusting size as needed.
  def replace(other)
    raise TypeError, "Array is frozen" if frozen?

    other = ary_from other

    @tuple = other.tuple.dup
    @total = other.total
    @start = other.start
    self
  end
  
  # Returns a new Array or subclass populated from self 
  # but in reverse order.
  def reverse()
    dup.reverse!
  end 
  
  # Reverses the order of elements in self. Returns self
  # even if no changes are made
  def reverse!
    raise TypeError, "Array is frozen" if frozen?

    return self unless @total > 1

    tuple = Tuple.new @total
    i = 0
    (@start + @total - 1).downto(@start) do |j|
      tuple.put i, @tuple.at(j)
      i += 1
    end
    
    @tuple = tuple
    @start = 0

    self
  end 
  
  # Goes through the Array back to front and yields 
  # each element to the supplied block. Returns self.
  def reverse_each()
    (@total - 1).downto(0) { |i| yield(at(i)) }
    self
  end

  # Returns the index of the last element in the Array
  # for which elem == obj is true.
  def rindex(obj)
    (@total - 1).downto(0) { |i| return i if at(i) == obj }
    nil
  end
  
  # Removes and returns the first element in the
  # Array or nil if empty. All other elements are
  # moved down one index.
  def shift()
    return nil if @total == 0
        
    obj = @tuple.at(@start)
    @start += 1
    @total -= 1
    
    obj
  end 

  # TODO will fix when #[]= works
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

  # Returns a new Array created by sorting elements in self. Comparisons
  # to sort are either done using <=> or a block may be provided. The
  # block must take two parameters and return -1, 0, 1 depending on 
  # whether the first parameter is smaller, equal or larger than the
  # second parameter.
  def sort(&block)
    return self.dup if self.size <= 1
    
    pivot, left, middle, right = at(0), [], [], []
    
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

    self.class.new(left + middle + right) # MRI #+ always returns Array    
  end                           

  # Sorts self in place as #sort. Array will be frozen while
  # the sorting ensues. See Array#sort for other details. 
  def sort!(&block)
    raise TypeError, "Array is frozen" if frozen?

    @sort_frozen = true     # MRI comp, must be #frozen? while #sort!ing
    res = sort(&block)
    @sort_frozen = false
    replace res
  ensure
    @sort_frozen = false
  end   

  # Returns self except on subclasses which are converted
  # or 'upcast' to Arrays.
  def to_a()
    if self.class == Array
      self
    else
      Array.new(self[0..-1])
    end
  end

  # Returns self
  def to_ary()
    self
  end

  # Produces a string by joining all elements without a 
  # separator. See #join
  def to_s()
    self.join
  end 

  # Treats all elements as being Arrays of equal lengths and
  # transposes their rows and columns so that the first contained
  # Array in the result includes the first elements of all the
  # Arrays and so on.
  def transpose()
    return [] if empty?

    out, max = [], nil

    each { |ary| 
      ary = ary_from ary
      max ||= ary.size

      # Catches too-large as well as too-small (for which #fetch would suffice)
      raise IndexError, "All arrays must be same length" if ary.size != max

      ary.size.times { |i| (out[i] ||= []) << ary.at(i) }
    }

    out
  end 

  # Returns a new Array by removing duplicate entries
  # from self. Equality is determined by using a Hash
  def uniq()
    seen, out = {}, self.class.new

    each { |elem|
      out << elem unless seen[elem]
      seen[elem] = true
    }

    out
  end

  # Removes duplicates from the Array in place as #uniq
  def uniq!()
    raise TypeError, "Array is frozen" if frozen?

    ary = uniq
    replace(ary) if size != ary.size
  end

#  # Inserts the element to the front of the Array and
#  # moves the other elements up by one index.
#  def unshift(*val)
#    raise TypeError, "Array is frozen" if frozen?
#    return self if val.empty?
#
#    self[0, 0] = val
#    self
#  end 

  # Returns a new Array populated from the elements in
  # self corresponding to the given selector(s.) The
  # arguments may be one or more integer indices or
  # Ranges. 
  def values_at(*args)
    out = []

    args.each { |elem|
      # Cannot use #[] because of subtly different errors
      if elem.kind_of? Range
        finish = elem.last
        start = elem.first  

        start, finish = int_from(start), int_from(finish)

        start += @total if start < 0
        next if start < 0

        finish += @total if finish < 0
        finish -= 1 if elem.exclude_end?
        finish = @total unless finish < @total

        next if finish < start

        start.upto(finish) { |i| out << at(i) }

      else
        i = int_from elem
        out << at(i)
      end
    }

    out
  end

  # Interleaves all given :to_ary's so that the n-th element of each
  # Array is inserted into the n-th subarray of the returned Array.
  # If a block is provided, then each subarray is passed to it 
  # instead. The maximum number of subarrays and therefore elements
  # used is the size of self. Missing indices are filled in with
  # nils and any elements past self.size in the other Arrays are
  # ignored.
  def zip(*others)
    out = Array.new(size) { [] }
    others = others.map { |ary| ary.to_a }

    size.times do |i|
      slot = out.at(i)
      slot << @tuple.at(@start + i)
      others.each { |ary| slot << ary.at(i) }
    end

    if block_given?
      out.each { |ary| yield ary }
      return nil
    end
    
    out
  end
  
  
  def size
    @total
  end

  def length
    @total
  end
  
  
  def unshift(val)
    if @start > 0
      @start -= 1
      @total += 1
      @tuple.put @start, val
      return self
    end
    
    @tuple = @tuple.shifted(1)
    
    @tuple.put 0, val
    @total += 1
    return self
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
  
  # Exactly the same as #replace but private
  def initialize_copy(other)
    @tuple = other.tuple.dup
    @total = other.total
    @start = other.start    
  end

  # Reallocates the internal Tuple to accommodate at least given size
  def reallocate(at_least)
    at_least = at_least[:at_least] if Hash === at_least
    return if at_least < @tuple.size

    factor = @total < 1_000_000 ? 2 : 1        # Does this make any sense?
    
    tuple = Tuple.new((at_least * factor) + 10)

    tuple.copy_from @tuple, @start     # Swap over old data

    @tuple = tuple
    @start = 0
  end

  # Attempt to convert the given object to_int or fail 
  def int_from(obj)
    unless obj.kind_of? Integer
      raise TypeError, "Unable to convert #{obj.inspect} to Integer" unless obj.respond_to? :to_int
      return obj.to_int
    end

    obj
  end  

  # Attempt to convert the given object to_ary or fail 
  def ary_from(obj)
    unless obj.kind_of? Array
      raise TypeError, "Unable to convert #{obj.inspect} to Array" unless obj.respond_to? :to_ary
      return obj.to_ary
    end

    obj
  end 

  # Helper to recurse through flattening since the method
  # is not allowed to recurse itself. Detects recursive structures.
  def recursively_flatten(array, out, stack)
    raise ArgumentError, "Recursive Array!" if stack.include?(array.object_id)
    stack.push array.object_id

    ret = nil

    array.each { |o|
      if o.kind_of? Array
        recursively_flatten o, out, stack 
        ret = array
      else
        out << o
      end
    }

    stack.pop                # Eep
    ret
  end

  # Helper to recurse through inspecting an Array.
  # Detects recursive structures.
  def recursively_inspect(array, stack)
    return "[...]" if stack.include?(array.object_id)
    stack.push array.object_id

    out = []
    
    array.each { |o|
      if o.kind_of? Array
        out << recursively_inspect(o, stack)
      else
        out << o.inspect
      end
    }

    stack.pop
    "[#{out.join ', '}]"
  end     

  # Helper to depth-first recurse through joining an Array
  # Detects recursive structures.
  def recursively_join(array, separator, out, stack, method)
    if stack.include?(array.object_id)
      out << separator << "[...]"
    end

    stack.push array.object_id

    array.each { |o|
      if Array === o
        recursively_join(o, separator, out, stack, method)
      else
        out << separator << o.__send__(method)
      end
    }

    stack.pop
    out
  end
end
