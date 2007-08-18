class Array
  
  def size
    @total
  end
        
  # THIS MUST NOT BE REMOVED. the kernel requires a simple
  # Array#[] to work while parts of the kernel boot.
  def [](idx)    
    if idx >= @total
      return nil
    end

    @tuple.at(idx)
  end
  
  def []=(idx, ent)
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
end
