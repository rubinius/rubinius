##
# The representation of a method name used for invalidating a cached SendSite.

class Selector

  def name
    @name
  end

  def inspect
    "#<Selector name=#{@name} sites=#{@send_sites.size}>"
  end

  def send_sites
    @send_sites
  end

  def receives
    @send_sites.inject(0) { |acc, ss| acc + ss.hits + ss.misses }
  end
end
