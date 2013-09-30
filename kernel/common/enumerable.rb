# Just to save you 10 seconds, the reason we always use #each to extract
# elements instead of something simpler is because Enumerable can not assume
# any other methods than #each. If needed, class-specific versions of any of
# these methods can be written *in those classes* to override these.

module Enumerable
  def chunk(initial_state = nil, &original_block)
    raise ArgumentError, "no block given" unless block_given?
    ::Enumerator.new do |yielder|
      previous = nil
      accumulate = []
      block = if initial_state.nil?
        original_block
      else
        duplicated_initial_state = initial_state.dup
        Proc.new{ |val| original_block.yield(val, duplicated_initial_state)}
      end
      each do |val|
        key = block.yield(val)
        if key.nil? || (key.is_a?(Symbol) && key.to_s[0, 1] == "_")
          yielder.yield [previous, accumulate] unless accumulate.empty?
          accumulate = []
          previous = nil
          case key
          when nil, :_separator
          when :_alone
            yielder.yield [key, [val]]
          else
            raise RuntimeError, "symbols beginning with an underscore are reserved"
          end
        else
          if previous.nil? || previous == key
            accumulate << val
          else
            yielder.yield [previous, accumulate] unless accumulate.empty?
            accumulate = [val]
          end
          previous = key
        end
      end
      yielder.yield [previous, accumulate] unless accumulate.empty?
    end
  end

  def collect
    if block_given?
      ary = []
      each do |*o|
        ary << yield(*o)
      end
      ary
    else
      to_enum :collect
    end
  end

  alias_method :map, :collect

  def count(item = undefined)
    seq = 0
    if !undefined.equal?(item)
      each { |o| seq += 1 if item == o }
    elsif block_given?
      each { |o| seq += 1 if yield(o) }
    else
      each { seq += 1 }
    end
    seq
  end

  def each_entry(*pass)
    return to_enum :each_entry, *pass unless block_given?
    each(*pass) do |*args|
      yield args.size == 1 ? args[0] : args
    end
    self
  end

  def each_with_object(memo)
    return to_enum :each_with_object, memo unless block_given?
    each do
      obj = Rubinius.single_block_arg
      yield obj, memo
    end
    memo
  end

  alias_method :with_object, :each_with_object

  def flat_map(&block)
    return to_enum(:flat_map) unless block_given?
    map(&block).flatten(1)
  end
  alias_method :collect_concat, :flat_map

  def lazy
    Enumerator::Lazy.new(self, enumerator_size) do |yielder, *args|
      yielder.<<(*args)
    end
  end

  def enumerator_size
    Rubinius::Type.object_respond_to?(self, :size) ? size : nil
  end
  private :enumerator_size

  def group_by
    return to_enum(:group_by) unless block_given?

    h = {}
    each do
      o = Rubinius.single_block_arg
      key = yield(o)
      if h.key?(key)
        h[key] << o
      else
        h[key] = [o]
      end
    end
    Rubinius::Type.infect h, self
    h
  end

  def slice_before(arg = undefined, &block)
    if block_given?
      has_init = !(undefined.equal? arg)
    else
      raise ArgumentError, "wrong number of arguments (0 for 1)" if undefined.equal? arg
      block = Proc.new{ |elem| arg === elem }
    end
    Enumerator.new do |yielder|
      init = arg.dup if has_init
      accumulator = nil
      each do |elem|
        start_new = has_init ? block.yield(elem, init) : block.yield(elem)
        if start_new
          yielder.yield accumulator if accumulator
          accumulator = [elem]
        else
          accumulator ||= []
          accumulator << elem
        end
      end
      yielder.yield accumulator if accumulator
    end
  end

  def to_a(*arg)
    ary = []
    each(*arg) do
      o = Rubinius.single_block_arg
      ary << o
      nil
    end
    Rubinius::Type.infect ary, self
    ary
  end
  alias_method :entries, :to_a

  def zip(*args)
    args.map! do |a|
      if a.respond_to? :to_ary
        a.to_ary
      else
        a.to_enum(:each)
      end
    end

    results = []
    i = 0
    each do
      o = Rubinius.single_block_arg
      entry = args.inject([o]) do |ary, a|
        ary << case a
               when Array
                 a[i]
               else
                 begin
                   a.next
                 rescue StopIteration
                   nil
                 end
               end
      end

      yield entry if block_given?

      results << entry
      i += 1
    end

    return nil if block_given?
    results
  end

  def each_with_index(*args)
    return to_enum(:each_with_index, *args) unless block_given?

    idx = 0
    each(*args) do
      o = Rubinius.single_block_arg
      yield o, idx
      idx += 1
    end

    self
  end

  def grep(pattern)
    ary = []

    if block_given?
      each do
        o = Rubinius.single_block_arg
        if pattern === o
          Regexp.set_block_last_match
          ary << yield(o)
        end
      end
    else
      each do
        o = Rubinius.single_block_arg
        if pattern === o
          Regexp.set_block_last_match
          ary << o
        end
      end
    end

    ary
  end

  def sort(&prc)
    to_a.sort!(&prc)
  end

  class SortedElement
    def initialize(val, sort_id)
      @value, @sort_id = val, sort_id
    end

    attr_reader :value
    attr_reader :sort_id

    def <=>(other)
      @sort_id <=> other.sort_id
    end
  end

  def sort_by
    return to_enum :sort_by unless block_given?

    # Transform each value to a tuple with the value and it's sort by value
    sort_values = map do
      x = Rubinius.single_block_arg
      SortedElement.new(x, yield(x))
    end

    # Now sort the tuple according to the sort by value
    sort_values.sort!

    # Now strip of the tuple leaving the original value
    sort_values.map! { |ary| ary.value }
  end

  def inject(initial=undefined, sym=undefined)
    if !block_given? or !undefined.equal?(sym)
      if undefined.equal?(sym)
        sym = initial
        initial = undefined
      end

      # Do the sym version

      sym = sym.to_sym

      each do
        o = Rubinius.single_block_arg
        if undefined.equal? initial
          initial = o
        else
          initial = initial.__send__(sym, o)
        end
      end

      # Block version
    else
      each do
        o = Rubinius.single_block_arg
        if undefined.equal? initial
          initial = o
        else
          initial = yield(initial, o)
        end
      end
    end

    undefined.equal?(initial) ? nil : initial
  end
  alias_method :reduce, :inject

  def all?
    if block_given?
      each { |*e| return false unless yield(*e) }
    else
      each { return false unless Rubinius.single_block_arg }
    end
    true
  end

  def any?
    if block_given?
      each { |*o| return true if yield(*o) }
    else
      each { return true if Rubinius.single_block_arg }
    end
    false
  end

  def cycle(many=nil)
    return to_enum(:cycle, many) unless block_given?

    if many
      many = Rubinius::Type.coerce_to(many, Fixnum, :to_int)
      return nil if many <= 0
    else
      many = nil
    end

    cache = []
    each do
      elem = Rubinius.single_block_arg
      cache << elem
      yield elem
    end

    return nil if cache.empty?

    if many
      i = 0
      many -= 1
      while i < many
        cache.each { |o| yield o }
        i += 1
      end
    else
      while true
        cache.each { |o| yield o }
      end
    end

    nil
  end

  def drop(n)
    n = Rubinius::Type.coerce_to(n, Fixnum, :to_int)
    raise ArgumentError, "attempt to drop negative size" if n < 0

    ary = to_a
    return [] if n > ary.size
    ary[n...ary.size]
  end

  def drop_while
    return to_enum(:drop_while) unless block_given?

    ary = []
    dropping = true
    each do
      obj = Rubinius.single_block_arg
      ary << obj unless dropping &&= yield(obj)
    end

    ary
  end

  def each_cons(num)
    return to_enum(:each_cons, num) unless block_given?

    n = Rubinius::Type.coerce_to(num, Fixnum, :to_int)
    raise ArgumentError, "invalid size: #{n}" if n <= 0

    array = []
    each do
      element = Rubinius.single_block_arg
      array << element
      array.shift if array.size > n
      yield array.dup if array.size == n
    end
    nil
  end

  def each_slice(slice_size)
    return to_enum(:each_slice, slice_size) unless block_given?

    n = Rubinius::Type.coerce_to(slice_size, Fixnum, :to_int)
    raise ArgumentError, "invalid slice size: #{n}" if n <= 0

    a = []
    each do
      element = Rubinius.single_block_arg
      a << element
      if a.length == n
        yield a
        a = []
      end
    end

    yield a unless a.empty?
    nil
  end

  def find(ifnone=nil)
    return to_enum(:find, ifnone) unless block_given?

    each do
      o = Rubinius.single_block_arg
      return o if yield(o)
    end

    ifnone.call if ifnone
  end

  alias_method :detect, :find

  def find_all
    return to_enum(:find_all) unless block_given?

    ary = []
    each do
      o = Rubinius.single_block_arg
      ary << o if yield(o)
    end
    ary
  end

  alias_method :select, :find_all

  def find_index(value=undefined)
    if undefined.equal? value
      return to_enum(:find_index) unless block_given?

      i = 0
      each do |*args|
        return i if yield(*args)
        i += 1
      end
    else
      i = 0
      each do
        e = Rubinius.single_block_arg
        return i if e == value
        i += 1
      end
    end
    nil
  end

  def first(n=undefined)
    return __take__(n) unless undefined.equal?(n)
    each { |obj| return obj }
    nil
  end

  def min
    min = undefined
    each do
      o = Rubinius.single_block_arg
      if undefined.equal? min
        min = o
      else
        comp = block_given? ? yield(o, min) : o <=> min
        unless comp
          raise ArgumentError, "comparison of #{o.class} with #{min} failed"
        end

        if Comparable.compare_int(comp) < 0
          min = o
        end
      end
    end

    undefined.equal?(min) ? nil : min
  end

  def max
    max = undefined
    each do
      o = Rubinius.single_block_arg
      if undefined.equal? max
        max = o
      else
        comp = block_given? ? yield(o, max) : o <=> max
        unless comp
          raise ArgumentError, "comparison of #{o.class} with #{max} failed"
        end

        if Comparable.compare_int(comp) > 0
          max = o
        end
      end
    end

    undefined.equal?(max) ? nil : max
  end

  def max_by
    return to_enum(:max_by) unless block_given?

    max_object = nil
    max_result = undefined

    each do
      object = Rubinius.single_block_arg
      result = yield object

      if undefined.equal?(max_result) or \
           Rubinius::Type.coerce_to_comparison(max_result, result) < 0
        max_object = object
        max_result = result
      end
    end

    max_object
  end

  def min_by
    return to_enum(:min_by) unless block_given?

    min_object = nil
    min_result = undefined

    each do
      object = Rubinius.single_block_arg
      result = yield object

      if undefined.equal?(min_result) or \
           Rubinius::Type.coerce_to_comparison(min_result, result) > 0
        min_object = object
        min_result = result
      end
    end

    min_object
  end

  def self.sort_proc
    @sort_proc ||= Proc.new do |a, b|
      unless ret = a <=> b
        raise ArgumentError, "Improper spaceship value"
      end
      ret
    end
  end

  def minmax(&block)
    block = Enumerable.sort_proc unless block
    first_time = true
    min, max = nil

    each do
      object = Rubinius.single_block_arg
      if first_time
        min = max = object
        first_time = false
      else
        unless min_cmp = block.call(min, object)
          raise ArgumentError, "comparison failed"
        end
        min = object if min_cmp > 0

        unless max_cmp = block.call(max, object)
          raise ArgumentError, "comparison failed"
        end

        max = object if max_cmp < 0
      end
    end
    [min, max]
  end

  def minmax_by(&block)
    return to_enum(:minmax_by) unless block_given?

    min_object = nil
    min_result = undefined

    max_object = nil
    max_result = undefined

    each do
      object = Rubinius.single_block_arg
      result = yield object

      if undefined.equal?(min_result) or \
           Rubinius::Type.coerce_to_comparison(min_result, result) > 0
        min_object = object
        min_result = result
      end

      if undefined.equal?(max_result) or \
           Rubinius::Type.coerce_to_comparison(max_result, result) < 0
        max_object = object
        max_result = result
      end
    end

    [min_object, max_object]
  end

  def none?
    if block_given?
      each { |*o| return false if yield(*o) }
    else
      each { return false if Rubinius.single_block_arg }
    end

    return true
  end

  def one?
    found_one = false

    if block_given?
      each do |*o|
        if yield(*o)
          return false if found_one
          found_one = true
        end
      end
    else
      each do
        if Rubinius.single_block_arg
          return false if found_one
          found_one = true
        end
      end
    end

    found_one
  end

  def partition
    return to_enum(:partition) unless block_given?

    left = []
    right = []
    each do
      o = Rubinius.single_block_arg
      yield(o) ? left.push(o) : right.push(o)
    end

    return [left, right]
  end

  def reject
    return to_enum(:reject) unless block_given?

    ary = []
    each do
      o = Rubinius.single_block_arg
      ary << o unless yield(o)
    end

    ary
  end

  def reverse_each(&block)
    return to_enum(:reverse_each) unless block_given?

    # There is no other way then to convert to an array first... see 1.9's source.
    to_a.reverse_each(&block)
    self
  end

  def take(n)
    n = Rubinius::Type.coerce_to(n, Fixnum, :to_int)
    raise ArgumentError, "attempt to take negative size: #{n}" if n < 0

    array = []

    unless n <= 0
      each do
        elem = Rubinius.single_block_arg
        array << elem
        break if array.size >= n
      end
    end

    array
  end
  alias_method :__take__, :take
  private :__take__

  def take_while
    return to_enum(:take_while) unless block_given?

    array = []
    each do |elem|
      return array unless yield(elem)
      array << elem
    end

    array
  end

  def include?(obj)
    each { return true if Rubinius.single_block_arg == obj }
    false
  end

  alias_method :member?, :include?

end
