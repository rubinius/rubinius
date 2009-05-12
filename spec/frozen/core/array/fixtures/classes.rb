module ArraySpecs
  not_compliant_on :rubinius do
    def self.max_32bit_size
      2**32/4
    end

    def self.max_64bit_size
      2**64/8
    end
  end

  deviates_on :rubinius do
    def self.max_32bit_size
      2**30-1
    end

    def self.max_64bit_size
      2**62-1
    end
  end

  def self.frozen_array
    frozen_array = [1,2,3]
    frozen_array.freeze
    frozen_array
  end

  def self.recursive_array
    a = [1, 'two', 3.0]
    5.times { a << a }
    a
  end

  def self.head_recursive_array
    a =  []
    5.times { a << a }
    a << 1 << 'two' << 3.0
    a
  end

  def self.empty_recursive_array
    a = []
    a << a
    a
  end

  class MyArray < Array; end

  class Sexp < Array
    def initialize(*args)
      super(args)
    end
  end

  # TODO: replace specs that use this with #should_not_receive(:to_ary)
  # expectations on regular objects (e.g. Array instances).
  class ToAryArray < Array
    def to_ary() ["to_ary", "was", "called!"] end
  end

  class MyRange < Range; end

  class AssocKey
    def ==(other); other == 'it'; end
  end

  class D
    def <=>(obj)
      return 4 <=> obj unless obj.class == D
      0
    end
  end

  class SubArray < Array
    def initialize(*args)
      ScratchPad.record args
    end
  end

  class ArrayConvertable
    attr_accessor :called
    def initialize(*values, &block)
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

  class ArrayNotReallyConvertable
    def to_ary
      raise "Oups"
    end
  end

end
