module Rubinius
  module Type
    def self.coerce_to_path(obj)
      StringValue(obj)
    end
  end
end
