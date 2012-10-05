module Rubinius::CType
  def self.unprintable_chr(num)
    str = "\\x00"
    str.modify!

    c = num.to_s(16).upcase
    str.copy_from c, 0, c.size, 4-c.size
  end

  def self.unprintable_utf8_chr(num)
    if num <= 0x7f
      str = "\\u0000"
      str.modify!

      c = num.to_s(16).upcase
      str.copy_from c, 0, c.size, 6-c.size
    else
      unprintable_chr(num)
    end
  end
end
