class Array
  include Enumerable

  def initialize(sz=0, obj=nil)
    #    super
    if Array === sz #.class == Array        # can we really use === here?
      # dup from sz, which is really an Array
      @tuple = Tuple.new(sz.size + 10)
      @tuple.copy_from sz.tuple, 0
      @total = sz.size
    elsif sz.respond_to? :to_ary
      @tuple = Tuple.new(sz.to_ary.size + 10)
      @tuple.copy_from sz.to_ary.tuple, 0
      @total = sz.to_ary.size
    elsif block_given?          # FIXME in class.rb: This apparently never is true!
      # fill sz times from yield
      @tuple = Tuple.new(sz + 10)
      sz.times { |i| @tuple.put i, yield(i) }
      @total = sz
    else
      # fill sz times from obj
      @tuple = Tuple.new(sz + 10)
      sz.times { |i| @tuple.put i, obj }
      @total = sz
    end
  end

  # Synonymous to #replace
  alias initialize_copy replace


  def [](idx, cnt=nil)
    # Don't use kind_of? or === here! Both of those use Array#[] and
    # it will descend and spiral out to infinity!

    # Normalise Range arguments
    if idx.class == Range
      if cnt
        raise ArgumentError, "Second argument invalid with a range"
      end

      # Extract the count
      lst = idx.last
      lst += @total if lst < 0
      return nil if lst < 0

      lst += 1 unless idx.exclude_end?

      idx = idx.first
      idx += @total if idx < 0
      return nil if idx < 0

      return [] if lst == 0

      cnt = lst - idx
    end

    if idx < 0
      idx += @total
    end

    if idx < 0
      return nil
    end

    if cnt
      return nil if cnt < 0
      return [] if cnt == 0

      out = []
      max = idx + cnt - 1
      max = @total - 1 if max >= @total
      idx.upto(max) do |i|
        out << @tuple.at(i)
      end

      return nil if idx > @total || max < 0
      return out
    end

    if idx < 0 || idx >= @total
      return nil
    end

    @tuple.at(idx)
  end

  def at(idx)
    if idx < 0
      idx += @total
      return nil if idx < 0
    end

    if idx >= @total
      return nil
    end

    @tuple.at(idx)
  end

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
  
  def join(sep = nil, meth=:to_s)
    str = ""
    return str if @total == 0
    sep = $, unless sep
    tuple.join_upto(sep, @total, meth)
  end

  def each_index
    i = 0
    while i < @total
      yield i
      i += 1
    end
    self
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
      idx = @total - idx
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

  def assoc(obj)
    find { |x|
      Array === x && x.first == obj
    }
  end

  def rassoc(obj)
    find { |x|
      Array === x && x[1] == obj
    }
  end

  def -(ary)
    set_exclude = { }
    out = []
    ary.each {  |x|
      set_exclude[x] = true
    }
    each {  |x|
      unless set_exclude[x]
        out << x
        #        set_exclude[x] = true    No!
      end
    }
    out
  end

  def |(ary)
    set_exclude = { }
    out = []
    each {  |x|
      unless set_exclude[x]
        out << x
        set_exclude[x] = true
      end
    }
    ary.each {  |x|
      unless set_exclude[x]
        out << x
        set_exclude[x] = true
      end
    }
    out
  end

  def &(ary)
    set_include = { }
    out = []
    ary.each {  |x|
      set_include[x] = true
    }
    each {  |x|
      if set_include[x]
        out << x
        set_include[x] = false
      end
    }
    out
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
      else
        i += 1
      end
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
    args
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

  # sort_without_block has a separate implementation because block args (&block)
  # have additional overhead, and this quicksort algorithm uses them recursively.
  # sort_without_block could be: sort_with_block {|a,z| a <=> z}, but this is faster
  def sort(&block)
    return self.dup if self.size <= 1
    array = self.dup
    if block_given?
      array.sort_with_block(&block)
    else
      array.sort_without_block
    end
  end

  def sort_without_block
    return self if size <= 1
    # Breaks amd64, FFI?
#    pivot = self[rand(size)] # Random pivot point
    pivot = self.first

    # Array#sort uses the first element of an array to perform the sort
    # In particular, Hash#sort uses this behavior
    pivot = pivot[0] if Array === pivot
    left = select   { |x| (Array === x ? x[0] : x) < pivot }.sort_without_block 
    middle = select { |x| (Array === x ? x[0] : x) == pivot }
    right = select  { |x| (Array === x ? x[0] : x) > pivot }.sort_without_block
    left + middle + right
  end

  def sort_with_block(&block)
    return self if size <= 1

    # Breaks amd64, FFI?
#    pivot = self[rand(size)] # Random pivot point
    pivot = self.first

    left = select do |x|
      (yield(x,pivot) || 0) < 0
    end

    middle = select do |x|
      (yield(x,pivot) || 0) == 0 # Treat nil block results as pre-sorted
    end

    right = select do |x| 
      (yield(x,pivot) || 0) > 0 
    end

    # If the block claims that none of the elements are the pivot, ignore it
    return self.sort if middle.empty?
      
    left.sort_with_block(&block) + middle + right.sort_with_block(&block)
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

  # TODO fill out pack.
  def pack(schema)
    if schema.size != self.size
      raise ArgumentError, "Schema includes #{schema.size} conversions, array only has #{self.size} elements."
    end
    
    ret = ""
    0.upto(schema.size - 1) do |idx|
      kind = schema[idx]
      if kind == ?N
        obj = self[idx]
        parts = []
        4.times do
          parts << (obj % 256)
          obj = obj / 256
        end
        3.downto(0) do |j|
          ret << parts[j].chr
        end
      else
        raise ArgumentError, "Unknown kind #{kind.chr}"
      end  
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
  
  def -(other)
    out = []
    each { |e| 
      unless other.include?(e)
        out << e 
      end
    }
    return out
  end
  
  def &(other)    
    out=[]
    self.uniq!
    each { |e| 
      if other.include?(e)
        out << e 
      end
    }
    return out
  end
  
  # TODO: test val for negative values and floats 
  def *(val)
    if val.class == String
      return self.join(val)
    else
      out=[]
      val.times do
        out.push(*self)
      end
      return out
    end
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
	  pos = self.length;
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
end
