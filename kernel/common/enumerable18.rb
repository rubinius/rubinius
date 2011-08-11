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
  alias_method :enum_cons, :each_cons
  alias_method :enum_slice, :each_slice
  alias_method :enum_with_index, :each_with_index
end
