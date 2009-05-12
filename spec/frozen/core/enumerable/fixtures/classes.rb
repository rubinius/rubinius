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

  class EachCounter < Numerous
    attr_reader :times_called, :times_yielded, :arguments_passed
    def initialize(*list)
      super(*list)
      @times_yielded = @times_called = 0
    end
    
    def each(*arg)
      @times_called += 1
      @times_yielded = 0
      @arguments_passed = arg
      @list.each do |i|
        @times_yielded +=1 
        yield i
      end
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
    
    def to_ary
      self.called = :to_ary
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

  class YieldsMulti
    include Enumerable
    def each
      yield 1,2
      yield 3,4,5
      yield 6,7,8,9
    end
  end

  class ReverseComparable
    include Comparable
    def initialize(num)
      @num = num
    end

    attr_accessor :num

    # Reverse comparison
    def <=>(other)
      other.num <=> @num
    end
  end
  
  class ComparableWithFixnum
    include Comparable
    def initialize(num)
      @num = num
    end

    def <=>(fixnum)
      @num <=> fixnum
    end
  end

end # EnumerableSpecs utility classes
