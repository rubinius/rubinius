class Array
  def total    ; @total ; end
  def tuple    ; @tuple ; end
  def start    ; @start ; end
  def __ivars__; nil    ; end

  def size
    @total
  end

  # THIS MUST NOT BE REMOVED. the kernel requires a simple
  # Array#[] to work while parts of the kernel boot.
  def [](idx)
    return nil if idx >= @total

    @tuple.at(@start + idx)
  end

  def []=(idx, ent)
    if idx >= @tuple.fields
      nt = Tuple.new(idx + 10)
      nt.copy_from @tuple, @start, 0
      @tuple = nt
    end

    @tuple.put @start + idx, ent
    if idx >= @total - 1
      @total = idx + 1
    end
    return ent
  end

  # Runtime method to support case when *foo syntax
  def __matches_when__(receiver)
    i = @start
    tot = @total + @start
    while i < tot
      return true if @tuple.at(i) === receiver
      i = i + 1
    end
    false
  end
end
