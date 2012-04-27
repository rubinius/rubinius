module Rubinius::CType
  def self.unprintable_chr(num)
    str = "\\x00"
    str.modify!

    c = num.to_s(16).upcase
    str.copy_from c, 0, c.size, 4-c.size
  end
end