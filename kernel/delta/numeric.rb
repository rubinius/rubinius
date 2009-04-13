class Numeric
  # C-API method corresponding to rb_num_coerce_bin
  def self.num_coerce_bin(x, y, op)
    b, a = x.coerce y
    a.send op, b
  end

  # C-API method corresponding to rb_num_coerce_cmp
  def self.num_coerce_cmp(x, y, op)
    begin
      b, a = x.coerce y
    rescue TypeError
      return nil
    end
    a.send op, b
  end
end
