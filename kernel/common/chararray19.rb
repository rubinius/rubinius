module Rubinius
  class CharArray
    # TODO: encoding
    def character_at_index(index)
      "" << self[index]
    end
  end
end
