# This stupidity is necessary to counter the stupidity of checking
# 1.9 syntax support by checking if Encoding is defined. Who said
# two stupids don't make a smart.
Encoding = Rubinius::Encoding::Encoding

class Encoding
  attr_reader :name
  attr_reader :dummy

  alias_method :to_s, :name
  alias_method :dummy?, :dummy

  def replicate(name)
    Rubinius.primitive :encoding_replicate
    raise PrimitiveFailure, "Encoding#replicate primitive failed"
  end
end
