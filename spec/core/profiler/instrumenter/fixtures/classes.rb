module ProfilerSpecs
  extend Rubinius::FFI::Library

  attach_function :rand, [], :int

  class EarlyOut < Exception; end

  def self.find_method(data, name)
    data[:methods].each do |key, value|
      return value if value[:name] == name
    end
    nil
  end

  def self.load
    lambda { 10 * 5 ** 2 }
  end

  def self.factor(n)
    100 * n
  end

  def self.method(sym, iterations)
    iterations.times { send sym, iterations }
  end

  def self.work(iterations)
    factor(iterations).times &load
  end

  def self.exceptional(iterations)
    factor(iterations).times &load
    raise EarlyOut
  end

  def self.ffi_function(iterations)
    factor(iterations).times { rand }
  end

  class Block
    def block
      [1, 2, 3, 4, 5].each { |a| 10 * 5 ** a }
    end
  end
end
