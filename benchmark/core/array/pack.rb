require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  a = ['abcdefghijklmnopqrstuvwxyz']
  b = ["011000010110001001100011"]
  h = ['414243444546414243444546414243444546']
  eightbit = [49, 52, 81, 19, 22, 42, 20, 22]
  sixteenbit = [0xABCD, 0xABCD, 0xABCD, 0xABCD, 0xABCD, 0xABCD, 0xABCD, 0xABCD]
  thirtytwobit = [0xABCDEF01, 0xABCDEF01, 0xABCDEF01, 0xABCDEF01, 0xABCDEF01, 0xABCDEF01, 0xABCDEF01, 0xABCDEF01]
  sixtyfourbit = [0xABCDEF0123456789, 0xABCDEF0123456789, 0xABCDEF0123456789, 0xABCDEF0123456789, 0xABCDEF0123456789, 0xABCDEF0123456789, 0xABCDEF0123456789, 0xABCDEF0123456789]
  float = [-1.0, 0.0, 18.3272, 90.23473827272, 12782.72739, -1.0, 0.0, 18.3272, 90.23473827272, 12782.72739]
  m = ["ABCDEF", "ABCDEF", "ABCDEF", "ABCDEF", "ABCDEF", "ABCDEF", "ABCDEF", "ABCDEF"]
  u = [262193, 4736, 191, 12, 107, 262193, 4736, 191, 12, 107]
  w = [2**64, 2**64, 2**64, 2**64, 2**64, 2**64, 2**64, 2**64]

  x.report "pack A*" do |times|
    i = 0
    while i < times
      a.pack('A*')
      i += 1
    end
  end

  x.report "pack a*" do |times|
    i = 0
    while i < times
      a.pack('a*')
      i += 1
    end
  end

  x.report "pack Z*" do |times|
    i = 0
    while i < times
      a.pack('Z*')
      i += 1
    end
  end

  x.report "pack B*" do |times|
    i = 0
    while i < times
      b.pack('B*')
      i += 1
    end
  end

  x.report "pack b*" do |times|
    i = 0
    while i < times
      b.pack('b*')
      i += 1
    end
  end

  x.report "pack H*" do |times|
    i = 0
    while i < times
      h.pack('H*')
      i += 1
    end
  end

  x.report "pack h*" do |times|
    i = 0
    while i < times
      h.pack('h*')
      i += 1
    end
  end

  x.report "pack C*" do |times|
    i = 0
    while i < times
      eightbit.pack('C*')
      i += 1
    end
  end

  x.report "pack c*" do |times|
    i = 0
    while i < times
      eightbit.pack('c*')
      i += 1
    end
  end

  x.report "pack N*" do |times|
    i = 0
    while i < times
      thirtytwobit.pack('N*')
      i += 1
    end
  end

  x.report "pack n*" do |times|
    i = 0
    while i < times
      sixteenbit.pack('n*')
      i += 1
    end
  end

  x.report "pack V*" do |times|
    i = 0
    while i < times
      thirtytwobit.pack('V*')
      i += 1
    end
  end

  x.report "pack v*" do |times|
    i = 0
    while i < times
      sixteenbit.pack('v*')
      i += 1
    end
  end

  x.report "pack S*" do |times|
    i = 0
    while i < times
      sixteenbit.pack('S*')
      i += 1
    end
  end

  x.report "pack s*" do |times|
    i = 0
    while i < times
      sixteenbit.pack('s*')
      i += 1
    end
  end

  x.report "pack L*" do |times|
    i = 0
    while i < times
      thirtytwobit.pack('L*')
      i += 1
    end
  end

  x.report "pack l*" do |times|
    i = 0
    while i < times
      thirtytwobit.pack('l*')
      i += 1
    end
  end

  x.report "pack Q*" do |times|
    i = 0
    while i < times
      sixtyfourbit.pack('Q*')
      i += 1
    end
  end

  x.report "pack q*" do |times|
    i = 0
    while i < times
      sixtyfourbit.pack('q*')
      i += 1
    end
  end

  x.report "pack I*" do |times|
    i = 0
    while i < times
      thirtytwobit.pack('I*')
      i += 1
    end
  end

  x.report "pack i*" do |times|
    i = 0
    while i < times
      thirtytwobit.pack('i*')
      i += 1
    end
  end

  x.report "pack f*" do |times|
    i = 0
    while i < times
      float.pack('f*')
      i += 1
    end
  end

  x.report "pack d*" do |times|
    i = 0
    while i < times
      float.pack('d*')
      i += 1
    end
  end

  x.report "pack e*" do |times|
    i = 0
    while i < times
      float.pack('e*')
      i += 1
    end
  end

  x.report "pack E*" do |times|
    i = 0
    while i < times
      float.pack('E*')
      i += 1
    end
  end

  x.report "pack g*" do |times|
    i = 0
    while i < times
      float.pack('g*')
      i += 1
    end
  end

  x.report "pack G*" do |times|
    i = 0
    while i < times
      float.pack('G*')
      i += 1
    end
  end

  x.report "pack M*" do |times|
    i = 0
    while i < times
      m.pack('M*')
      i += 1
    end
  end

  x.report "pack m*" do |times|
    i = 0
    while i < times
      m.pack('m*')
      i += 1
    end
  end

  x.report "pack U*" do |times|
    i = 0
    while i < times
      u.pack('U*')
      i += 1
    end
  end

  x.report "pack u*" do |times|
    i = 0
    while i < times
      m.pack('u*')
      i += 1
    end
  end

  x.report "pack w*" do |times|
    i = 0
    while i < times
      w.pack('w*')
      i += 1
    end
  end
end
