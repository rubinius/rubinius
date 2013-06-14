# -*- encoding: us-ascii -*-

class String
  class Complexifier
    SPACE = Rationalizer::SPACE
    NUMERATOR = Rationalizer::NUMERATOR
    DENOMINATOR = Rationalizer::DENOMINATOR
    NUMBER = "[-+]?#{NUMERATOR}(?:\\/#{DENOMINATOR})?"
    NUMBERNOS = "#{NUMERATOR}(?:\\/#{DENOMINATOR})?"
    PATTERN0 = Regexp.new "\\A#{SPACE}(#{NUMBER})@(#{NUMBER})#{SPACE}"
    PATTERN1 = Regexp.new "\\A#{SPACE}([-+])?(#{NUMBER})?[iIjJ]#{SPACE}"
    PATTERN2 = Regexp.new "\\A#{SPACE}(#{NUMBER})(([-+])(#{NUMBERNOS})?[iIjJ])?#{SPACE}"

    def initialize(value)
      @value = value
    end

    def convert
      if m = PATTERN0.match(@value)
        sr = m[1]
        si = m[2]
        re = m.post_match
        po = true
      elsif m = PATTERN1.match(@value)
        sr = nil
        si = (m[1] || "") + (m[2] || "1")
        re = m.post_match
        po = false
      elsif m = PATTERN2.match(@value)
        sr = m[1]
        si = m[2] ? m[3] + (m[4] || "1") : nil
        re = m.post_match
        po = false
      else
        return Complex.new(0, 0)
      end

      r = 0
      i = 0

      if sr
        if sr.include?("/")
          r = sr.to_r
        elsif sr.match(/[.eE]/)
          r = sr.to_f
        else
          r = sr.to_i
        end
      end

      if si
        if si.include?("/")
          i = si.to_r
        elsif si.match(/[.eE]/)
          i = si.to_f
        else
          i = si.to_i
        end
      end

      if po
        Complex.polar(r, i)
      else
        Complex.rect(r, i)
      end
    end
  end
end
