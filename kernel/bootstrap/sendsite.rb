class SendSite

  def at(index)
    Ruby.primitive :sendsite_at
  end

  def name
    at(0)
  end

  def selector
    at(1)
  end

  def hits
    at(6)
  end

  def misses
    at(7)
  end

  def data(which)
    at(2 + which)
  end

  def inspect
    "#<SendSite:0x#{object_id.to_s(16)} name=#{name} hits=#{hits} misses=#{misses}>"
  end
end
