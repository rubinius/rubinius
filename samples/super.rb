class Go
  def initialize(a)
    8 + 9
    p a
    raise "crap #{a}!"
  end
end

class Box < Go
    def initialize
        super("evan")
        9 + 8
    end
end

Box.new
