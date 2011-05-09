class Encoding
  attr_reader :name
  attr_reader :dummy

  alias_method :to_s, :name
  alias_method :dummy?, :dummy

  def replicate(name)
    Ruby.primitive :encoding_replicate
    raise PrimitiveFailure, "Encoding#replicate primitive failed"
  end
end
