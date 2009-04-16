class Numeric
  # C-API method corresponding to rb_num_coerce_bin
  def self.num_coerce_bin(x, y, op)
    begin
      a, b = y.coerce x
    rescue
      raise TypeError, "#{y.inspect} can't be coerced into #{x.class}"
    end
    a.send op, b
  end

  # C-API method corresponding to rb_num_coerce_cmp
  def self.num_coerce_cmp(x, y, op)
    begin
      a, b = y.coerce x
    rescue
      return nil
    end
    a.send op, b
  end
end
