require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  lowercase_o = "aaaabbbbccccddddeeeeffff"
  uppercase_o = "AAAABBBBCCCCDDDDEEEEFFFF"
  mixedcase_o = "AaAaBbBbCcCcDdDdEeEeFfFf"

  lowercase = lowercase_o.dup
  uppercase = uppercase_o.dup
  mixedcase = mixedcase_o.dup
  
  # String#upcase
  x.report "string upcase from lowercase" do |times|
    i = 0
    while i < times
      lowercase.upcase
      i += 1
    end
  end

  x.report "string upcase from uppercase" do |times|
    i = 0
    while i < times
      uppercase.upcase
      i += 1
    end
  end

  x.report "string upcase from mixedcase" do |times|
    i = 0
    while i < times
      mixedcase.upcase
      i += 1
    end
  end

  # String#upcase!
  x.report "string upcase! from lowercase" do |times|
    i = 0
    while i < times
      lowercase = lowercase_o.dup
      lowercase.upcase!
      i += 1
    end
  end

  x.report "string upcase! from uppercase" do |times|
    i = 0
    while i < times
      uppercase = uppercase_o.dup
      uppercase.upcase!
      i += 1
    end
  end

  x.report "string upcase! from mixedcase" do |times|
    i = 0
    while i < times
      mixedcase = mixedcase_o.dup
      mixedcase.upcase!
      i += 1
    end
  end

  # String#downcase
  x.report "string downcase from lowercase" do |times|
    i = 0
    while i < times
      lowercase.downcase
      i += 1
    end
  end

  x.report "string downcase from uppercase" do |times|
    i = 0
    while i < times
      uppercase.downcase
      i += 1
    end
  end

  x.report "string downcase from mixedcase" do |times|
    i = 0
    while i < times
      mixedcase.downcase
      i += 1
    end
  end

  # String#downcase!
  x.report "string downcase! from lowercase" do |times|
    i = 0
    while i < times
      lowercase = lowercase_o.dup
      lowercase.downcase!
      i += 1
    end
  end

  x.report "string downcase! from uppercase" do |times|
    i = 0
    while i < times
      uppercase = uppercase_o.dup
      uppercase.downcase!
      i += 1
    end
  end

  x.report "string downcase! from mixedcase" do |times|
    i = 0
    while i < times
      mixedcase = mixedcase_o.dup
      mixedcase.downcase!
      i += 1
    end
  end

  # String#swapcase
  x.report "string swapcase from lowercase" do |times|
    i = 0
    while i < times
      lowercase.swapcase
      i += 1
    end
  end

  x.report "string swapcase from uppercase" do |times|
    i = 0
    while i < times
      uppercase.swapcase
      i += 1
    end
  end

  x.report "string swapcase from mixedcase" do |times|
    i = 0
    while i < times
      mixedcase.swapcase
      i += 1
    end
  end

  # String#swapcase!
  x.report "string swapcase! from lowercase" do |times|
    i = 0
    while i < times
      lowercase = lowercase_o.dup
      lowercase.swapcase!
      i += 1
    end
  end

  x.report "string swapcase! from uppercase" do |times|
    i = 0
    while i < times
      uppercase = uppercase_o.dup
      uppercase.swapcase!
      i += 1
    end
  end

  x.report "string swapcase! from mixedcase" do |times|
    i = 0
    while i < times
      mixedcase = mixedcase_o.dup
      mixedcase.swapcase!
      i += 1
    end
  end

end

