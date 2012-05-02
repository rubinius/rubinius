module Rubinius::CType
  def self.unprintable_chr(num)
    str = "\\000"
    str.modify!

    c = num.to_s 8
    str.copy_from c, 0, c.size, 4-c.size
  end
end