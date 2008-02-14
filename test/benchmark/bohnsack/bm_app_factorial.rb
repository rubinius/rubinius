class Integer
  def fact
    f=1
    (2..self).each { |i| f *= i }
    f
  end
end

100.times {5000.fact}
