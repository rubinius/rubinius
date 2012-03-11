# -*- encoding: us-ascii -*-

module Enumerable
  def collect
    if block_given?
      ary = []
      each { |o| ary << yield(o) }
      ary
    else
      to_a
    end
  end

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

  def zip(*args)
    args.map! { |a| a.to_a }

    results = []
    i = 0
    each do
      o = Rubinius.single_block_arg
      entry = args.inject([o]) { |ary, a| ary << a[i] }

      yield entry if block_given?

      results << entry
      i += 1
    end

    return nil if block_given?
    results
  end

end
