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
    } || nil
  end

  # FRED
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

  alias indexes values_at
  alias indices values_at

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
end
