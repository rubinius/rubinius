module AL
  def AL.a
    :a
  end

  class << AL
    alias b a
  end

  def AL.c
    :c
  end

  class << self
    alias d c
  end
end

p [AL.a, AL.b, AL.c, AL.d]

# OUTPUT
# [:a, :a, :c, :c]
