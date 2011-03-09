require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  lowercase_o = "aaaabbbbccccddddeeeeffff"
  uppercase_o = "AAAABBBBCCCCDDDDEEEEFFFF"
  mixedcase_o = "AaAaBbBbCcCcDdDdEeEeFfFf"

# String#upcase
  x.report "string upcase from lowercase" do |times|
    i = 0
    lowercase = lowercase_o.dup
    while i < times
      lowercase.upcase
      i += 1
    end
  end

  x.report "string upcase from uppercase" do |times|
    i = 0
    uppercase = uppercase_o.dup
    while i < times
      uppercase.upcase
      i += 1
    end
  end

  x.report "string upcase from mixedcase" do |times|
    i = 0
    mixedcase = mixedcase_o.dup
    while i < times
      mixedcase.upcase
      i += 1
    end
  end

# String#upcase!
  x.report "string upcase! from lowercase" do |times|
    i = 0
    lowercase = lowercase_o.dup
    while i < times
      lowercase.upcase!
      i += 1
    end
  end

  x.report "string upcase! from uppercase" do |times|
    i = 0
    uppercase = uppercase_o.dup
    while i < times
      uppercase.upcase!
      i += 1
    end
  end

  x.report "string upcase! from mixedcase" do |times|
    i = 0
    mixedcase = mixedcase_o.dup
    while i < times
      mixedcase.upcase!
      i += 1
    end
  end

# String#downcase
  x.report "string downcase from lowercase" do |times|
    i = 0
    lowercase = lowercase_o.dup
    while i < times
      lowercase.downcase
      i += 1
    end
  end

  x.report "string downcase from uppercase" do |times|
    i = 0
    uppercase = uppercase_o.dup
    while i < times
      uppercase.downcase
      i += 1
    end
  end

  x.report "string downcase from mixedcase" do |times|
    i = 0
    mixedcase = mixedcase_o.dup
    while i < times
      mixedcase.downcase
      i += 1
    end
  end

# String#downcase!
  x.report "string downcase! from lowercase" do |times|
    i = 0
    lowercase = lowercase_o.dup
    while i < times
      lowercase.downcase!
      i += 1
    end
  end

  x.report "string downcase! from uppercase" do |times|
    i = 0
    uppercase = uppercase_o.dup
    while i < times
      uppercase.downcase!
      i += 1
    end
  end

  x.report "string downcase! from mixedcase" do |times|
    i = 0
    mixedcase = mixedcase_o.dup
    while i < times
      mixedcase.downcase!
      i += 1
    end
  end

  # String#swapcase
  x.report "string swapcase from lowercase" do |times|
    i = 0
    lowercase = lowercase_o.dup
    while i < times
      lowercase.swapcase
      i += 1
    end
  end

  x.report "string swapcase from uppercase" do |times|
    i = 0
    uppercase = uppercase_o.dup
    while i < times
      uppercase.swapcase
      i += 1
    end
  end

  x.report "string swapcase from mixedcase" do |times|
    i = 0
    mixedcase = mixedcase_o.dup
    while i < times
      mixedcase.swapcase
      i += 1
    end
  end

# String#swapcase!
  x.report "string swapcase! from lowercase" do |times|
    i = 0
    lowercase = lowercase_o.dup
    while i < times
      lowercase.swapcase!
      i += 1
    end
  end

  x.report "string swapcase! from uppercase" do |times|
    i = 0
    uppercase = uppercase_o.dup
    while i < times
      uppercase.swapcase!
      i += 1
    end
  end

  x.report "string swapcase! from mixedcase" do |times|
    i = 0
    mixedcase = mixedcase_o.dup
    while i < times
      mixedcase.swapcase!
      i += 1
    end
  end


end
