class Rubinius::Randomizer
  def self.allocate
    Rubinius.primitive :randomizer_allocate
    raise PrimitiveFailure, "Randomizer.allocate primitive failed"
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
    Rubinius.primitive :randomizer_seed
    raise PrimitiveFailure, "Randomizer#seed primitive failed"
  end

  def swap_seed(new_seed)
    old_seed  = self.seed
    self.seed = new_seed
    old_seed
  end

  def random(limit)
    if undefined.equal?(limit)
      random_float
    else
      if limit.kind_of?(Range)
        if time_value?(limit.min)
          random_time_range(limit)
        else
          random_range(limit)
        end
      elsif limit.kind_of?(Float)
        raise ArgumentError, "invalid argument - #{limit}" if limit <= 0
        random_float * limit
      else
        limit_int = Rubinius::Type.coerce_to limit, Integer, :to_int
        raise ArgumentError, "invalid argument - #{limit}" if limit_int <= 0

        if limit.is_a?(Integer)
          random_integer(limit - 1)
        elsif limit.respond_to?(:to_f)
          random_float * limit
        else
          random_integer(limit_int - 1)
        end
      end
    end
  end

  # Generate a random Float, in the range 0...1.0
  def random_float
    Rubinius.primitive :randomizer_rand_float
    raise PrimitiveFailure, "Randomizer#rand_float primitive failed"
  end

  # Generate a random Integer, in the range 0...limit
  def random_integer(limit)
    Rubinius.primitive :randomizer_rand_int
    raise PrimitiveFailure, "Randomizer#rand_int primitive failed"
  end

  def random_range(limit)
    min, max = limit.max.coerce(limit.min)
    diff = max - min
    diff += 1 if max.kind_of?(Integer)
    random(diff) + min
  end

  def generate_seed
    Rubinius.primitive :randomizer_gen_seed
    raise PrimitiveFailure, "Randomizer#gen_seed primitive failed"
  end

  ##
  # Returns a random value from a range made out of Time, Date or DateTime
  # instances.
  #
  # @param [Range] range
  # @return [Time|Date|DateTime]
  #
  def random_time_range(range)
    min  = time_to_float(range.min)
    max  = time_to_float(range.max)
    time = Time.at(random(min..max))

    if defined?(DateTime) && range.min.is_a?(DateTime)
      time = time.to_datetime
    elsif range.min.is_a?(Date)
      time = time.to_date
    end

    return time
  end

  ##
  # Casts a Time/Date/DateTime instance to a Float.
  #
  # @param [Time|Date|DateTime] input
  # @return [Float]
  #
  def time_to_float(input)
    return input.respond_to?(:to_time) ? input.to_time.to_f : input.to_f
  end

  ##
  # Checks if a given value is a Time, Date or DateTime object.
  #
  # @param [Mixed] input
  # @return [TrueClass|FalseClass]
  #
  def time_value?(input)
    return input.is_a?(Time) || input.is_a?(Date)
  end
end

class Random
  def self.new_seed
    Thread.current.randomizer.generate_seed
  end

  def self.srand(seed=undefined)
    if undefined.equal? seed
      seed = Thread.current.randomizer.generate_seed
    end

    seed = Rubinius::Type.coerce_to seed, Integer, :to_int
    Thread.current.randomizer.swap_seed seed
  end

  def self.rand(limit=undefined)
    Thread.current.randomizer.random(limit)
  end

  def initialize(seed=undefined)
    @randomizer = Rubinius::Randomizer.new
    if !undefined.equal?(seed)
      @randomizer.swap_seed seed.to_int
    end
  end

  def rand(limit=undefined)
    @randomizer.random(limit)
  end

  def seed
    @randomizer.seed
  end

  def state
    @randomizer.seed
  end
  private :state

  def ==(other)
    return false unless other.kind_of?(Random)
    seed == other.seed
  end

  # Returns a random binary string.
  # The argument size specified the length of the result string.
  def bytes(length)
    length = Rubinius::Type.coerce_to length, Integer, :to_int
    s = ''
    i = 0
    while i < length
      s << @randomizer.random_integer(255).chr
      i += 1
    end

    s
  end
end
