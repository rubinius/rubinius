module Enumerable
  ##
  # :call-seq:
  #   enum.count(item)             => int
  #   enum.count { | obj | block } => int
  #
  # Returns the number of items in +enum+ for which equals to +item+. If a
  # block is given, counts the number of elements yielding a true value.
  #
  #   ary = [1, 2, 4, 2]
  #   ary.count(2)          # => 2
  #   ary.count{ |x|x%2==0}  # => 3

  def count(item = Undefined)
    seq = 0
    if ! item.equal? Undefined
      each { |o| seq += 1 if item == o }
    elsif block_given?
      each { |o| seq += 1 if yield(o) }
    else
      each { seq += 1 }
    end
    seq
  end

  ##
  # :call-seq:
  #   enum.cycle(n = nil){ | obj | block } => nil or enumerator
  #
  # Calls block for each element of enum repeatedly n times or forever if none
  # or nil is given. If a non-positive number is given or the collection is empty,
  # does nothing. Returns nil if the loop has finished without getting interrupted.
  #
  # Enumerable#cycle saves elements in an internal array so changes to enum after the first pass have no effect.
  #
  #   a = ["a", "b", "c"]
  #   a.cycle {|x| puts x }  # prints a, b, c, a, b, c,.. forever.
  #   a.cycle(2) {|x| puts x }  # prints a, b, c, a, b, c.

  def cycle(n = nil, &block)
    return to_enum :cycle, n unless block_given?
    return loop(&block) if nil == n
    n = Type.coerce_to(n, Fixnum, :to_int)
    if n >= 1
      cache = []
      each do |elem|
        cache << elem
        block.call(elem)
      end
      cache.cycle(n-1, &block)
    end
  end

  ##
  # :call-seq:
  #   enum.drop(n)   => an_array
  #
  # Drops first n elements from enum, and returns rest elements in an array.
  
  def drop(n)
    n = Type.coerce_to(n, Fixnum, :to_int)
    raise ArgumentError, "attempt to drop negative size" if n < 0
    ary = to_a
    ary[n...ary.size]
  end

  ##
  # :call-seq:
  #   enum.drop_while {|obj| block } => array
  #
  # Drops elements up to, but not including, the first element for which the block
  # returns nil or false and returns an array containing the remaining elements.
  
  def drop_while
    return to_enum :drop_while unless block_given?
    ary = []
    dropping = true
    each do |obj|
      ary << obj unless dropping &&= yield(obj)
    end
    ary
  end

  def each_cons(num)
    n = Type.coerce_to(num, Fixnum, :to_int)
    raise ArgumentError, "invalid size: #{n}" if n <= 0
    return to_enum :each_cons, num unless block_given?
    array = []
    each do |element|
      array << element
      array.shift if array.size > n
      yield array.dup if array.size == n
    end
    nil
  end

  def each_slice(slice_size)
    n = Type.coerce_to(slice_size, Fixnum, :to_int)
    raise ArgumentError, "invalid slice size: #{n}" if n <= 0
    return to_enum :each_slice, slice_size unless block_given?
    a = []
    each do |element|
      a << element
      if a.length == n
        yield a
        a = []
      end
    end
    yield a unless a.empty?
    nil
  end

  ##
  # :call-seq:
  #   enum.each_with_index(*arg){ |obj, i| block }  -> enum or enumerator
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

  def each_with_index(*arg)
    return to_enum :each_with_index, *arg unless block_given?
    idx = 0
    each(*arg) { |o| yield(o, idx); idx += 1 }
    self
  end

  ##
  # :call-seq:
  #   enum.detect(ifnone = nil) { | obj | block }  => obj or nil
  #   enum.find(ifnone = nil)   { | obj | block }  => obj or nil
  #
  # Passes each entry in +enum+ to +block+>. Returns the first for which
  # +block+ is not false.  If no object matches, calls +ifnone+ and returns
  # its result when it is specified, or returns nil
  #
  #   (1..10).detect  { |i| i % 5 == 0 and i % 7 == 0 }   #=> nil
  #   (1..100).detect { |i| i % 5 == 0 and i % 7 == 0 }   #=> 35

  def find(ifnone = nil)
    return to_enum :find, ifnone unless block_given?
    each { |o| return o if yield(o) }
    ifnone.call if ifnone
  end

  alias_method :detect, :find

  ##
  # :call-seq:
  #   enum.find_all { | obj | block }  => array
  #   enum.select   { | obj | block }  => array
  #
  # Returns an array containing all elements of +enum+ for which +block+ is
  # not false (see also Enumerable#reject).
  #
  #   (1..10).find_all { |i|  i % 3 == 0 }   #=> [3, 6, 9]

  def find_all
    return to_enum :find_all unless block_given?
    ary = []
    each do |o|
      if yield(o)
        ary << o
      end
    end
    ary
  end

  alias_method :select, :find_all

  ##
  # :call-seq:
  #   enum.find_index(value) => int
  #   enum.find_index{|elem| block } => int
  #   enum.find_index => enumerator
  #
  # Compares each entry in enum with value or passes to block.
  # Returns the index for the first for which the evaluated value is non-false. If no object matches, returns nil

  def find_index(value = Undefined)
    if value.equal? Undefined
      return to_enum :find_index unless block_given?
      each_with_index do |element, i|
        return i if yield element
      end
    else
      each_with_index do |element, i|
        return i if element == value
      end
    end
    nil
  end

  ##
  # :call-seq:
  #   enum.first      => obj or nil
  #   enum.first(n)   => an_array
  #
  # Returns the first element, or the first +n+ elements, of the enumerable.
  # If the enumerable is empty, the first form returns nil, and the second
  # form returns an empty array.

  def first(n = Undefined)
    return take(n) unless n.equal?(Undefined)
    each{|obj| return obj}
    nil
  end

  ##
  # :call-seq:
  #   enum.group_by { | obj | block }  => a_hash
  #
  # Returns a hash, which keys are evaluated result from the block, and values
  # are arrays of elements in +enum+ corresponding to the key.
  #
  #    (1..6).group_by { |i| i%3}   #=> {0=>[3, 6], 1=>[1, 4], 2=>[2, 5]}

  def group_by
    return to_enum :group_by unless block_given?
    h = {}
    i = 0
    each do |o|
      key = yield(o)
      if h.key?(key)
        h[key] << o
      else
        h[key] = [o]
      end
    end
    h
  end

  ##
  # :call-seq:
  #   enum.reduce(initial, sym) => obj
  #   enum.reduce(sym)          => obj
  #   enum.inject(initial) { | memo, obj | block }  => obj
  #   enum.inject          { | memo, obj | block }  => obj
  #
  # Combines all elements of enum by applying a binary operation,
  # specified by a block or a symbol that names a method or operator.
  # 
  # If you specify a block, then for each element in enum<i> the block
  # is passed an accumulator value (<i>memo) and the element.
  # If you specify a symbol instead, then each element in the collection
  # will be passed to the named method of memo. In either case,
  # the result becomes the new value for memo. At the end of the
  # iteration, the final value of memo is the return value fo the method.
  # 
  # If you do not explicitly specify an initial value for memo,
  # then uses the first element of collection is used as the initial value of memo.
    
  def inject(initial= Undefined, sym = Undefined, &block)
    unless block_given? && sym.equal?(Undefined)
      initial, sym = Undefined, initial if sym.equal?(Undefined)
      block = Proc.new{|memo, obj| memo.send(sym, obj) }
    end
    each do |o|
      if initial.equal? Undefined
        initial = o
      else
        initial = block.call(initial, o)
      end
    end
    initial.equal?(Undefined) ? nil : initial
  end
  alias_method :reduce, :inject

  ##
  # :call-seq:
  #   enum.max_by { | obj| block }   => obj
  #
  # Uses the values returned by the given block as a substitute for the real
  # object to determine what is considered the largest object in +enum+ using
  # <tt>lhs <=> rhs</tt>. In the event of a tie, the object that appears first
  # in #each is chosen. Returns the "largest" object or nil if the enum is
  # empty.
  #
  #   a = %w[albatross dog horse]
  #   a.max_by { |x| x.length }   #=> "albatross"
  
  def max_by(&block)
    return to_enum(:max_by) unless block_given?
    max_object, max_result = nil, Rubinius::FakeComparator.new(-1)
    each do |object|
      result = yield object
      max_object, max_result = object, result if Type.coerce_to_comparison(max_result, result) < 0
    end
    max_object
  end
  
  ##
  # :call-seq:
  #   enum.min_by { |obj| block }   => obj
  #
  # Uses the values returned by the given block as a substitute for the real
  # object to determine what is considered the smallest object in +enum+ using
  # <tt>lhs <=> rhs</tt>. In the event of a tie, the object that appears first
  # in #each is chosen. Returns the "smallest" object or nil if the enum is
  # empty.
  #
  #   a = %w[albatross dog horse]
  #   a.min_by { |x| x.length }   #=> "dog"
  
  def min_by
    return to_enum(:min_by) unless block_given?
    min_object, min_result = nil, Rubinius::FakeComparator.new(1)
    each do |object|
      result = yield object
      min_object, min_result = object, result if Type.coerce_to_comparison(min_result, result) > 0
    end
    min_object
  end
  
  # Returns two elements array which contains the minimum and the maximum value
  # in the enumerable. The first form assumes all objects implement Comparable;
  # the second uses the block to return a <=> b.
  
  def minmax(&block)
    block = Proc.new{|a,b| a <=> b} unless block_given?
    first_time = true
    min, max = nil
    each do |object|
      if first_time
        min = max = object
        first_time = false
      else
        min = object if Type.coerce_to_comparison(min, object, block.call(min, object)) > 0
        max = object if Type.coerce_to_comparison(max, object, block.call(max, object)) < 0
      end
    end
    [min, max]
  end
  
  # Returns two elements array array containing the objects in enum that
  # gives the minimum and maximum values respectively from the given block. 
  
  def minmax_by(&block)
    return to_enum(:minmax_by) unless block_given?
    min_object, min_result = nil, Rubinius::FakeComparator.new(1)
    max_object, max_result = nil, Rubinius::FakeComparator.new(-1)
    each do |object|
      result = yield object
      min_object, min_result = object, result if Type.coerce_to_comparison(min_result, result) > 0
      max_object, max_result = object, result if Type.coerce_to_comparison(max_result, result) < 0
    end
    [min_object, max_object]
  end

  ##
  # :call-seq:
  #   enum.none?                   => true or false
  #   enum.none? { |obj| block }   => true or false
  #
  # Passes each element of the collection to the given block. The method
  # returns true if the block never returns true for all elements. If the
  # block is not given, none? will return true only if any of the collection
  # members is true.
  #
  #    %w{ant bear cat}.none? { |word| word.length == 4}   #=> true
  #    %w{ant bear cat}.none? { |word| word.length >= 4}   #=> false
  #    [ nil, true, 99 ].none?                             #=> true

  def none?(&prc)
    if block_given?
      each { |o| return false if yield(o) }
    else
      each { |o| return false if o }
    end
    return true
  end

  ##
  # :call-seq:
  #   enum.one?                   => true or false
  #   enum.one? { |obj| block }   => true or false
  #
  # Passes each element of the collection to the given block. The method
  # returns true if the block returns true exactly once. If the block is not
  # given, one? will return true only if exactly one of the collection members
  # are true.
  #
  #   %w[ant bear cat].one? { |word| word.length == 4}   #=> true
  #   %w[ant bear cat].one? { |word| word.length >= 4}   #=> false
  #   [ nil, true, 99 ].one?                             #=> true

  def one?(&prc)
    found_one = false
    if block_given?
      each do |o|
        if yield(o)
          return false if found_one
          found_one = true
        end
      end
    else
      each do |o|
        if o
          return false if found_one
          found_one = true
        end
      end
    end
    found_one
  end

  ##
  # :call-seq:
  #   enum.partition { | obj | block }  => [ true_array, false_array ]
  #
  # Returns two arrays, the first containing the elements of +enum+ for which
  # the block evaluates to true, the second containing the rest.
  #
  #   (1..6).partition { |i| (i&1).zero?}   #=> [[2, 4, 6], [1, 3, 5]]

  def partition
    return to_enum :partition unless block_given?
    left = []
    right = []
    each { |o| yield(o) ? left.push(o) : right.push(o) }
    return [left, right]
  end

  ##
  # :call-seq:
  #   enum.reject { | obj | block }  => array
  #
  # Returns an array for all elements of +enum+ for which +block+ is false
  # (see also Enumerable#find_all).
  #
  #    (1..10).reject { |i|  i % 3 == 0 }   #=> [1, 2, 4, 5, 7, 8, 10]

  def reject
    return to_enum :reject unless block_given?
    ary = []
    each do |o|
      unless yield(o)
        ary << o
      end
    end
    ary
  end

  ##
  # :call-seq:
  #   enum.take(n)                => array
  #
  # Returns first n elements from enum.

  def take(n)
    n = Type.coerce_to(n, Fixnum, :to_int)
    raise ArgumentError, "attempt to take negative size: #{n}" if n < 0
    array = []
    each do |elem|
      break if array.size >= n
      array << elem
    end unless n <= 0
    array
  end

  # Passes elements to the block until the block returns nil or false,
  # then stops iterating and returns an array of all prior elements.
  
  def take_while
    return to_enum :take_while unless block_given?
    array = []
    each do |elem|
      return array unless yield elem
      array << elem
    end
    array
  end
end

module Rubinius
  class FakeComparator
    def initialize(comparison_result)
      @comparison_result = comparison_result
    end
    
    def <=>(whatever)
      @comparison_result
    end
  end
end