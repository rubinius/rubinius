class Rubinius::Randomizer
  def self.allocate
    Ruby.primitive :randomizer_allocate
    raise PrimitiveFailure, "Randomizer.allocate primitive failed"
  end

  def self.instance
    @instance ||= new
  end

  def initialize
    self.seed = generate_seed
  end

  attr_reader :seed
  def seed=(new_seed)
    set_seed new_seed
    @seed = new_seed
  end

  def set_seed(new_seed)
    Ruby.primitive :randomizer_seed
    raise PrimitiveFailure, "Randomizer#seed primitive failed"
  end

  def swap_seed(new_seed)
    old_seed  = self.seed
    self.seed = new_seed
    old_seed
  end

  # Generate a random Float, in the range 0...1.0
  def random_float
    Ruby.primitive :randomizer_rand_float
    raise PrimitiveFailure, "Randomizer#rand_float primitive failed"
  end

  # Generate a random Integer, in the range 0...limit
  def random_integer(limit)
    Ruby.primitive :randomizer_rand_int
    raise PrimitiveFailure, "Randomizer#rand_int primitive failed"
  end

  def generate_seed
    Ruby.primitive :randomizer_gen_seed
    raise PrimitiveFailure, "Randomizer#gen_seed primitive failed"
  end
end
