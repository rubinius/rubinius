require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  c_string = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00"
  e_string = "\xF3\x02\x00\x42\x32\x87\xF3\x00"
  g_string = "\x00\x00\x00\x42\x32\x87\xF3\x02" # ha
  q_string = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE"
  u_string = "\xF1\x80\x80\xB1\xE1\x8A\x80\xC2\xBF\x0C\x6B"
  v_string = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00"
  n_string = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00"
  
  x.report "string unpack C*" do |times|
    i = 0
    while i < times
      c_string.unpack('C*')
      i += 1
    end
  end

  x.report "string unpack E*" do |times|
    i = 0
    while i < times
      e_string.unpack('E*')
      i += 1
    end
  end

  x.report "string unpack G*" do |times|
    i = 0
    while i < times
      g_string.unpack('G*')
      i += 1
    end
  end

  x.report "string unpack N*" do |times|
    i = 0
    while i < times
      n_string.unpack('N*')
      i += 1
    end
  end

  x.report "string unpack Q*" do |times|
    i = 0
    while i < times
      q_string.unpack('Q*')
      i += 1
    end
  end

  x.report "string unpack U*" do |times|
    i = 0
    while i < times
      u_string.unpack('U*')
      i += 1
    end
  end

  x.report "string unpack V*" do |times|
    i = 0
    while i < times
      v_string.unpack('V*')
      i += 1
    end
  end
end
