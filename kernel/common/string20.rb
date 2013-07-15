# -*- encoding: us-ascii -*-

class String
  def chars
    if block_given?
      each_char do |char|
        yield char
      end
    else
      each_char.to_a
    end
  end

  def bytes
    if block_given?
      each_byte do |byte|
        yield byte
      end
    else
      each_byte.to_a
    end
  end

  def lines(sep=$/)
    if block_given?
      each_line(sep) do |line|
        yield line
      end
    else
      each_line(sep).to_a
    end
  end
end
