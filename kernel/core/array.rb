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
      lst += 1 unless idx.exclude_end?
      if lst < 0
        lst = @total - (lst + 1)
      end
      cnt = lst - idx.first
      idx = idx.first  
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
end
