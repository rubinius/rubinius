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

  def max_by()
    max_obj, max_value = Undefined, Undefined

    each do |o|
      value = yield(o)

      if max_obj.equal?(Undefined) or (max_value <=> value) < 0
        max_obj, max_value = o, value
      end
    end

    max_obj.equal?(Undefined) ? nil : max_obj
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

  def min_by()
    min_obj, min_value = Undefined, Undefined

    each do |o|
      value = yield(o)

      if min_obj.equal?(Undefined) or (min_value <=> value) > 0
        min_obj, min_value = o, value
      end
    end

    min_obj.equal?(Undefined) ? nil : min_obj
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

end