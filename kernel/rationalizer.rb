# Float#rationalize and String#to_r require complex algorithms. The Regexp
# required for String#to_r cannot be created at class scope when loading the
# kernel because Regexp needs to load after String and making a simpler
# Regexp#initialize for bootstrapping isn't really feasible. So these
# algorithms are located here.
#
class String
  class Rationalizer
    SPACE = "\\s*"
    DIGITS = "(?:[0-9](?:_[0-9]|[0-9])*)"
    NUMERATOR = "(?:#{DIGITS}?\\.)?#{DIGITS}(?:[eE][-+]?#{DIGITS})?"
    DENOMINATOR = DIGITS
    RATIONAL = "\\A#{SPACE}([-+])?(#{NUMERATOR})(?:\\/(#{DENOMINATOR}))?#{SPACE}"
    PATTERN = Regexp.new RATIONAL

    def initialize(value)
      @value = value
    end

    def convert
      if m = PATTERN.match(@value)
        si = m[1]
        nu = m[2]
        de = m[3]
        re = m.post_match

        ifp, exp = nu.split /[eE]/
        ip, fp = ifp.split /\./

        value = Rational.new(ip.to_i, 1)

        if fp
          ctype = Rubinius::CType
          i = count = 0
          size = fp.size
          while i < size
            count += 1 if ctype.isdigit fp.getbyte(i)
            i += 1
          end

          l = 10 ** count
          value *= l
          value += fp.to_i
          value = value.quo(l)
        end

        value = -value if si == "-"
        value *= 10 ** exp.to_i if exp
        value = value.quo(de.to_i) if de

        value
      else
        Rational(0, 1)
      end
    end
  end
end
