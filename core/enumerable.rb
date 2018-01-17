# Just to save you 10 seconds, the reason we always use #each to extract
# elements instead of something simpler is because Enumerable can not assume
# any other methods than #each. If needed, class-specific versions of any of
# these methods can be written *in those classes* to override these.

module Enumerable
  def chunk
    raise ArgumentError, "no block given" unless block_given?
    ::Enumerator.new do |yielder|
      previous = nil
      accumulate = []
      each do |element|
        key = yield(element)
        if key.nil? || (key.is_a?(Symbol) && key.to_s[0, 1] == "_")
          yielder.yield [previous, accumulate] unless accumulate.empty?
          accumulate = []
          previous = nil
          case key
          when nil, :_separator
          when :_alone
            yielder.yield [key, [element]]
          else
            raise RuntimeError, "symbols beginning with an underscore are reserved"
          end
        else
          if previous.nil? || previous == key
            accumulate << element
          else
            yielder.yield [previous, accumulate] unless accumulate.empty?
            accumulate = [element]
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
      each do |*element|
        ary << yield(*element)
      end
      ary
    else
      to_enum(:collect) { enumerator_size }
    end
  end

  alias_method :map, :collect

  def count(item = undefined)
    seq = 0
    if !undefined.equal?(item)
      each do
        element = Rubinius.single_block_arg
        seq += 1 if item == element
      end
    elsif block_given?
      each { |element| seq += 1 if yield(element) }
    else
      each { seq += 1 }
    end
    seq
  end

  def each_entry(*pass)
    return to_enum(:each_entry, *pass) { enumerator_size } unless block_given?
    each(*pass) do
      element = Rubinius.single_block_arg
      yield element
    end
    self
  end

  def each_with_object(memo)
    return to_enum(:each_with_object, memo) { enumerator_size } unless block_given?
    each do
      element = Rubinius.single_block_arg
      yield element, memo
    end
    memo
  end

  alias_method :with_object, :each_with_object

  def flat_map
    return to_enum(:flat_map) { enumerator_size } unless block_given?

    array = []
    each do |*element|
      result = yield(*element)

      value = Rubinius::Type.try_convert(result, Array, :to_ary) || result

      if value.kind_of? Array
        array.concat value
      else
        array.push value
      end
    end

    array
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
    return to_enum(:group_by) { enumerator_size } unless block_given?

    h = {}
    each do
      element = Rubinius.single_block_arg
      key = yield(element)
      if h.key?(key)
        h[key] << element
      else
        h[key] = [element]
      end
    end
    Rubinius::Type.infect h, self
    h
  end

  def slice_before(pattern = undefined, &block)
    pattern_given = !(undefined.equal? pattern)

    raise ArgumentError, "cannot pass both pattern and block" if pattern_given && block_given?
    raise ArgumentError, "a pattern or a block must be provided" if !pattern_given && !block_given?

    block = Proc.new{ |elem| pattern === elem } if pattern_given

    Enumerator.new do |yielder|
      accumulator = nil
      each do
        element = Rubinius.single_block_arg
        start_new = block.yield(element)
        if start_new
          yielder.yield accumulator if accumulator
          accumulator = [element]
        else
          accumulator ||= []
          accumulator << element
        end
      end
      yielder.yield accumulator if accumulator
    end
  end

  def slice_after(pattern = undefined, &block)
    pattern_given = !undefined.equal?(pattern)

    raise ArgumentError, "cannot pass both pattern and block" if pattern_given && block_given?
    raise ArgumentError, "a pattern or a block must be provided" if !pattern_given && !block_given?

    block = Proc.new { |elem| pattern === elem } if pattern_given

    Enumerator.new do |yielder|
      accumulator = nil
      each do
        element = Rubinius.single_block_arg
        end_chunk = block.yield(element)
        accumulator ||= []
        if end_chunk
          accumulator << element
          yielder.yield accumulator
          accumulator = nil
        else
          accumulator << element
        end
      end
      yielder.yield accumulator if accumulator
    end
  end

  def slice_when(&block)
    raise ArgumentError, "wrong number of arguments (0 for 1)" unless block_given?

    Enumerator.new do |enum|
      ary = nil
      last_after = nil
      each_cons(2) do |before, after|
        last_after = after
        match = block.call before, after

        ary ||= []
        if match
          ary << before
          enum.yield ary
          ary = []
        else
          ary << before
        end
      end

      unless ary.nil?
        ary << last_after
        enum.yield ary
      end
    end
  end

  def chunk_while(&block)
    raise ArgumentError, "no block given" unless block_given?

    slice_when { |before, after| !(yield before, after) }
  end

  def to_a(*arg)
    ary = []
    each(*arg) do
      element = Rubinius.single_block_arg
      ary << element
      nil
    end
    Rubinius::Type.infect ary, self
    ary
  end
  alias_method :entries, :to_a

  def to_h(*arg)
    h = {}
    each_with_index(*arg) do |element, i|
      unless element.respond_to?(:to_ary)
        raise TypeError, "wrong element type #{element.class} at #{i} (expected array)"
      end

      ary = element.to_ary
      if ary.size != 2
        raise ArgumentError, "wrong array length at #{i} (expected 2, was #{ary.size})"
      end

      h[ary[0]] = ary[1]
    end
    h
  end

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
      element = Rubinius.single_block_arg
      entry = args.inject([element]) do |ary, a|
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
    return to_enum(:each_with_index, *args) { enumerator_size } unless block_given?

    idx = 0
    each(*args) do
      element = Rubinius.single_block_arg
      yield element, idx
      idx += 1
    end

    self
  end

  def grep(pattern)
    ary = []

    each do
      element = Rubinius.single_block_arg
      if pattern === element
        Regexp.set_block_last_match
        if block_given?
          ary << yield(element)
        else
          ary << element
        end
      end
    end

    ary
  end

  def grep_v(pattern)
    ary = []

    each do
      element = Rubinius.single_block_arg
      unless pattern === element
        if block_given?
          ary << yield(element)
        else
          ary << element
        end
      end
    end

    ary
  end

  def sort(&prc)
    ary = to_a
    ary.frozen? ? ary.sort(&prc) : ary.sort!(&prc)
  end

  class SortedElement
    def initialize(val, sort_id)
      @value, @sort_id = val, sort_id
    end

    private :initialize

    attr_reader :value
    attr_reader :sort_id

    def <=>(other)
      @sort_id <=> other.sort_id
    end
  end

  def sort_by
    return to_enum(:sort_by) { enumerator_size } unless block_given?

    # Transform each value to a tuple with the value and it's sort by value
    sort_values = map do
      element = Rubinius.single_block_arg
      SortedElement.new(element, yield(element))
    end.to_a

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
        element = Rubinius.single_block_arg
        if undefined.equal? initial
          initial = element
        else
          initial = initial.__send__(sym, element)
        end
      end

      # Block version
    else
      each do
        element = Rubinius.single_block_arg
        if undefined.equal? initial
          initial = element
        else
          initial = yield(initial, element)
        end
      end
    end

    undefined.equal?(initial) ? nil : initial
  end
  alias_method :reduce, :inject

  def all?
    if block_given?
      each { |*element| return false unless yield(*element) }
    else
      each { return false unless Rubinius.single_block_arg }
    end
    true
  end

  def any?
    if block_given?
      each { |*element| return true if yield(*element) }
    else
      each { return true if Rubinius.single_block_arg }
    end
    false
  end

  def cycle(many=nil)
    unless block_given?
      return to_enum(:cycle, many) do
        Rubinius::EnumerableHelper.cycle_size(enumerator_size, many)
      end
    end

    if many
      many = Rubinius::Type.coerce_to_collection_index many
      return nil if many <= 0
    else
      many = nil
    end

    cache = []
    each do
      element = Rubinius.single_block_arg
      cache << element
      yield element
    end

    return nil if cache.empty?

    if many
      i = 0
      many -= 1
      while i < many
        cache.each { |element| yield element }
        i += 1
      end
    else
      while true
        cache.each { |element| yield element }
      end
    end

    nil
  end

  def drop(n)
    n = Rubinius::Type.coerce_to_collection_index n
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
      element = Rubinius.single_block_arg
      ary << element unless dropping &&= yield(element)
    end

    ary
  end

  def each_cons(num)
    n = Rubinius::Type.coerce_to_collection_index num
    raise ArgumentError, "invalid size: #{n}" if n <= 0

    unless block_given?
      return to_enum(:each_cons, num) do
        enum_size = enumerator_size
        if enum_size.nil?
          nil
        elsif enum_size == 0 || enum_size < n
          0
        else
          enum_size - n + 1
        end
      end
    end

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
    n = Rubinius::Type.coerce_to_collection_index slice_size
    raise ArgumentError, "invalid slice size: #{n}" if n <= 0

    unless block_given?
      return to_enum(:each_slice, slice_size) do
        enum_size = enumerator_size
        enum_size.nil? ? nil : (enum_size.to_f / n).ceil
      end
    end

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
      element = Rubinius.single_block_arg
      return element if yield(element)
    end

    ifnone.call if ifnone
  end

  alias_method :detect, :find

  def find_all
    return to_enum(:find_all) { enumerator_size } unless block_given?

    ary = []
    each do
      element = Rubinius.single_block_arg
      ary << element if yield(element)
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
    each do
      return Rubinius.single_block_arg
    end
    nil
  end

  def min(n=nil)
    if n.nil?
      min = undefined
      each do
        element = Rubinius.single_block_arg
        if undefined.equal? min
          min = element
        else
          comp = block_given? ? yield(element, min) : element <=> min
          unless comp
            raise ArgumentError, "comparison of #{element.class} with #{min} failed"
          end

          if Comparable.compare_int(comp) < 0
            min = element
          end
        end
      end

      undefined.equal?(min) ? nil : min
    else
      if block_given?
        sort { |a, b| yield a, b }.take n
      else
        sort.take n
      end
    end
  end

  def max(n=nil)
    if n.nil?
      max = undefined
      each do
        element = Rubinius.single_block_arg
        if undefined.equal? max
          max = element
        else
          comp = block_given? ? yield(element, max) : element <=> max
          unless comp
            raise ArgumentError, "comparison of #{element.class} with #{max} failed"
          end

          if Comparable.compare_int(comp) > 0
            max = element
          end
        end
      end

      undefined.equal?(max) ? nil : max
    else
      if block_given?
        sort { |a, b| yield a, b }.reverse.take n
      else
        sort.reverse.take n
      end
    end
  end

  def max_by(n=nil)
    return to_enum(:max_by, n) { enumerator_size } unless block_given?

    if n.nil?
      max_element = nil
      max_result = undefined

      each do
        element = Rubinius.single_block_arg
        result = yield element

        if undefined.equal?(max_result) or \
             Rubinius::Type.coerce_to_comparison(max_result, result) < 0
          max_element = element
          max_result = result
        end
      end

      max_element
    else
      sort_by { |element| yield element }.reverse.take n
    end
  end

  def min_by(n=nil)
    return to_enum(:min_by, n) { enumerator_size } unless block_given?

    if n.nil?
      min_element = nil
      min_result = undefined

      each do
        element = Rubinius.single_block_arg
        result = yield element

        if undefined.equal?(min_result) or \
             Rubinius::Type.coerce_to_comparison(min_result, result) > 0
          min_element = element
          min_result = result
        end
      end

      min_element
    else
      sort_by { |element| yield element }.take n
    end
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
      element = Rubinius.single_block_arg
      if first_time
        min = max = element
        first_time = false
      else
        unless min_cmp = block.call(min, element)
          raise ArgumentError, "comparison failed"
        end
        min = element if min_cmp > 0

        unless max_cmp = block.call(max, element)
          raise ArgumentError, "comparison failed"
        end

        max = element if max_cmp < 0
      end
    end
    [min, max]
  end

  def minmax_by(&block)
    return to_enum(:minmax_by) { enumerator_size } unless block_given?

    min_element = nil
    min_result = undefined

    max_element = nil
    max_result = undefined

    each do
      element = Rubinius.single_block_arg
      result = yield element

      if undefined.equal?(min_result) or \
           Rubinius::Type.coerce_to_comparison(min_result, result) > 0
        min_element = element
        min_result = result
      end

      if undefined.equal?(max_result) or \
           Rubinius::Type.coerce_to_comparison(max_result, result) < 0
        max_element = element
        max_result = result
      end
    end

    [min_element, max_element]
  end

  def none?
    if block_given?
      each { |*element| return false if yield(*element) }
    else
      each { return false if Rubinius.single_block_arg }
    end

    return true
  end

  def one?
    found_one = false

    if block_given?
      each do |*element|
        if yield(*element)
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
    return to_enum(:partition) { enumerator_size } unless block_given?

    left = []
    right = []
    each do
      element = Rubinius.single_block_arg
      yield(element) ? left.push(element) : right.push(element)
    end

    return [left, right]
  end

  def reject
    return to_enum(:reject) { enumerator_size } unless block_given?

    ary = []
    each do
      element = Rubinius.single_block_arg
      ary << element unless yield(element)
    end

    ary
  end

  def reverse_each(&block)
    return to_enum(:reverse_each) { enumerator_size } unless block_given?

    # There is no other way then to convert to an array first... see 1.9's source.
    to_a.reverse_each(&block)
    self
  end

  def take(n)
    n = Rubinius::Type.coerce_to_collection_index n
    raise ArgumentError, "attempt to take negative size: #{n}" if n < 0

    array = []

    unless n <= 0
      each do
        element = Rubinius.single_block_arg
        array << element
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
    each do |element|
      return array unless yield(element)
      array << element
    end

    array
  end

  def include?(obj)
    each { return true if Rubinius.single_block_arg == obj }
    false
  end

  alias_method :member?, :include?

end
