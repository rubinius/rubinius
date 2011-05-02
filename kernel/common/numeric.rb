class Numeric
  include Comparable

  # Always raises TypeError, as dup'ing Numerics is not allowed.
  def initialize_copy(other)
    raise TypeError, "copy of #{self.class} is not allowed"
  end

  def +@
    self
  end

  def -@
    0 - self
  end

  def divmod(other)
    [div(other), self % other]
  end

  def div(other)
    Float(self.__slash__(other)).floor
  end

  def quo(other)
    self.__slash__(other)
  end

  alias_method :fdiv, :quo

  def eql?(other)
    return false unless other.instance_of? self.class
    self == other
  end

  def <=>(other)
    # It's important this method NOT contain the coercion protocols!
    # MRI doesn't and doing so breaks stuff!

    return 0 if self.equal? other
    return nil
  end

  def step(limit, step=1, &block)
    return to_enum(:step, limit, step) unless block_given?

    raise ArgumentError, "step cannot be 0" if step == 0

    value = self
    if value.kind_of? Fixnum and limit.kind_of? Fixnum and step.kind_of? Fixnum
      if step > 0
        while value <= limit
          yield value
          value += step
        end
      else
        while value >= limit
          yield value
          value += step
        end
      end

    elsif value.kind_of? Float or limit.kind_of? Float or step.kind_of? Float
      # Ported from MRI

      value = FloatValue(value)
      limit = FloatValue(limit)
      step =  FloatValue(step)

      range = limit - value
      n = range / step;
      err = (value.abs + limit.abs + range.abs) / step.abs * Float::EPSILON

      if step.infinite?
        yield value if step > 0 ? value <= limit : value >= limit
      else
        err = 0.5 if err > 0.5
        n = (n + err).floor + 1
        i = 0
        while i < n
          yield i * step + value
          i += 1
        end
      end

    else
      if step > 0
        until value > limit
          yield value
          value += step
        end
      else
        until value < limit
          yield value
          value += step
        end
      end
    end

    return self
  end

  def truncate
    Float(self).truncate
  end

  # Delegate #to_int to #to_i in subclasses
  def to_int
    to_i
  end

  # Delegate #modulo to #% in subclasses
  def modulo(other)
    self % other
  end

  def integer?
    false
  end

  def zero?
    self == 0
  end

  def nonzero?
    zero? ? nil : self
  end

  def round
    to_f.round
  end

  def abs
    self < 0 ? -self : self
  end

  def floor
    FloatValue(self).floor
  end

  def ceil
    FloatValue(self).ceil
  end

  def remainder(other)
    mod = self % other

    if mod != 0 and ((self < 0 and other > 0) or (self > 0 and other < 0))
      mod - other
    else
      mod
    end
  end

  #--
  # We deviate from MRI behavior here because we ensure that Fixnum op Bignum
  # => Bignum (possibly normalized to Fixnum)
  #
  # Note these differences on MRI, where a is a Fixnum, b is a Bignum
  #
  #   a.coerce b => [Float, Float]
  #   b.coerce a => [Bignum, Bignum]
  #++

  def coerce(other)
    if other.instance_of? self.class
      return [other, self]
    end

    [Float(other), Float(self)]
  end

  ##
  # This method mimics the semantics of MRI's do_coerce function
  # in numeric.c. Note these differences between it and #coerce:
  #
  #   1.2.coerce("2") => [2.0, 1.2]
  #   1.2 + "2" => TypeError: String can't be coerced into Float
  #
  # See also Integer#coerce

  def math_coerce(other, error=:coerce_error)
    begin
      values = other.coerce(self)
    rescue
      if error == :coerce_error
        raise TypeError, "#{other.class} can't be coerced into #{self.class}"
      else
        raise ArgumentError, "comparison of #{self.class} with #{other.class} failed"
      end
    end

    unless Rubinius::Type.object_kind_of?(values, Array) && values.length == 2
      raise TypeError, "coerce must return [x, y]"
    end

    return values[1], values[0]
  end
  private :math_coerce

  def redo_coerced(meth, right)
    b, a = math_coerce(right)
    a.__send__ meth, b
  end
  private :redo_coerced

  def redo_compare(meth, right)
    b, a = math_coerce(right, :compare_error)
    a.__send__ meth, b
  end

end

