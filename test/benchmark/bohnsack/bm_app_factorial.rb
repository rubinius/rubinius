class Integer
  def fact
    f=1
    (2..self).each { |i| f *= i }
    f
  end
end

8.times {5000.fact}
