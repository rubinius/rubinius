module Rubinius
  class CharArray
    alias_method :character_at_index, :[]
  end
end
