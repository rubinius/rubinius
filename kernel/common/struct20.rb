# -*- encoding: us-ascii -*-

class Struct
  def to_h
    Hash[each_pair.to_a]
  end
end
