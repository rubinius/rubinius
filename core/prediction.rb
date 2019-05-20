module Rubinius
  class Prediction
    def inspect
      "#<#{self.class.name}:0x#{object_id.to_s(16)} valid=#{valid?}>"
    end

    alias_method :to_s, :inspect

    def valid?
      Rubinius.primitive :prediction_valid_p
      raise PrimitiveFailure, "Prediction#valid? primitive failed"
    end
  end
end
