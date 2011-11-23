module Rubinius
  class CharArray
    attr_accessor :encoding

    # TODO: encoding
    def character_at_index(index)
      "" << self[index]
    end
  end
end
