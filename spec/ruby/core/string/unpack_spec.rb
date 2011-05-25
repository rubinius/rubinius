require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes.rb', __FILE__)

if ENV['MRI'] then
  $: << 'kernel/core'
  require 'unpack'
end

# FIXME: "according to the format string" is NOT a spec description!

describe "String#unpack" do
  it "returns an array by decoding self according to the format string" do
    "abc \0\0abc \0\0".unpack('A6Z6').should == ["abc", "abc "]
    "abc \0\0".unpack('a3a3').should == ["abc", " \000\000"]
    "aa".unpack('b8B8').should == ["10000110", "01100001"]
    "aaa".unpack('h2H2c').should == ["16", "61", 97]
    "now=20is".unpack('M*').should == ["now is"]
    "whole".unpack('xax2aX2aX1aX2a').should == ["h", "e", "l", "l", "o"]
  end

  little_endian do
    it "returns an array by decoding self in little-endian (native format) order according to the format string" do
      "\xfe\xff\xfe\xff".unpack('sS').should == [-2, 65534]
    end
  end

  big_endian do
    it "returns an array by decoding self in big-endian (native format) order according to the format string" do
      "\xfe\xff\xfe\xff".unpack('sS').should == [-257, 65279]
    end
  end
end

describe "String#unpack with nil format argument" do
  it "raises a TypeError exception" do
    lambda { "abc".unpack(nil) }.should raise_error(TypeError)
  end
end

describe "String#unpack with 'Z' directive" do
  it "returns an array by decoding self according to the format string" do
    "abc \0abc \0".unpack('Z*Z*').should == ["abc ", "abc "]
    "abc \0abc \0".unpack('Z10').should == ["abc "]
    "abc \0abc \0".unpack('Z7Z*').should == ["abc ", "c "]
    "abc \0abc \0".unpack('Z50Z*').should == ["abc ", ""]
    "abc \0\0\0abc \0".unpack('Z*Z*').should == ["abc ", ""]
    "abc \0\0\0\0".unpack('Z*').should == ["abc "]
    "abc \0\0\0\0".unpack('Z*Z*').should == ["abc ", ""]
    "\0".unpack('Z*').should == [""]
    "\0\0".unpack('Z*').should == [""]
    "\0\0abc".unpack('Z*').should == [""]
    "\0\0abc\0\0".unpack('Z*').should == [""]
    "\0 abc\0\0abc\0abc \0\0 ".unpack('Z2Z*Z3ZZ*Z10Z10').should == ["", "abc", "", "c", "", "abc ", ""]
  end
end

describe "String#unpack with 'N' and 'n' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xF3\x02\x00\x42".unpack('N').should == [4076994626]
    "\xF3\x02".unpack('N').should == [nil]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('N2').should == [4076994626, 847770368]
    "\xF3\x02\xC0\x42\x3A\x87\xF3\x00".unpack('N*').should == [4077043778, 981988096]
    "\xF3\x02\x00\x42".unpack('n').should == [62210]
    "\xF3\x02\x00\x42".unpack('n5').should == [62210, 66, nil, nil, nil]
    "\xF3".unpack('n').should == [nil]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('n3').should == [62210, 66, 12935]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('n*').should == [62210, 66, 12935, 62208]
    "\xF3\x02\x00\x42\x32\x87\xF3\x02".unpack('n0N*').should == [4076994626, 847770370]
  end
end

describe "String#unpack with 'V' and 'v' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xF3\x02\x00\x42".unpack('V').should == [1107297011]
    "\xF3\x02".unpack('V').should == [nil]
    "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('V2').should == [4294967283, 133938]
    "\xF3\x02\xC0\x42\x3A\x87\xF3\x00".unpack('V*').should == [1119879923, 15959866]
    "\xF3\x02\x00\x42".unpack('v').should == [755]
    "\xF3\x02\x00\x42".unpack('v5').should == [755, 16896, nil, nil, nil]
    "\xF3".unpack('v').should == [nil]
    "\xF3\xFF\xFF\xFF\x32\x87\xF3\x00".unpack('v3').should == [65523, 65535, 34610]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('v*').should == [755, 16896, 34610, 243]
    "\xF3\x02\x00\x42\x32\x87\xF3\x02".unpack('v0V*').should == [1107297011, 49514290]
  end
end

describe "String#unpack with 'C' and 'c' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xF3\x02\x00\x42".unpack('C').should == [243]
    "".unpack('C').should == [nil]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('C2').should == [243, 2]
    "\xF3\x02\xC0\x42\x3A\x87\xF3\x00".unpack('C*').should == [243, 2, 192, 66, 58, 135, 243, 0]
    "\xF3\x02\x00\x42".unpack('c').should == [-13]
    "\xF3\x02\x00\x42".unpack('c5').should == [-13, 2, 0, 66, nil]
    "\x80".unpack('c').should == [-128]
    "\x7F\x02\x00\x42\x32\x87\xF3\x00".unpack('c3').should == [127, 2, 0]
    "\x80\x02\x00\x42\xFF\x87\xF3\x00".unpack('c*').should == [-128, 2, 0, 66, -1, -121, -13, 0]
    "\xF3\x02\x00\x42\x32\x87\xF3\x02".unpack('c0C*').should == [243, 2, 0, 66, 50, 135, 243, 2]
  end

  it "decodes respective of the already decoded data" do
    "\0\0\0\1hello".unpack("iC5").last.should == 111
  end
end

describe "String#unpack with 'Q' and 'q' directives" do
  little_endian do
    it "returns an array in little-endian (native format) order by decoding self according to the format string" do
      "\xF3\x02\x00\x42\x32\x23\xB3\xF0".unpack('Q').should == [17344245288696546035]
      "\xF3\x02".unpack('Q*').should == []
      "\xF3\x02\xC0\x42\x3A\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('Q*').should ==
        [68547103638422259, 17218254449219272698]
      "\xF3\x02\x00\x42".unpack('q').should == [nil]
      "\xF3\x02\x00\x42".unpack('q5').should == [nil, nil, nil, nil, nil]
      "\xF3".unpack('q').should == [nil]
      "\xF3\xFF\xFF\xFF\xFF\xFA\xF3\xFD".unpack('q3').should == [-147498385354522637, nil, nil]
      "\x0A\x02\x00\x02\x32\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('q*').should ==
        [68547068192358922, -1228489624490278918]
      "\x7F\x77\x77\x77\x77\x77\x77\x77".unpack('q0Q*').should == [8608480567731124095]
    end

    ruby_bug "#", "1.8.7" do
      it "pads nil when the string is short" do
        "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('Q2').should == [575263624658931, nil]
      end
    end
  end

  big_endian do
    it "returns an array in big-endian (native format) order by decoding self according to the format string" do
      "\xF3\x02\x00\x42\x32\x23\xB3\xF0".unpack('Q').should == [17510558585478951920]
      "\xF3\x02".unpack('Q*').should == []
      "\xF3\x02\xC0\x42\x3A\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('Q*').should ==
        [17510769691852272384, 18086174637980906478]
      "\xF3\x02\x00\x42".unpack('q').should == [nil]
      "\x01\x62\xEE\x42".unpack('q-7q').should == [nil, nil]
      "\xF3\x02\x00\x42".unpack('q5').should == [nil, nil, nil, nil, nil]
      "\xF3".unpack('q').should == [nil]
      "\xF3\xFF\xFF\xFF\xFF\xFA\xF3\xFD".unpack('q3').should == [-864691128455465987, nil, nil]
      "\x0A\x02\x00\x02\x32\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('q*').should ==
        [721138899770405632, -360569435728645138]
      "\x7F\x77\x77\x77\x77\x77\x77\x77".unpack('q0Q*').should == [9184941320034547575]
    end

    ruby_version_is "" ... "1.8.8" do
      it "does NOT pad nil when the string is short" do
        "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('Q2').should == [17582052941799031296]
      end
    end

    ruby_version_is "1.8.8" do
      it "pads nil when the string is short" do
        "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('Q2').should == [17582052941799031296, nil]
      end
    end
  end

  little_endian do
    it "returns Bignums for big numeric values on little-endian platforms" do
      "\xF3\x02\x00\x42\x32\x23\xB3\xF0".unpack('Q')[0].class.should ==
        17344245288696546035.class
      "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFE".unpack('q')[0].class.should == -72057594037927937.class
    end
  end

  big_endian do
    it "returns Bignums for big numeric values on big-endian platforms" do
      "\xF3\x02\x00\x42\x32\x23\xB3\xF0".unpack('Q')[0].class.should ==
        17344245288696546035.class
      "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFE".unpack('q')[0].should be_kind_of(Fixnum)
    end
  end

  it "returns Fixnums for small numeric values" do
    "\x00\x00\x00\x00\x00\x00\x00\x00".unpack('Q').should == [0]
    "\x00\x00\x00\x00\x00\x00\x00\x00".unpack('q').should == [0]
    "\x00\x00\x00\x00\x00\x00\x00\x00".unpack('Q')[0].should be_kind_of(Fixnum)
    "\x00\x00\x00\x00\x00\x00\x00\x00".unpack('q')[0].should be_kind_of(Fixnum)
  end
end

describe "String#unpack with 'a', 'X' and 'x' directives" do
  it "returns an array by decoding self according to the format string" do
    "abc".unpack('x3a').should == [""]
    "abc".unpack('x3X3x0a').should == ["a"]
    "abcdefghijklmnopqrstuvwxyz".unpack('aaax10aX14X0a').should == ["a", "b", "c", "n", "a"]
    "abcdefghijklmnopqrstuvwxyz".unpack('x*aX26a').should == ["", "a"]
    "abcdefghijklmnopqrstuvwxyz".unpack('x*x*x*aX26a').should == ["", "a"]
    "abc".unpack('x3X*').should == []
    "abc".unpack('x3X*a').should == [""]
    "abc".unpack('x3X*X3a').should == ["a"]
    lambda { "abc".unpack('X*') }.should raise_error(ArgumentError)
    lambda { "abc".unpack('x*x') }.should raise_error(ArgumentError)
    lambda { "abc".unpack('x4') }.should raise_error(ArgumentError)
    lambda { "abc".unpack('X') }.should raise_error(ArgumentError)
    lambda { "abc".unpack('xX*') }.should raise_error(ArgumentError)
  end
end

describe "String#unpack with 'DdEeFfGg' directives" do
  before :each do
    @precision_small = 1E-12
    @precision_large = 1E+17
  end

  it "returns an array by decoding self according to the format string" do
    res = "\xF3\x02\x00\x42\xF3\x02\x00\x42".unpack('eg')
    res.length.should == 2
    res[0].should be_close(32.0028800964355, @precision_small)
    res[1].should be_close(-1.02997409159585e+31, @precision_large)

    res = "\xF3\x02\x00\x42\xF3\x02\x00\x42".unpack('eg')
    res.length.should == 2
    res[0].should be_close(32.0028800964355, @precision_small)
    res[1].should be_close(-1.02997409159585e+31, @precision_large)

    "\xF3\x02".unpack('GD').should == [nil, nil]

#     "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('E*')[0].should be_close(
#         4.44943241769783e-304, @precision_small)
#     "\x00\x00\x00\x42\x32\x87\xF3\x02".unpack('g0G*')[0].should be_close(
#         1.40470576419087e-312, @precision_small)
  end

  little_endian do
    it "returns an array by decoding self in little-endian order according to the format string" do
      # 'F2' pattern
      res = "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('F2')
      res.length.should == 2
      res[0].nan?.should == true
      res[1].should be_close(1.87687113714737e-40, @precision_small)

      # 'f*' pattern
      res = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00".unpack('f*')
      res.length.should == 2
      res[0].should be_close(96.0057601928711, @precision_small)
      res[1].should be_close(2.23645357166299e-38, @precision_small)

      # We expect -Infinity to be the result, i.e. a Float object named
      # -Infinity. There doesn't seem to be any way to declare this
      # explicitly, so we generate it by summing the maximum Float value and
      # inverting the result...
      "\xFF\x80\x00\x00".unpack('g')[0].should == -(Float::MAX + Float::MAX)
      "\x00\x00\x00\x00".unpack('f5').should == [0.0, nil, nil, nil, nil]
      "\xF3".unpack('E').should == [nil]

      # 'd3' pattern
      res = "\xF3\xFF\xFF\xFF\x32\x87\xF3\x00".unpack('d3')
      res.length.should == 3
#       res[0].should be_close(4.44943499804409e-304, @precision_small)
      res[1].should == nil
      res[2].should == nil
    end
  end

  big_endian do
    it "returns an array by decoding self in big-endian order according to the format string" do
      # 'F2' pattern
      res = "\xFF\xFF\xFF\xF3\x00\x02\x0B\x32".unpack('F2')
      res.length.should == 2
      res[0].nan?.should == true
      res[1].should be_close(1.87687113714737e-40, @precision_small)

      # 'f*' pattern
      res = "\x42\xC0\x02\xF3\x00\xF3\x87\x3A".unpack('f*')
      res.length.should == 2
      res[0].should be_close(96.0057601928711, @precision_small)
      res[1].should be_close(2.23645357166299e-38, @precision_small)

      # 'd3' pattern
      res = "\x00\xF3\x87\x32\xFF\xFF\xFF\xF3".unpack('d3')
      res.length.should == 3
#       res[0].should be_close(4.44943499804409e-304, @precision_small)
      res[1].should == nil
      res[2].should == nil
    end
  end
end

describe "String#unpack with 'B' and 'b' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('B64').should ==
      ["1111111100000000100000000000000101111110101010101111000000001111"]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('b64').should ==
      ["1111111100000000000000011000000001111110010101010000111111110000"]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('B12b12B12b12B*b*').should ==
      ["111111110000", "000000011000", "011111101010", "000011111111", "", ""]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('B4b4B4b4B4b4B16').should ==
      ["1111", "0000", "1000", "1000", "0111", "0101", "1111000000001111"]
    "\xFF\x00\x80\xAA".unpack('B0b0').should == ["", ""]
    "\xFF\x00\x80\xAA".unpack('B3b*B*').should == ["111", "000000000000000101010101", ""]
    "\xFF\x00\x80\xAA".unpack('B3B*b*').should == ["111", "000000001000000010101010", ""]
  end
end

describe "String#unpack with 'H' and 'h' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('H16').should == ["ff0080017eaaf00f"]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('h16').should == ["ff000810e7aa0ff0"]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('H3h3H3h3H*h*').should ==
      ["ff0", "081", "7ea", "0ff", "", ""]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('HhHhHhH4').should ==
      ["f", "0", "8", "1", "7", "a", "f00f"]
    "\xFF\x00\x80\xAA".unpack('H0h0').should == ["", ""]
    "\xFF\x00\x80\xAA".unpack('H3h*H*').should == ["ff0", "08aa", ""]
    "\xFF\x00\x80\xAA".unpack('H3H*h*').should == ["ff0", "80aa", ""]
  end
end

describe "String#unpack with 'IiLlSs' directives" do
  platform_is :wordsize => 32 do
    little_endian do
      it "returns an array in little-endian (native format) by decoding self according to the format string" do
        "\xF3\x02\x00\x42\x32\x23\xB3\xF0".unpack('SLI').should == [755, 590496256, nil]
        "\xF3\x02".unpack('L*I*').should == []
        "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('I2').should == [4294967283, 133938]
        "\xF3\x02\xC0\x42\x3A\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('L*').should ==
          [1119879923, 15959866, 33619962, 4008937266]
        "\xF3\x02\x00\x42".unpack('i').should == [1107297011]
        "\xF3\x02\x00\x42".unpack('s5').should == [755, 16896, nil, nil, nil]
        "\xF3".unpack('s').should == [nil]
        "\xF3\xFF\xFF\xFF\xFF\xFA\xF3\xFD".unpack('i3').should == [-13, -34342145, nil]
        "\x0A\x02\x00\x02\x32\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('l*').should ==
          [33554954, 15959858, 33619962, -286030030]
        "\x7F\x77\x77\x77\x77\x77\x77\x77".unpack('i0I*').should == [2004318079, 2004318071]
      end
    end

    big_endian do
      it "returns an array in big-endian (native format) by decoding self according to the format string" do
        "\xF3\x02\x00\x42\x32\x23\xB3\xF0".unpack('SLI').should == [62210, 4338211, nil]
        "\xF3\x02".unpack('L*I*').should == []
        "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('I2').should == [4093640703, 839582208]
        "\xF3\x02\xC0\x42\x3A\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('L*').should ==
          [4077043778, 981988096, 4211015682, 847770606]
        "\xF3\x02\x00\x42".unpack('i').should == [-217972670]
        "\x01\x62\xEE\x42".unpack('l-7l').should == [23260738, nil]
        "\xF3\x02\x00\x42".unpack('s5').should == [-3326, 66, nil, nil, nil]
        "\xF3".unpack('s').should == [nil]
        "\xF3\xFF\xFF\xFF\xFF\xFA\xF3\xFD".unpack('i3').should == [-201326593, -330755, nil]
        "\x0A\x02\x00\x02\x32\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('l*').should ==
          [167903234, 847770368, -83951614, 847770606]
        "\x7F\x77\x77\x77\x77\x77\x77\x77".unpack('i0I*').should == [2138535799, 2004318071]
      end
    end
  end

  it "uses sizeof(int) as an integer" do
    little_endian do
      "\000\000\001\000".unpack("i").should == [65536]
      "\000\000\001\000\000\000\001\000".unpack("i2").should == [65536, 65536]
      "\000\000\001\000\000\000\001\000hello".unpack("i2a5").should == [65536, 65536, "hello"]
      "\377\377\377\377".unpack("i").should == [-1]
      "\377\377\377\377".unpack("I").should == [4294967295]
      "\000\000\000\000\000\000\000\000".unpack("I*").should == [0,0]
    end
    big_endian do
      "\000\001\000\000".unpack("i").should == [65536]
      "\000\001\000\000\000\001\000\000".unpack("i2").should == [65536, 65536]
      "\000\001\000\000\000\001\000\000hello".unpack("i2a5").should == [65536, 65536, "hello"]
      "\377\377\377\377".unpack("i").should == [-1]
      "\377\377\377\377".unpack("I").should == [4294967295]
      "\000\000\000\000\000\000\000\000".unpack("I*").should == [0,0]
    end
  end

  it "ignores the result if there aren't 4 bytes" do
    little_endian do
      "\000".unpack("I").should == [nil]
      "\000".unpack("I2").should == [nil, nil]
      "\000".unpack("I*").should == []
      "\000\000\000\000\000\000\000\000\000".unpack("I*").should == [0,0]
    end
    big_endian do
      "\000".unpack("I").should == [nil]
      "\000".unpack("I2").should == [nil, nil]
      "\000".unpack("I*").should == []
      "\000\000\000\000\000\000\000\000\000".unpack("I*").should == [0,0]
    end
  end
end

describe "String#unpack with 'lL'" do
  it "uses 4 bytes for an integer" do
    little_endian do
      "\000\000\001\000".unpack("l").should == [65536]
      "\000\000\001\000\000\000\001\000".unpack("l2").should == [65536, 65536]
      "\000\000\001\000\000\000\001\000hello".unpack("l2a5").should == [65536, 65536, "hello"]
      "\377\377\377\377".unpack("l").should == [-1]
      "\377\377\377\377".unpack("L").should == [4294967295]
    end
    big_endian do
      "\000\001\000\000".unpack("l").should == [65536]
      "\000\001\000\000\000\001\000\000".unpack("l2").should == [65536, 65536]
      "\000\001\000\000\000\001\000\000hello".unpack("l2a5").should == [65536, 65536, "hello"]
      "\377\377\377\377".unpack("l").should == [-1]
      "\377\377\377\377".unpack("L").should == [4294967295]
    end
  end

  it "ignores the result if there aren't 4 bytes" do
    little_endian do
      "\000".unpack("L").should == [nil]
      "\000".unpack("L2").should == [nil, nil]
      "\000".unpack("L*").should == []
      "\000\000\000\000\000\000\000\000\000".unpack("I*").should == [0,0]
    end
    big_endian do
      "\000".unpack("L").should == [nil]
      "\000".unpack("L2").should == [nil, nil]
      "\000".unpack("L*").should == []
      "\000\000\000\000\000\000\000\000\000".unpack("I*").should == [0,0]
    end
  end
end

describe "String#unpack with 'U' directive" do
  it "returns an array by decoding self according to the format string" do
    "\xFD\x80\x80\xB7\x80\x80".unpack('U').should == [1073967104]
    "\xF9\x80\x80\x80\x80".unpack('U').should == [16777216]
    "\xF1\x80\x80\x80".unpack('UU').should == [262144]
    "\xE1\xB7\x80".unpack('U').should == [7616]
    "\x00\x7F".unpack('U100').should == [0, 127]
    "\x05\x7D".unpack('U0U0').should == []
    "".unpack('U').should == []
    "\xF1\x80\x80\xB1\xE1\x8A\x80\xC2\xBF\x0C\x6B".unpack('U*').should == [262193, 4736, 191, 12, 107]
    "\xF1\x8F\x85\xB1\xE1\x8A\x89\xC2\xBF\x0C\x6B".unpack('U2x2U').should == [323953, 4745, 12]
    lambda { "\xF0\x80\x80\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xE0\x80\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xC0\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xC1\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xF1\x80\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xE1\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xC2".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xF1\x00\x00\x00".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xE1\x00\x00".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xC2\x00".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xFE".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\x03\xFF".unpack('UU') }.should raise_error(ArgumentError)
  end
end

describe "String#unpack with 'A' directive" do
  it "returns an array by decoding self according to the format string" do
    "".unpack('A').should == [""]
    " \0 abc \0\0\0abc\0\0 \0abc".unpack('A*').should ==
      [" \000 abc \000\000\000abc\000\000 \000abc"]
    " \0 abc \0\0\0abc\0\0 \0abc \0 a ".unpack('A3A4AAA').should == ["", "abc", "", "", ""]
    " \0 abc \0\0\0abc\0\0 \0abc \0 a ".unpack('A2A0A14').should == ["", "", " abc \000\000\000abc"]
  end
end

describe "String#unpack with '@' directive" do
  it "returns an array by decoding self according to the format string" do
    "abcdefg".unpack('@2').should     == []
    "abcdefg".unpack('@3@5a').should  == ["f"]
    "abcdefg".unpack('@7a').should    == [""]
    lambda { "abcdefg".unpack('@8') }.should raise_error(ArgumentError)
  end
end

describe "String#unpack with 'M' directive" do
  it "decodes the rest of the string as MIMEs quoted-printable" do
    "".unpack('M').should == [""]
    "=5".unpack('Ma').should == ["", ""]
    "abc=".unpack('M').should == ["abc"]
    "a=*".unpack('MMMM').should == ["a", "*", "", ""]

    "\x3D72=65abcdefg=5%=33".unpack('M').should    == ["reabcdefg"]
    "\x3D72=65abcdefg=5%=33".unpack('M0').should   == ["reabcdefg"]
    "\x3D72=65abcdefg=5%=33".unpack('M100').should == ["reabcdefg"]
    "\x3D72=65abcdefg=5%=33".unpack('M*').should   == ["reabcdefg"]

    "abc===abc".unpack('MMMMM').should == ["abc", "", "\253c", "", ""]
    "=$$=47".unpack('MMMM').should == ["", "$$G", "", ""]
    "=$$=4@=47".unpack('MMMMM').should == ["", "$$", "@G", "", ""]
    "=$$=4@=47".unpack('M5000').should == [""]
    "=4@".unpack('MMM').should == ["", "@", ""]
  end

  it "decodes across new lines" do
    input = "A fax has arrived from remote ID ''.=0D=0A-----------------------=\r\n-------------------------------------=0D=0ATime: 3/9/2006 3:50:52=\r\n PM=0D=0AReceived from remote ID: =0D=0AInbound user ID XXXXXXXXXX, r=\r\nouting code XXXXXXXXX=0D=0AResult: (0/352;0/0) Successful Send=0D=0AP=\r\nage record: 1 - 1=0D=0AElapsed time: 00:58 on channel 11=0D=0A"

    expected = "A fax has arrived from remote ID ''.\r\n------------------------------------------------------------\r\nTime: 3/9/2006 3:50:52 PM\r\nReceived from remote ID: \r\nInbound user ID XXXXXXXXXX, routing code XXXXXXXXX\r\nResult: (0/352;0/0) Successful Send\r\nPage record: 1 - 1\r\nElapsed time: 00:58 on channel 11\r\n"

    input.unpack("M").first.should == expected

    "abc=02def=\ncat=\n=01=\n".unpack("M9M3M4").should == ["abc\002defcat\001", "", ""]
  end
end

describe "String#unpack with 'm' directive" do
  it "returns an array by decoding self according to the format string" do
    "YQ==".unpack('m').should == ["a"]
    "YWE=".unpack('m').should == ["aa"]
    "ab c=awerB2y+".unpack('mmmmm').should == ["i\267", "k\a\253\al\276", "", "", ""]
    "a=b=c=d=e=f=g=".unpack('mamamamam').should ==
      ["i", "=", "q", "=", "y", "=", "", "", ""]
    "a===b===c===d===e===f===g===".unpack('mamamamam').should ==
      ["i", "=", "q", "=", "y", "=", "", "", ""]
    "ab c= de f= gh i= jk l=".unpack('mmmmmmmmmm').should ==
      ["i\267", "u\347", "\202\030", "\216I", "", "", "", "", "", ""]
    "+/=\n".unpack('mam').should           == ["\373", "=", ""]
    "aGk=aGk=aGk=".unpack('m*mm').should   == ["hi", "hi", "hi"]
    "aA".unpack('m55').should              == [""]
    "aGk".unpack('m').should               == [""]
    "/w==".unpack('m').should              == ["\377"]
    "Pj4+".unpack('m').should              == [">>>"]
    "<>:?Pj@$%^&*4+".unpack('m').should    == [">>>"]
    "<>:?Pja@$%^&*4+".unpack('ma').should  == [">6\270", ""]
    "<>:?P@$%^&*+".unpack('ma').should     == ["", ""]
    "54321".unpack('m').should             == ["\347\215\366"]
    "==43".unpack('m').should              == [""]
    "43aw".unpack('mmm').should            == ["\343v\260", "", ""]
    "=======43aw".unpack('m').should       == ["\343v\260"]
    "cmVxdWlyZSAnYmFzZTY0Jw==".unpack('m').should == ["require 'base64'"]
    "+/=".unpack('m').should == ["\373"]
    "YXNkb2Zpc09BSVNERk9BU0lESjk4ODc5ODI0YWlzdWYvLy8rKw==".unpack('m').should ==
      ["asdofisOAISDFOASIDJ98879824aisuf///++"]
    "IUAjJSMgJCBeJV4qJV4oXiYqKV8qKF8oKStQe308Pj9LTCJLTCI6\n".unpack('m').should ==
      ["!@#$@#%# $ ^%^*%^(^&*)_*(_()+P{}<>?KL\"KL\":"]
    "sfj98349//+ASDd98934jg+N,CBMZP2133GgHJiYrB12".unpack('m').should ==
      ["\261\370\375\363~=\377\377\200H7}\363\335\370\216\017\215\b\023\031?mw\334h\a&&+"]
  end
end

describe "String#unpack with 'w' directive" do
  it "produces a BER-compressed integer" do
    'X'.unpack('w').should == [88]
    'XYZ'.unpack('www').should == [88,89,90]
    'XYZ'.unpack('w3').should == [88,89,90]
    '\001\002\003\004'.unpack('w4').should == [92,48,48,49]
    'hello world'.unpack('w*').should == [104,101,108,108,111,32,119,111,114,108,100]
    "\204\314\330\205R".unpack('w').should == [1234567890]
  end
end

describe "String#unpack with 'P' directive" do
  it "returns a random object after consume a words worth of byte" do
    lambda {
      if 1.size == 8
        "\0\0\0\0\0\0\0\0".unpack("P")
      else
        "\0\0\0\0".unpack("P")
      end
    }.should_not raise_error(Exception)
  end
end
