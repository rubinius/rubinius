module Rubinius
  module EnumerableHelper
    def self.cycle_size(enum_size, many)
      if many
        many = Rubinius::Type.coerce_to_collection_index many
        many = 0 if many < 0
        (_size = enum_size).nil? ? nil : _size * many
      else
        enum_size.nil? ? nil : Float::INFINITY
      end
    end
  end
end
