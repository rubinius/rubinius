module Rubinius
  module Type
    def self.coerce_to_path(obj)
      unless object_kind_of?(obj, String)
        if converted = try_convert(obj, String, :to_path)
          obj = converted
        else
          obj = StringValue(obj)
        end
      end
      obj
    end
  end
end
