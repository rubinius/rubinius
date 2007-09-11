class Apple < Struct; end

failure :rbx do
  Struct.new('Ruby', :version, :platform)
  Struct.new('Car', :make, :model, :year)
end

