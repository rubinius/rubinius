class Rational
  def rationalize(eps=undefined)
    if eps.equal?(undefined)
      self
    else
      e = eps.abs
      a = self - e
      b = self + e

      p0 = 0
      p1 = 1
      q0 = 1
      q1 = 0

      while true
        c = a.ceil
        break if c < b
        k = c - 1
        p2 = k * p1 + p0
        q2 = k * q1 + q0
        t = 1 / (b - k)
        b = 1 / (a - k)
        a = t
        p0 = p1
        q0 = q1
        p1 = p2
        q1 = q2
      end

      Rational(c * p1 + p0, c * q1 + q0)
    end
  end
end
