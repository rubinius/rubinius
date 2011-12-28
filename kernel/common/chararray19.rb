module Rubinius
  class CharArray
    # TODO: encoding
    def character_at_index(index)
      "" << self[index]
    end

    def force_encoding(enc)
      @ascii = @valid = nil
      @encoding = Type.coerce_to_encoding enc
    end
  end
end
