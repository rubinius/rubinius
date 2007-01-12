class Array
  include Enumerable
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

  def []=(idx, ent)
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
