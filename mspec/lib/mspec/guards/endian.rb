require 'mspec/guards/guard'

# Despite that these are inverses, the two classes are
# used to simplify MSpec guard reporting modes

class BigEndianGuard < SpecGuard
  def pattern
    [1].pack('L')
  end
  private :pattern

  def match?
    pattern[-1] == ?\001
  end
end

class LittleEndianGuard < SpecGuard
  def pattern
    [1].pack('L')
  end
  private :pattern

  def match?
    pattern[-1] == ?\000
  end
end

class Object
  def big_endian
    g = BigEndianGuard.new
    yield if g.yield?
    g.unregister
  end

  def little_endian
    g = LittleEndianGuard.new
    yield if g.yield?
    g.unregister
  end
end
