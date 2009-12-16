class Apple < Struct; end

Struct.new('Ruby', :version, :platform)
Struct.new('Car', :make, :model, :year)
class Honda < Struct::Car
  def initialize(*args)
    self.make = "Honda"
    super(*args)
  end
end
