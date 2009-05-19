class String
  alias_method :bytesize, :length
  
  def each_char(&block)
    return to_enum :each_char unless block_given?
    return scan(/./u, &block) if $KCODE == "UTF-8"
    i = 0
    while i < @num_bytes do
      yield @data.get_byte(i).chr
      i += 1
    end

    self
  end

  alias_method :chars, :each_char
end
