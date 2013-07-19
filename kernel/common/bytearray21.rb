# -*- encoding: us-ascii -*-

module Rubinius
  class ByteArray
    # TODO: encoding
    def character_at_index(index)
      "" << self[index]
    end
  end
end
