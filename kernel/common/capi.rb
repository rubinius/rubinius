module Rubinius

  # CAPI contains utility methods used by capi.
  module CAPI
    # C-API method corresponding to rb_num_coerce_bin
    def self.rb_num_coerce_bin(x, y, op)
      begin
        a, b = y.coerce x
      rescue
        raise TypeError, "#{y.inspect} can't be coerced into #{x.class}"
      end

      a.__send__ op, b
    end

    # C-API method corresponding to rb_num_coerce_cmp
    def self.rb_num_coerce_cmp(x, y, op)
      begin
        a, b = y.coerce x
      rescue
        return nil
      end

      a.__send__ op, b
    end
  end
end
