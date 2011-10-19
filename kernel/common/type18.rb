module Rubinius
  module Type
    def self.coerce_to_path(obj)
      StringValue(obj)
    end

    def self.coerce_to_float(obj)
      Float(obj)
    end
  end
end
