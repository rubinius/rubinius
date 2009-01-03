module EnumerableSpecs

  class Numerous
    include Enumerable
    
    def initialize(*list)
      @list = list.empty? ? [2, 5, 3, 6, 1, 4] : list
    end
    
    def each
      @list.each { |i| yield i }
    end      
  end

  class Empty
    include Enumerable
    def each
    end
  end

  class ThrowingEach
    include Enumerable
    def each
      raise "from each"
    end
  end

  class NoEach
    include Enumerable
  end

  # (Legacy form rubycon)
  class EachDefiner

    include Enumerable

    attr_reader :arr

    def initialize(*arr)
      @arr = arr
    end
    
    def each
      i = 0
      loop do
        break if i == @arr.size
        yield @arr[i]
        i += 1
      end
    end

  end 

  class SortByDummy
    def initialize(s)
      @s = s
    end

    def s
      @s
    end
  end

  class ComparesByVowelCount
    
    attr_accessor :value, :vowels
    
    def self.wrap(*args)
      args.map {|element| ComparesByVowelCount.new(element)}
    end
    
    def initialize(string)
      self.value = string
      all_vowels = ['a', 'e' , 'i' , 'o', 'u']
      self.vowels = string.gsub(/[^aeiou]/,'').size
    end
    
    def <=>(other)
      self.vowels <=> other.vowels
    end
    
  end

  class InvalidComparable
    def <=>(other)
      "Not Valid"
    end
  end

  class ArrayConvertable
    attr_accessor :called
    def initialize(*values)
      @values = values;
    end
    
    def to_a
      self.called = :to_a
      @values
    end
    
    def to_arr
      self.called = :to_arr
      @values
    end
  end

  class Equals
    def initialize(obj)
      @obj = obj
    end
    def ==(other)
      @obj == other
    end
  end

end # EnumerableSpecs utility classes
