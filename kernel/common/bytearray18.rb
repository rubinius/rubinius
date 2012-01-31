# -*- encoding: us-ascii -*-

module Rubinius
  class ByteArray
    alias_method :character_at_index, :[]
  end
end
