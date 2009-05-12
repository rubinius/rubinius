# These methods are overriden by lib/1.8.7/... or lib/1.9/...
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

  alias_method :map, :collect

  ##
  # :call-seq:
  #   enum.each_with_index { |obj, i| block }  -> enum
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
    idx = 0
    each { |o| yield(o, idx); idx += 1 }
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
  #   enum.partition { | obj | block }  => [ true_array, false_array ]
  #
  # Returns two arrays, the first containing the elements of +enum+ for which
  # the block evaluates to true, the second containing the rest.
  #
  #   (1..6).partition { |i| (i&1).zero?}   #=> [[2, 4, 6], [1, 3, 5]]

  def partition
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
    ary = []
    each do |o|
      unless yield(o)
        ary << o
      end
    end
    ary
  end

end