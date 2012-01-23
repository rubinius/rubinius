# -*- encoding: us-ascii -*-

class Regexp
  def fixed_encoding?
    Rubinius.primitive :regexp_fixed_encoding_p
    raise PrimitiveFailure, "Regexp.fixed_encoding? primitive failed"
  end
end
