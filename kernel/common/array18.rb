# -*- encoding: us-ascii -*-

class Array
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

      if ent.nil?
        # optimize for fast removal..
        reg_start = index + ins_length
        reg_length = @total - reg_start

        if reg_start <= @total
          # If we're removing from the front, also reset @start to better
          # use the Tuple
          if index == 0
            # Use a shift start optimization if we're only removing one
            # element and the shift started isn't already huge.
            if ins_length == 1
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

          @total -= ins_length

          return ent
        end

        # This is actually an addition! silly, I know.
        replace_count = 0
      elsif ent.kind_of? Array
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
        Rubinius::Type.infect(out, self) if multiplier != 0.0
        return out
      end

      new_total = multiplier * @total
      new_tuple = Rubinius::Tuple.new(new_total)

      out = self.class.allocate
      out.tuple = new_tuple
      out.total = new_total
      Rubinius::Type.infect(out, self) if multiplier != 0.0

      offset = 0
      while offset < new_total
        new_tuple.copy_from @tuple, @start, @total, offset
        offset += @total
      end

      out
    end
  end

  def choice
    at Kernel.rand(size)
  end

  def compact
    out = dup
    out.compact! || out
  end

  def concat(other)
    Rubinius.primitive :array_concat

    other = Rubinius::Type.coerce_to(other, Array, :to_ary)
    return self if other.empty?

    Rubinius.check_frozen

    concat other
  end

  def delete(obj)
    key = undefined
    i = @start
    total = i + @total
    tuple = @tuple

    while i < total
      if tuple.at(i) == obj
        # We MUST check frozen here, not at the top, because MRI
        # requires that #delete not raise unless an element would
        # be deleted.
        Rubinius.check_frozen
        tuple.put i, key
      end
      i += 1
    end

    deleted = @tuple.delete @start, @total, key
    if deleted > 0
      @total -= deleted
      reallocate_shrink()
      return obj
    end

    if block_given?
      yield
    else
      nil
    end
  end

  def flatten(level=-1)
    level = Rubinius::Type.coerce_to(level, Integer, :to_int)
    return self if level == 0

    out = new_reserved size
    recursively_flatten(self, out, level)
    Rubinius::Type.infect(out, self)
    out
  end

  def flatten!(level=-1)
    level = Rubinius::Type.coerce_to(level, Integer, :to_int)
    return nil if level == 0

    out = new_reserved size
    if recursively_flatten(self, out, level)
      replace(out)
      return self
    end

    nil
  end

  def indexes(*args)
    warn 'Array#indexes is deprecated, use Array#values_at instead'

    out = []

    args.each do |a|
      if a.kind_of? Range
        out << self[a]
      else
        idx = Rubinius::Type.coerce_to(a, Fixnum, :to_int)
        out << at(idx)
      end
    end

    out
  end

  alias_method :indices, :indexes

  def insert(idx, *items)
    return self if items.length == 0

    Rubinius.check_frozen

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
    return "[...]" if Thread.detect_recursion self do
      sep = sep.nil? ? $, : StringValue(sep)

      # We've manually unwound the first loop entry for performance
      # reasons.
      x = @tuple[@start]

      case x
      when String
        # Nothing
      when Array
        x = x.join(sep)
      else
        x = x.to_s
      end

      out << x
      total = @start + size()
      i = @start + 1

      while i < total
        out << sep if sep

        x = @tuple[i]

        case x
        when String
          # Nothing
        when Array
          x = x.join(sep)
        else
          x = x.to_s
        end

        out << x
        i += 1
      end
    end

    Rubinius::Type.infect(out, self)
  end

  def pack(directives)
    Rubinius.primitive :array_pack18

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

    result
  end

  def push(*args)
    return self if args.empty?

    concat args
  end

  def reject(&block)
    return to_enum(:reject) unless block_given?
    dup.delete_if(&block)
  end

  def reverse
    dup.reverse!
  end

  def shuffle
    dup.shuffle!
  end

  def shuffle!
    Rubinius.check_frozen

    size.times do |i|
      r = i + Kernel.rand(size - i)
      @tuple.swap(@start + i, @start + r)
    end
    self
  end

  def slice!(start, length=undefined)
    Rubinius.check_frozen

    if undefined.equal? length
      if start.kind_of? Range
        out = self[start]

        s = Rubinius::Type.coerce_to start.begin, Fixnum, :to_int
        unless s >= @total or -s > @total
          self[start] = nil
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
          self[start, 1] = nil
        end
      end
    else
      start = Rubinius::Type.coerce_to start, Fixnum, :to_int
      length = Rubinius::Type.coerce_to length, Fixnum, :to_int

      out = self[start, length]

      unless start >= @total or -start > @total
        self[start, length] = nil
      end
    end

    out
  end

  def sort(&block)
    dup.sort_inplace(&block)
  end

  def inspect
    return "[]" if @total == 0

    comma = ", "
    result = "["

    return "[...]" if Thread.detect_recursion self do
      each { |o| result << o.inspect << comma }
    end

    Rubinius::Type.infect(result, self)
    result.shorten!(2)
    result << "]"
  end

  def to_s
    join
  end

  def uniq
    dup.uniq! or dup
  end

  def uniq!
    im = Rubinius::IdentityMap.from self
    return if im.size == size

    Rubinius.check_frozen

    array = im.to_array
    @tuple = array.tuple
    @start = array.start
    @total = array.total

    self
  end

  def zip(*others)
    out = Array.new(size) { [] }
    others = others.map { |ary| ary.to_ary }

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
    return self if values.empty?

    Rubinius.check_frozen

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
