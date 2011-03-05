require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  ampersand_string = "abcdefghijklmnopqrstuvwxyz"
  a_string = " \0 abc \0\0\0abc\0\0 \0abc"
  b_string = "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F"
  c_string = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00"
  e_string = "\xF3\x02\x00\x42\x32\x87\xF3\x00"
  f_string = "\x42\xC0\x02\xF3\x00\xF3\x87\x3A"
  g_string = "\x00\x00\x00\x42\x32\x87\xF3\x02" # ha
  h_string = "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F"
  ils_string = "\x7F\x77\x77\x77\x77\x77\x77\x77"
  m_string = "\x3D72=65abcdefg=5%=33"
  m2_string = "aGk=aGk=aGk=aGk=aGk=aGk=aGk=aGk=aGk="
  q_string = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE"
  u_string = "\xF1\x80\x80\xB1\xE1\x8A\x80\xC2\xBF\x0C\x6B"
  v_string = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00"
  n_string = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00"
  w_string = 'hello world hello world hello world hello world'
  z_string = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  
  x.report "string unpack @*" do |times|
    i = 0
    while i < times
      ampersand_string.unpack('@*')
      i += 1
    end
  end
  
  x.report "string unpack A*" do |times|
    i = 0
    while i < times
      a_string.unpack('A*')
      i += 1
    end
  end
  
  x.report "string unpack B*" do |times|
    i = 0
    while i < times
      b_string.unpack('B*')
      i += 1
    end
  end
  
  x.report "string unpack b*" do |times|
    i = 0
    while i < times
      b_string.unpack('b*')
      i += 1
    end
  end
  
  x.report "string unpack C*" do |times|
    i = 0
    while i < times
      c_string.unpack('C*')
      i += 1
    end
  end
  
  x.report "string unpack c*" do |times|
    i = 0
    while i < times
      c_string.unpack('c*')
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

  x.report "string unpack e*" do |times|
    i = 0
    while i < times
      e_string.unpack('e*')
      i += 1
    end
  end

  x.report "string unpack F*" do |times|
    i = 0
    while i < times
      e_string.unpack('F*')
      i += 1
    end
  end

  x.report "string unpack f*" do |times|
    i = 0
    while i < times
      e_string.unpack('f*')
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

  x.report "string unpack g*" do |times|
    i = 0
    while i < times
      g_string.unpack('g*')
      i += 1
    end
  end

  x.report "string unpack H*" do |times|
    i = 0
    while i < times
      h_string.unpack('H*')
      i += 1
    end
  end

  x.report "string unpack h*" do |times|
    i = 0
    while i < times
      h_string.unpack('h*')
      i += 1
    end
  end

  x.report "string unpack I*" do |times|
    i = 0
    while i < times
      ils_string.unpack('I*')
      i += 1
    end
  end

  x.report "string unpack i*" do |times|
    i = 0
    while i < times
      ils_string.unpack('i*')
      i += 1
    end
  end

  x.report "string unpack L*" do |times|
    i = 0
    while i < times
      ils_string.unpack('L*')
      i += 1
    end
  end

  x.report "string unpack l*" do |times|
    i = 0
    while i < times
      ils_string.unpack('l*')
      i += 1
    end
  end

  x.report "string unpack M*" do |times|
    i = 0
    while i < times
      m_string.unpack('M*')
      i += 1
    end
  end

  x.report "string unpack m*" do |times|
    i = 0
    while i < times
      m2_string.unpack('m*')
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

  x.report "string unpack n*" do |times|
    i = 0
    while i < times
      n_string.unpack('n*')
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

  x.report "string unpack q*" do |times|
    i = 0
    while i < times
      q_string.unpack('q*')
      i += 1
    end
  end

  x.report "string unpack S*" do |times|
    i = 0
    while i < times
      ils_string.unpack('S*')
      i += 1
    end
  end

  x.report "string unpack s*" do |times|
    i = 0
    while i < times
      ils_string.unpack('s*')
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

  x.report "string unpack v*" do |times|
    i = 0
    while i < times
      v_string.unpack('v*')
      i += 1
    end
  end

  x.report "string unpack w*" do |times|
    i = 0
    while i < times
      w_string.unpack('w*')
      i += 1
    end
  end

  x.report "string unpack Z*" do |times|
    i = 0
    while i < times
      z_string.unpack('Z*')
      i += 1
    end
  end
end
