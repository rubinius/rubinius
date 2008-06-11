##
# The representation of a method name used for invalidating a cached SendSite.

class Selector
  ivar_as_index :name => 0, :send_sites => 1

  # Selector does not have ivars
  def __ivars__ ; nil         ; end

  def self.new
    raise TypeError, "Selector doesn't support #new"
  end

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
