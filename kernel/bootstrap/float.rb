class Float
  def to_s_formatted(format)
    Ruby.primitive :float_sprintf
  end
end
