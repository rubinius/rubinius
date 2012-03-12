# -*- encoding: us-ascii -*-

module Rubinius
  module Type
    def self.coerce_to_array(obj)
      return [obj] unless obj

      return obj.to_ary if obj.respond_to?(:to_ary)

      # Just call #to_a, which wraps the reciever in an
      # array if it's not one.
      return obj.to_a
    end
  end
end
