module ArraySpecs
  def self.frozen_array
    @frozen_array ||= [1,2,3]
    @frozen_array.freeze
    @frozen_array
  end
end

class MyArray < Array; end
class ToAryArray < Array
  def to_ary() ["to_ary", "was", "called!"] end
end

class MyRange < Range; end

module ArraySpecs
  class AssocKey
    def ==(other); other == 'it'; end
  end
end

class D 
  def <=>(obj) 
    return 4 <=> obj unless obj.class == D
    0
  end
end
