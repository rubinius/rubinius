# -*- encoding: us-ascii -*-

module Rubinius
  def self.pack_to_float(obj)
    unless obj.kind_of?(Numeric)
      raise TypeError, "unable to convert String to Float"
    end
    Type.coerce_to obj, Float, :to_f
  end
end
