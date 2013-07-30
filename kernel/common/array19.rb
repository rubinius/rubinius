# -*- encoding: us-ascii -*-

class Array
  # Try to convert obj into an array, using to_ary method.
  # Returns converted array or nil if obj cannot be converted
  # for any reason. This method is to check if an argument is an array.
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Array, :to_ary
  end

  def set_index(index, ent, fin=undefined)
    Rubinius.primitive :array_aset

    Rubinius.check_frozen

    ins_length = nil
    unless undefined.equal? fin
      ins_length = Rubinius::Type.coerce_to ent, Fixnum, :to_int
      ent = fin             # 2nd arg (ins_length) is the optional one!
    end

    # Normalise Ranges
    if index.kind_of? Range
      if ins_length
        raise ArgumentError, "Second argument invalid with a range"
      end

      last = Rubinius::Type.coerce_to index.last, Fixnum, :to_int
      last += @total if last < 0
      last += 1 unless index.exclude_end?

      index = Rubinius::Type.coerce_to index.first, Fixnum, :to_int

      if index < 0
        index += @total
        raise RangeError, "Range begin #{index-@total} out of bounds" if index < 0
      end

      # m..n, m > n allowed
      last = index if index > last

      ins_length = last - index
    else
      index = Rubinius::Type.coerce_to index, Fixnum, :to_int

      if index < 0
        index += @total
        raise IndexError,"Index #{index-@total} out of bounds" if index < 0
      end
    end

    if ins_length
      # ins_length < 0 not allowed
      raise IndexError, "Negative length #{ins_length}" if ins_length < 0

      # MRI seems to be forgiving here!
      space = @total - index
      if ins_length > space
        ins_length = space > 0 ? space : 0
      end

      replace_count = 0

      if ent.kind_of? Array
        replacement = ent
        replace_count = replacement.size
        replacement = replacement.first if replace_count == 1
      elsif ent.respond_to? :to_ary
        replacement = ent.to_ary
        replace_count = replacement.size
        replacement = replacement.first if replace_count == 1
      else
        replacement = ent
        replace_count = 1
      end

      new_total = (index > @total) ? index : @total
      if replace_count > ins_length
        new_total += replace_count - ins_length
      elsif replace_count < ins_length
        new_total -= ins_length - replace_count
      end

      if new_total > @tuple.size - @start
        # Expand the size just like #<< does.
        # MRI uses a straight realloc here to the exact size, but
        # realloc can easily include bumper data so it's pretty fast.
        # We simply compensate by using the same logic to reduce
        # having to copy data.
        new_tuple = Rubinius::Tuple.new(new_total + @tuple.size / 2)

        new_tuple.copy_from(@tuple, @start, index < @total ? index : @total, 0)

        case replace_count
        when 1
          new_tuple[index] = replacement
        when 0
          # nothing
        else
          new_tuple.copy_from(replacement.tuple, replacement.start,
                              replace_count, index)
        end

        if index < @total
          new_tuple.copy_from(@tuple, @start + index + ins_length,
                              @total - index - ins_length,
                              index + replace_count)
        end
        @start = 0
        @tuple = new_tuple
        @total = new_total
      else
        # Move the elements to the right
        if index < @total
          right_start = @start + index + ins_length
          right_len = @total - index - ins_length

          @tuple.copy_from(@tuple, right_start, right_len,
                           @start + index + replace_count)
        end

        case replace_count
        when 1
          @tuple[@start + index] = replacement
        when 0
          # nothing
        else
          @tuple.copy_from(replacement.tuple, replacement.start,
                              replace_count, @start + index)
        end

        @total = new_total
      end

      return ent
    else
      nt = @start + index + 1
      reallocate(nt) if @tuple.size < nt

      @tuple.put @start + index, ent
      if index >= @total - 1
        @total = index + 1
      end
      return ent
    end
  end

  alias_method :[]=, :set_index

  private :set_index

  def *(multiplier)
    if multiplier.respond_to? :to_str
      return join(multiplier)

    else
      # Aaargh stupid MRI's stupid specific stupid error stupid types stupid
      multiplier = Rubinius::Type.coerce_to multiplier, Fixnum, :to_int

      raise ArgumentError, "Count cannot be negative" if multiplier < 0

      case @total
      when 0
        # Edge case
        out = self.class.allocate
        Rubinius::Type.infect(out, self)
        return out
      when 1
        # Easy case
        tuple = Rubinius::Tuple.pattern multiplier, at(0)
        out = self.class.allocate
        out.tuple = tuple
        out.total = multiplier
        Rubinius::Type.infect(out, self)
        return out
      end

      new_total = multiplier * @total
      new_tuple = Rubinius::Tuple.new(new_total)

      out = self.class.allocate
      out.tuple = new_tuple
      out.total = new_total
      Rubinius::Type.infect(out, self)

      offset = 0
      while offset < new_total
        new_tuple.copy_from @tuple, @start, @total, offset
        offset += @total
      end

      out
    end
  end

  def concat(other)
    Rubinius.primitive :array_concat

    other = Rubinius::Type.coerce_to(other, Array, :to_ary)
    Rubinius.check_frozen

    return self if other.empty?

    concat other
  end

  def delete(obj)
    key = undefined
    i = @start
    total = i + @total
    tuple = @tuple

    while i < total
      element = tuple.at i
      if element == obj
        # We MUST check frozen here, not at the top, because MRI
        # requires that #delete not raise unless an element would
        # be deleted.
        Rubinius.check_frozen
        tuple.put i, key
        last_matched_element = element
      end
      i += 1
    end

    deleted = @tuple.delete @start, @total, key
    if deleted > 0
      @total -= deleted
      reallocate_shrink()
      return last_matched_element
    end

    if block_given?
      yield
    else
      nil
    end
  end

  def flatten(level=-1)
    level = Rubinius::Type.coerce_to(level, Integer, :to_int)
    return self.dup if level == 0

    out = new_reserved size
    recursively_flatten(self, out, level)
    Rubinius::Type.infect(out, self)
    out
  end

  def flatten!(level=-1)
    Rubinius.check_frozen

    level = Rubinius::Type.coerce_to(level, Integer, :to_int)
    return nil if level == 0

    out = new_reserved size
    if recursively_flatten(self, out, level)
      replace(out)
      return self
    end

    nil
  end

  def insert(idx, *items)
    Rubinius.check_frozen

    return self if items.length == 0

    # Adjust the index for correct insertion
    idx = Rubinius::Type.coerce_to idx, Fixnum, :to_int
    idx += (@total + 1) if idx < 0    # Negatives add AFTER the element
    raise IndexError, "#{idx} out of bounds" if idx < 0

    self[idx, 0] = items   # Cheat
    self
  end

  def join(sep=nil)
    return "" if @total == 0

    out = ""
    raise ArgumentError, "recursive array join" if Thread.detect_recursion self do
      sep = sep.nil? ? $, : StringValue(sep)

      # We've manually unwound the first loop entry for performance
      # reasons.
      x = @tuple[@start]

      if str = String.try_convert(x)
        x = str
      elsif ary = Array.try_convert(x)
        x = ary.join(sep)
      else
        x = x.to_s
      end

      out.force_encoding(x.encoding)
      out << x

      total = @start + size()
      i = @start + 1

      while i < total
        out << sep if sep

        x = @tuple[i]

        if str = String.try_convert(x)
          x = str
        elsif ary = Array.try_convert(x)
          x = ary.join(sep)
        else
          x = x.to_s
        end

        out << x
        i += 1
      end
    end

    Rubinius::Type.infect(out, self)
  end

  def keep_if(&block)
    return to_enum :keep_if unless block_given?

    Rubinius.check_frozen

    replace select(&block)
  end

  def pack(directives)
    Rubinius.primitive :array_pack19

    unless directives.kind_of? String
      return pack(StringValue(directives))
    end

    raise ArgumentError, "invalid directives string: #{directives}"
  end

  # Implementation notes: We build a block that will generate all the
  # combinations by building it up successively using "inject" and starting
  # with one responsible to append the values.
  def product(*args)
    args.map! { |x| Rubinius::Type.coerce_to(x, Array, :to_ary) }

    # Check the result size will fit in an Array.
    sum = args.inject(size) { |n, x| n * x.size }

    if sum > Fixnum::MAX
      raise RangeError, "product result is too large"
    end

    # TODO rewrite this to not use a tree of Proc objects.

    # to get the results in the same order as in MRI, vary the last argument first
    args.reverse!

    result = []
    args.push self

    outer_lambda = args.inject(result.method(:push)) do |trigger, values|
      lambda do |partial|
        values.each do |val|
          trigger.call(partial.dup << val)
        end
      end
    end

    outer_lambda.call([])

    if block_given?
      block_result = self
      result.each { |v| block_result << yield(v) }
      block_result
    else
      result
    end
  end

  def push(*args)
    Rubinius.check_frozen

    return self if args.empty?

    concat args
  end

  def repeated_combination(combination_size, &block)
    combination_size = combination_size.to_i
    unless block_given?
      return Enumerator.new(self, :repeated_combination, combination_size)
    end

    if combination_size < 0
      # yield nothing
    else
      Rubinius.privately do
        dup.compile_repeated_combinations(combination_size, [], 0, combination_size, &block)
      end
    end

    return self
  end

  def compact
    out = dup
    out.untaint if out.tainted?
    out.trust if out.untrusted?

    Array.new(out.compact! || out)
  end

  def compile_repeated_combinations(combination_size, place, index, depth, &block)
    if depth > 0
      (length - index).times do |i|
        place[combination_size-depth] = index + i
        compile_repeated_combinations(combination_size,place,index + i,depth-1, &block)
      end
    else
      yield place.map { |element| self[element] }
    end
  end

  private :compile_repeated_combinations

  def reject(&block)
    return to_enum(:reject) unless block_given?
    Array.new(self).delete_if(&block)
  end

  def repeated_permutation(combination_size, &block)
    combination_size = combination_size.to_i
    unless block_given?
      return Enumerator.new(self, :repeated_permutation, combination_size)
    end

    if combination_size < 0
      # yield nothing
    elsif combination_size == 0
      yield []
    else
      Rubinius.privately do
        dup.compile_repeated_permutations(combination_size, [], 0, &block)
      end
    end

    return self
  end

  def compile_repeated_permutations(combination_size, place, index, &block)
    length.times do |i|
      place[index] = i
      if index < (combination_size-1)
        compile_repeated_permutations(combination_size, place, index + 1, &block)
      else
        yield place.map { |element| self[element] }
      end
    end
  end

  private :compile_repeated_permutations

  def reverse
    Array.new dup.reverse!
  end

  def rotate(n=1)
    n = Rubinius::Type.coerce_to(n, Integer, :to_int)
    return Array.new(self) if length == 1
    return []       if empty?

    ary = Array.new(self)
    idx = n % ary.size

    ary[idx..-1].concat ary[0...idx]
  end

  def rotate!(cnt=1)
    Rubinius.check_frozen

    return self if length == 0 || length == 1

    ary = rotate(cnt)
    replace ary
  end

  def sample(count=undefined, options=undefined)
    return at Kernel.rand(size) if undefined.equal? count

    if undefined.equal? options
      if o = Rubinius::Type.check_convert_type(count, Hash, :to_hash)
        options = o
        count = nil
      else
        options = nil
        count = Rubinius::Type.coerce_to count, Fixnum, :to_int
      end
    else
      count = Rubinius::Type.coerce_to count, Fixnum, :to_int
      options = Rubinius::Type.coerce_to options, Hash, :to_hash
    end

    if count and count < 0
      raise ArgumentError, "count must be greater than 0"
    end

    rng = options[:random] if options
    rng = Kernel unless rng and rng.respond_to? :rand

    unless count
      random = Rubinius::Type.coerce_to rng.rand, Float, :to_f
      raise RangeError, "random value must be >= 0.0" if random < 0
      raise RangeError, "random value must be < 1.0" if random >= 1.0

      return at random * size
    end

    count = size if count > size
    result = Array.new self

    count.times do |i|
      random = Rubinius::Type.coerce_to rng.rand, Float, :to_f
      raise RangeError, "random value must be >= 0.0" if random < 0
      raise RangeError, "random value must be < 1.0" if random >= 1.0

      r = random * size
      result.tuple.swap(i, r.to_i)
    end

    return count == size ? result : result[0, count]
  end

  def select!(&block)
    return to_enum :select! unless block_given?

    Rubinius.check_frozen

    ary = select(&block)
    replace ary unless size == ary.size
  end

  def shuffle(options = undefined)
    return dup.shuffle!(options) if instance_of? Array
    Array.new(self).shuffle!(options)
  end

  def shuffle!(options = undefined)
    Rubinius.check_frozen

    random_generator = Kernel

    unless undefined.equal? options
      options = Rubinius::Type.coerce_to options, Hash, :to_hash
      random_generator = options[:random] if options[:random].respond_to?(:rand)
    end

    size.times do |i|
      r = i + random_generator.rand(size - i)
      @tuple.swap(@start + i, @start + r)
    end
    self
  end

  def slice!(start, length=undefined)
    Rubinius.check_frozen

    if undefined.equal? length
      if start.kind_of? Range
        range = start
        out = self[range]

        range_start = Rubinius::Type.coerce_to range.begin, Fixnum, :to_int
        if range_start < 0
          range_start = range_start + @total
        end

        range_end = Rubinius::Type.coerce_to range.end, Fixnum, :to_int
        if range_end < 0
          range_end = range_end + @total
        elsif range_end >= @total
          range_end = @total - 1
        end

        range_length = range_end - range_start
        range_length += 1 unless range.exclude_end?

        if range_start < @total && range_start >= 0 && range_end < @total && range_end >= 0 && range_length > 0
          delete_range(range_start, range_length)
        end
      else
        # make sure that negative values are not passed through to the
        # []= assignment
        start = Rubinius::Type.coerce_to start, Integer, :to_int
        start = start + @total if start < 0

        # This is to match the MRI behaviour of not extending the array
        # with nil when specifying an index greater than the length
        # of the array.
        return out unless start >= 0 and start < @total

        out = @tuple.at start + @start

        # Check for shift style.
        if start == 0
          @tuple.put @start, nil
          @total -= 1
          @start += 1
        else
          delete_range(start, 1)
        end
      end
    else
      start = Rubinius::Type.coerce_to start, Fixnum, :to_int
      length = Rubinius::Type.coerce_to length, Fixnum, :to_int

      out = self[start, length]

      if start < 0
        start = @total + start
      end
      if start + length > @total
        length = @total - start
      end

      if start < @total && start >= 0
        delete_range(start, length)
      end
    end

    out
  end

  # WARNING: This method does no boundary checking. It is expected that
  # the caller handle that, eg #slice!
  def delete_range(index, del_length)
    # optimize for fast removal..
    reg_start = index + del_length
    reg_length = @total - reg_start

    if reg_start <= @total
      # If we're removing from the front, also reset @start to better
      # use the Tuple
      if index == 0
        # Use a shift start optimization if we're only removing one
        # element and the shift started isn't already huge.
        if del_length == 1
          @start += 1
        else
          @tuple.copy_from @tuple, reg_start + @start, reg_length, 0
          @start = 0
        end
      else
        @tuple.copy_from @tuple, reg_start + @start, reg_length,
          @start + index
      end

      # TODO we leave the old references in the Tuple, we should
      # probably clear them out though.
      @total -= del_length
    end
  end

  private :delete_range

  def sort(&block)
    Array.new dup.sort_inplace(&block)
  end

  def sort_by!(&block)
    Rubinius.check_frozen

    return to_enum :sort_by! unless block_given?

    replace sort_by(&block)
  end

  def inspect
    return "[]" if @total == 0
    comma = ", "
    result = "["

    return "[...]" if Thread.detect_recursion self do
      each_with_index do |element, index|
        temp = element.inspect
        result.force_encoding(temp.encoding) if index == 0
        result << temp << comma
      end
    end

    Rubinius::Type.infect(result, self)
    result.shorten!(2)
    result << "]"
    result
  end

  alias_method :to_s, :inspect

  def uniq(&block)
    dup.uniq!(&block) or dup
  end

  def uniq!(&block)
    Rubinius.check_frozen

    if block_given?
      im = Rubinius::IdentityMap.from(self, &block)
    else
      im = Rubinius::IdentityMap.from(self)
    end
    return if im.size == size

    array = im.to_array
    @tuple = array.tuple
    @start = array.start
    @total = array.total

    self
  end

  def zip(*others)
    out = Array.new(size) { [] }
    others = others.map do |ary|
      if ary.respond_to?(:to_ary)
        ary.to_ary
      else
        elements = []
        ary.each { |e| elements << e }
        elements
      end
    end

    size.times do |i|
      slot = out.at(i)
      slot << @tuple.at(@start + i)
      others.each { |ary| slot << ary.at(i) }
    end

    if block_given?
      out.each { |ary| yield ary }
      return nil
    end

    out
  end

  def unshift(*values)
    Rubinius.check_frozen

    return self if values.empty?

    if @start > values.size
      # fit the new values in between 0 and @start if possible
      @start -= values.size
      @tuple.copy_from(values.tuple, 0, values.size, @start)
    else
      new_tuple = Rubinius::Tuple.new @total + values.size
      new_tuple.copy_from values.tuple, 0, values.size, 0
      new_tuple.copy_from @tuple, @start, @total, values.size
      @start = 0
      @tuple = new_tuple
    end

    @total += values.size
    self
  end
end
