module Rubinius::CType
  def self.unprintable_chr(num)
    str = "\\x00"
    str.modify!
    m = Rubinius::Mirror.reflect str

    c = num.to_s(16).upcase
    m.copy_from c, 0, c.size, 4-c.size

    str
  end
end
