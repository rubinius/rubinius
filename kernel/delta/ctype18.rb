module Rubinius::CType
  # TODO: rethink this.
  def self.unprintable_chr(num)
    str = "\\000"
    str.modify!
    m = Rubinius::Mirror.reflect str

    c = num.to_s 8
    m.copy_from c, 0, c.size, 4-c.size

    str
  end
end
