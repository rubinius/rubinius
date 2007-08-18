class Tuple
  def self.new(cnt)
    Ruby.primitive :allocate_count
  end
  
  def shifted(distance)
    Ruby.primitive :tuple_shifted
  end
  
  def [](idx)
    Ruby.primitive :at
    raise InvalidIndex, "Unable to access index '#{idx}' of #{self}"
  end

  def []=(idx, val)
    Ruby.primitive :put
    raise InvalidIndex, "Unable to access index '#{idx}' of #{self}"
  end
  
end