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
end
