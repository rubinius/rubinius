class Array
  include Enumerable

  def initialize (sz=0, obj=nil)
#    super
    if Array === sz #.class == Array        # can we really use === here?
      # dup from sz, which is really an Array
      @tuple = Tuple.new(sz.size + 10)
      @tuple.copy_from sz.tuple, 0
      @total = sz.size
    elsif block_given?          # FIXME in class.rb: This apparently never is true!
      # fill sz times from yield
      @tuple = Tuple.new(sz + 10)
      sz.times { |i| @tuple.put i, yield }
      @total = sz
    else
      # fill sz times from obj
      @tuple = Tuple.new(sz + 10)
      sz.times { |i| @tuple.put i, obj }
      @total = sz
    end
  end

  def [](idx, cnt=nil)
    # Don't use kind_of? or === here! Both of those use Array#[] and
    # it will desend and spiral out to infinity!
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
      cnt = lst - idx
    end
    
    if cnt
      out = []
      max = idx + cnt - 1
      max = @total - 1 if max >= @total
      idx.upto(max) do |i|
        out << @tuple.at(i)
      end
      
      return nil if idx > @total || max < 0
      return out
    end
    
    if idx < 0
      idx = @total + idx
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
      cnt = lst - idx
    end

    if cnt
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

    if idx < 0
      idx += @total
      raise IndexError if idx < 0
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

  def - (ary)
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

  def | (ary)
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

  def & (ary)
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
  def slice! (*args)
    a1 = args.shift
    out = self[a1, *args]       # FIXME: a1 is needed to avoid compiler crash
    args.unshift a1

    args << 1 if args.size == 1 && !(Range === args[0])
    args << []
    self.[]=(*args)
#    self[*args] = []       # FIXME: this has mysterious bugs
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

  def self.[] (*args)
    args
  end

  def values_at (*args)
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

  def indexes (*args)
    out = []
    args.each { |x|
      out << self[x]
    }
    out
  end

  alias indices indexes

  def transpose
    out = []
    a1 = shift
    a1.zip(*self) { |x| out << x }
    out
  end

  def sort (&block)
    if size <= 1
      self.dup
    else
      mid = size/2
      midval = self[mid]
      al, am, ar = [], [midval], []
      if block_given?
        each_with_index { |o, i|
          if i != mid
            cmp = yield(o, midval)
            if cmp < 0; al << o
            elsif cmp == 0; am << o
            else; ar << o
            end
          end
        }
        out = al.sort(&block)
        out.concat am
        out.concat(ar.sort(&block))
      else
        each_with_index { |o1, i1|
          if i1 != mid
            cmp = o1 <=> midval
            if cmp < 0; al << o1
            elsif cmp == 0; am << o1
            else; ar << o1
            end
          end
        }
        out = al.sort
        out.concat am
        out.concat(ar.sort)
      end
      out
    end
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
        out.push *self
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
end
