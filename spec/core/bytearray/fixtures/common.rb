module ByteArraySpecs
  def self.new_bytearray(str)
    b = Rubinius::ByteArray.new str.size
    str.each_byte.with_index { |x, i| b[i] = x }
    b
  end
end
