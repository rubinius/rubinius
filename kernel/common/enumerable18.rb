module Enumerable
  ##
  # :call-seq:
  #   enum.collect { | obj | block }  => array
  #   enum.map     { | obj | block }  => array
  #
  # Returns a new array with the results of running +block+ once for every
  # element in +enum+.
  #
  #   (1..4).collect { |i| i*i }   #=> [1, 4, 9, 16]
  #   (1..4).collect { "cat"  }   #=> ["cat", "cat", "cat", "cat"]

  def collect
    if block_given?
      ary = []
      each { |o| ary << yield(o) }
      ary
    else
      to_a
    end
  end

  ##
  # :call-seq:
  #   enum.each_with_index { |obj, i| block }  -> enum or enumerator
  #
  # Calls +block+ with two arguments, the item and its index, for
  # each item in +enum+.
  #
  #   hash = {}
  #   %w[cat dog wombat].each_with_index { |item, index|
  #     hash[item] = index
  #   }
  #
  #   p hash   #=> {"cat"=>0, "wombat"=>2, "dog"=>1}

  def each_with_index
    return to_enum(:each_with_index) unless block_given?

    idx = 0
    each do |o|
      yield o, idx
      idx += 1
    end

    self
  end

  alias_method :map, :collect
  alias_method :enum_cons, :each_cons
  alias_method :enum_slice, :each_slice
  alias_method :enum_with_index, :each_with_index

  ##
  # :call-seq:
  #    enum.zip(arg, ...)                   => array
  #    enum.zip(arg, ...) { |arr| block }   => nil
  #
  # Converts any arguments to arrays, then merges elements of +enum+ with
  # corresponding elements from each argument. This generates a sequence of
  # enum#size +n+-element arrays, where +n+ is one more that the count of
  # arguments. If the size of any argument is less than enum#size, nil values
  # are supplied. If a block given, it is invoked for each output array,
  # otherwise an array of arrays is returned.
  #
  #   a = [ 4, 5, 6 ]
  #   b = [ 7, 8, 9 ]
  #
  #   (1..3).zip(a, b)      #=> [[1, 4, 7], [2, 5, 8], [3, 6, 9]]
  #   "cat\ndog".zip([1])   #=> [["cat\n", 1], ["dog", nil]]
  #   (1..3).zip            #=> [[1], [2], [3]]

  def zip(*args)
    args.map! { |a| a.to_a }

    results = []

    each_with_index do |o, i|
      entry = args.inject([o]) { |ary, a| ary << a[i] }

      yield entry if block_given?

      results << entry
    end

    return nil if block_given?
    results
  end

end
