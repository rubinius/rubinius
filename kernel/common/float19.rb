class Float
  INFINITY = 1.0 / 0.0
  NAN = 0.0 / 0.0

  def imaginary
    0
  end

  def numerator
    if nan?
      NAN
    elsif infinite? == 1
      INFINITY
    elsif infinite? == -1
      -INFINITY
    else
      super
    end
  end

  def denominator
    if infinite? || nan?
      1
    else
      super
    end
  end

  def to_r
    f, e = Math.frexp self
    f = Math.ldexp(f, MANT_DIG).to_i
    e -= MANT_DIG

    (f * (RADIX ** e)).to_r
  end

  def round(precision=0)
    Integer.induced_from(precision)
    if precision == 0
      round_up? ? ceil : floor
    else
      multiple_of_ten = 10.0 ** Integer(precision).abs
      if precision > 0
        if finite?
          number = self * multiple_of_ten
          if number.infinite?
            self
          else
            if round_up?(number)
              number.ceil / multiple_of_ten
            else
              number.floor / multiple_of_ten
            end
          end
        else
          self
        end
      else
        if multiple_of_ten.infinite?
          0
        else
          number = self / multiple_of_ten
          if round_up?(number)
            (1..Integer(precision).abs).inject(number.ceil) { |num| num * 10 }
          else
            (1..Integer(precision).abs).inject(number.floor) { |num| num * 10 }
          end
        end
      end
    end
  end

  def round_up?(value=self)
    (value > 0 && (value - value.to_i).abs >= 0.5) || (value < 0 && (value - value.to_i).abs < 0.5)
  end
  private :round_up?
end
